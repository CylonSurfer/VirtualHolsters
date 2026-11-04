
#include "RockFunctionHelpers.h"
#include "f4se/GameForms.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <ROCK/Client.h>
#include <ROCK/Grab.h>
#include <ROCK/Weapon.h>

#include <cstdint>


namespace Holsters {

	namespace {
		namespace api = rock::api;
		api::Client s_rock;
		const api::grab::ApiV1* s_grab = nullptr;
		const api::weapon::ApiV1* s_weapon = nullptr;

		bool RockConnected() {
			return s_rock.owner() != 0 && s_grab && s_weapon;
		}

		bool ReadRockFrame(api::core::SnapshotV1& snapshot) {
			if (!RockConnected()) {
				return false;
			}
			const auto status = s_rock.core()->getFrameSnapshot(s_rock.owner(), &snapshot);
			return status == api::Status::Ok && snapshot.providerReady != 0;
		}

		bool ReadWeaponHandling(api::weapon::EquippedWeaponHandlingStateV1& state) {
			if (!RockConnected()) {
				return false;
			}
			return s_weapon->getEquippedWeaponHandlingStateV1(s_rock.owner(), &state) == api::Status::Ok;
		}
	}

	bool InitializeRock() {
		if (RockConnected()) {
			return true;
		}
		if (s_rock.owner() == 0) {
			const auto module = GetModuleHandleA("ROCK.dll");
			if (!module) {
				return false; // Retry after ROCK is loaded.
			}
			const auto query = reinterpret_cast<api::QueryInterfaceV1>(GetProcAddress(module, api::kQueryExportName));
			if (!query) {
				_MESSAGE("Virtual Holsters: ROCK modular API export is unavailable");
				return false;
			}
			const auto status = s_rock.connect(query, "VirtualHolsters");
			if (status != api::Status::Ok) {
				_MESSAGE("Virtual Holsters: ROCK registration failed: %u", static_cast<unsigned>(status));
				return false;
			}
		}
		// Retain an existing owner across binding retries. Do not register
		// another owner with the same mod name after a partial initialization.
		if (!s_grab) {
			constexpr auto permissions =
				static_cast<std::uint32_t>(api::grab::PermissionV1::Read) |
				static_cast<std::uint32_t>(api::grab::PermissionV1::Write);
			const auto status = s_rock.acquire(permissions, s_grab);
			if (status != api::Status::Ok) {
				_MESSAGE( "Virtual Holsters: ROCK Grab binding failed: %u", static_cast<unsigned>(status));
				return false;
			}
		}
		if (!s_weapon) {
			constexpr auto permissions = static_cast<std::uint32_t>(api::weapon::PermissionV1::Read);
			const auto status = s_rock.acquire(permissions, s_weapon);
			if (status != api::Status::Ok) {
				_MESSAGE( "Virtual Holsters: ROCK Weapon binding failed: %u", static_cast<unsigned>(status));
				return false;
			}
		}
		_MESSAGE("Virtual Holsters: Registered with ROCK modular API");
		return true;
	}

	bool ShutdownRock() {
		// Call on ROCK's owner thread after stopping callers of this bridge.
		const auto status = s_rock.close();
		if (status != api::Status::Ok &&
			status != api::Status::OwnerNotRegistered) {
			_MESSAGE("Virtual Holsters: ROCK shutdown failed: %u", static_cast<unsigned>(status));
			return false;
		}
		s_grab = nullptr;
		s_weapon = nullptr;
		return true;
	}

	void RockGrabItem(bool isLeft, TESObjectREFR* obj) {
		if (!obj) {
			return;
		}
		api::core::SnapshotV1 snapshot{};
		if (!ReadRockFrame(snapshot)) {
			return;
		}
		api::grab::ForceGrabRequestV1 request{};
		request.hand = isLeft ? api::Hand::Left : api::Hand::Right;
		request.targetFormId = obj->formID;
		request.targetBodyId = 0x7FFFFFFF;
		request.worldGeneration = snapshot.worldGeneration;
		request.skeletonGeneration = snapshot.skeletonGeneration;
		request.providerGeneration = snapshot.providerGeneration;
		request.maxDistanceGame = 100.0f;

		std::uint64_t commandId{};
		const auto admitted = s_grab->requestForceGrabV1(s_rock.owner(), &request, &commandId);
		if (admitted != api::Status::RequestQueued) {
			_MESSAGE( "Virtual Holsters: ROCK force grab rejected: %u", static_cast<unsigned>(admitted));
		}
		// RequestQueued means accepted for later execution.
		// commandId identifies the request for getInteractionCommandResultV1.
	}

