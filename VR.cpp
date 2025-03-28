#include "VR.h"
#include "matrix.h"


namespace VRHook {

	RelocPtr<uint64_t*> vrDataStruct(0x59429c0);

	void HmdMatrixToNiTransform(NiTransform* a_transform, vr::TrackedDevicePose_t* a_pose) {
		using func_t = decltype(&HmdMatrixToNiTransform);
		RelocAddr<func_t> func(0x1bab210);
		return func(a_transform, a_pose);
	}


	void VRSystem::getTrackerNiTransformByIndex(vr::TrackedDeviceIndex_t idx, NiTransform* transform) {
		vr::TrackedDevicePose_t pose = renderPoses[idx];
		HmdMatrixToNiTransform(transform, &pose);

		if (vrDataStruct != nullptr && roomNode != nullptr) {
			NiMatrix43* worldSpaceMat = (NiMatrix43*)((char*)(*vrDataStruct) + 0x210);
			NiPoint3* worldSpaceVec = (NiPoint3*)((char*)(*vrDataStruct) + 0x158);
			transform->pos = transform->pos - *worldSpaceVec;
			transform->pos = *worldSpaceMat * transform->pos;
			//		transform->pos = roomNode->m_worldTransform.rot * transform->pos * roomNode->m_worldTransform.scale;
		}
	}

	void VRSystem::getControllerNiTransformByName(std::string trackerName, NiTransform* transform) {
		vr::TrackedDevicePose_t pose = renderPoses[controllers[trackerName]];
		HmdMatrixToNiTransform(transform, &pose);

		if (vrDataStruct != nullptr && roomNode != nullptr) {
			NiMatrix43* worldSpaceMat = (NiMatrix43*)((char*)(*vrDataStruct) + 0x210);
			NiPoint3* worldSpaceVec = (NiPoint3*)((char*)(*vrDataStruct) + 0x158);
			transform->pos = transform->pos - *worldSpaceVec;
			transform->pos = *worldSpaceMat * transform->pos;
		}
	}

	void VRSystem::debugPrint() {
		vr::VRCompositorError error = vrHook->GetVRCompositor()->GetLastPoses((vr::TrackedDevicePose_t*)renderPoses, vr::k_unMaxTrackedDeviceCount, (vr::TrackedDevicePose_t*)gamePoses, vr::k_unMaxTrackedDeviceCount);
		if (error && error != vr::EVRCompositorError::VRCompositorError_None)
			_MESSAGE("Error while retriving game poses!");
	}

	VRSystem* g_vrHook = nullptr;

}