	void RockForceDropItem(bool isLeft, TESObjectREFR* obj) {
		if (!obj) {
			return;
		}
		api::core::SnapshotV1 snapshot{};
		if (!ReadRockFrame(snapshot)) {
			return;
		}
		api::grab::ForceReleaseRequestV1 request{};
		request.hand = isLeft ? api::Hand::Left : api::Hand::Right;
		request.targetFormId = obj->formID;
		request.targetBodyId = 0x7FFFFFFF;
		request.worldGeneration = snapshot.worldGeneration;
		request.skeletonGeneration = snapshot.skeletonGeneration;
		request.providerGeneration = snapshot.providerGeneration;
		// Preserve the original flags = 0 release behavior.
		// Supplying targetFormId alone does not require a matching target.
		std::uint64_t commandId{};
		const auto admitted = s_grab->requestForceReleaseV1(s_rock.owner(), &request, &commandId);
		if (admitted != api::Status::RequestQueued) {
			_MESSAGE(
				"Virtual Holsters: ROCK force release rejected: %u",
				static_cast<unsigned>(admitted));
		}
	}

	TESObjectREFR* RockGetHeld(bool isLeft) {
		if (!RockConnected()) {
			return nullptr;
		}
		const auto hand = isLeft ? api::Hand::Left : api::Hand::Right;
		api::grab::HandInteractionStateV1 state{};
		const auto status = s_grab->getHandInteractionStateV1(s_rock.owner(), hand, &state);
		constexpr auto valid = static_cast<std::uint32_t>(
		api::grab::HandInteractionFlagV1::Valid);
		if (status != api::Status::Ok || (state.flags & valid) == 0 || state.phase != api::grab::HandInteractionPhaseV1::Holding || state.targetFormId == 0) {
			return nullptr;
		}
		TESForm* form = LookupFormByID(state.targetFormId);
		return form ? dynamic_cast<TESObjectREFR*>(form) : nullptr;
	}

	EquippedWeapon GetEquippedWeaponHand() {
		api::weapon::EquippedWeaponHandlingStateV1 state{};
		if (!ReadWeaponHandling(state)) {
			return EquippedWeapon::None;
		}
		constexpr auto weaponPresent = static_cast<std::uint32_t>(api::weapon::EquippedWeaponHandlingRuntimeFlagV1::WeaponPresent);
		if ((state.runtimeFlags & weaponPresent) == 0) {
			return EquippedWeapon::None;
		}
		if (state.currentFiringHand == api::Hand::Right) {
			return EquippedWeapon::equippedRight;
		}
		if (state.currentFiringHand == api::Hand::Left) {
			return EquippedWeapon::equippedLeft;
		}
		return EquippedWeapon::None;
	}

	HeldWeapon GetHeldWeaponHand() {
		TESObjectREFR* leftHand = RockGetHeld(true);
		TESObjectREFR* rightHand = RockGetHeld(false);
		TESObjectWEAP* leftWeap = nullptr;
		TESObjectWEAP* rightWeap = nullptr;
		if (leftHand) {
			leftWeap = dynamic_cast<TESObjectWEAP*>(leftHand->baseForm);
		}
		if (rightHand) {
			rightWeap = dynamic_cast<TESObjectWEAP*>(rightHand->baseForm);
		}
		if (leftWeap && rightWeap) return HeldWeapon::inBoth;
		if (leftWeap && !rightWeap) return HeldWeapon::inLeft;
		if (!leftWeap && rightWeap) return HeldWeapon::inRight;
		else return HeldWeapon::None;
	}

	bool isTwoHandingWeapon() {
		if (!RockConnected()) {
			return false;
		}
		api::weapon::EquippedWeaponGripStateV1 state{};
		const auto status = s_weapon->getEquippedWeaponGripStateV1(s_rock.owner(), &state);
		if (status != api::Status::Ok) {
			return false;
		}
		using Flag = api::weapon::EquippedWeaponGripStateFlagV1;
		constexpr auto required = static_cast<std::uint32_t>(Flag::Valid) | static_cast<std::uint32_t>(Flag::TwoHandGripActive);
		return (state.flags & required) == required;
	}

	bool isPrimaryHandDetached() {
		api::weapon::EquippedWeaponHandlingStateV1 state{};
		if (!ReadWeaponHandling(state)) {
			return false;
		}
		using Flag = api::weapon::EquippedWeaponHandlingRuntimeFlagV1;
		const bool hasWeapon =(state.runtimeFlags & static_cast<std::uint32_t>(Flag::WeaponPresent)) != 0;
		const bool gripOccupied = (state.runtimeFlags & static_cast<std::uint32_t>(Flag::FiringGripOccupied)) != 0;
		return hasWeapon && !gripOccupied;
	}
}