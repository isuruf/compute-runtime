/*
 * Copyright (C) 2017-2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "performance_counters_linux.h"

#include "shared/source/device/device.h"
#include "shared/source/device/sub_device.h"
#include "shared/source/helpers/hw_helper.h"
#include "shared/source/os_interface/linux/os_interface.h"
#include "shared/source/os_interface/linux/os_time_linux.h"

namespace NEO {
////////////////////////////////////////////////////
// PerformanceCounters::create
////////////////////////////////////////////////////
std::unique_ptr<PerformanceCounters> PerformanceCounters::create(Device *device) {
    auto counter = std::make_unique<PerformanceCountersLinux>();
    auto osInterface = device->getOSTime()->getOSInterface()->get();
    auto drm = osInterface->getDrm();
    auto gen = device->getHardwareInfo().platform.eRenderCoreFamily;
    auto &hwHelper = HwHelper::get(gen);
    UNRECOVERABLE_IF(counter == nullptr);

    if (device->getParentDevice() == nullptr) {

        // Root device.
        counter->subDevice.Enabled = false;
        counter->subDeviceIndex.Index = 0;
        counter->subDeviceCount.Count = device->getNumAvailableDevices();
    } else {

        // Sub device.
        counter->subDevice.Enabled = true;
        counter->subDeviceIndex.Index = static_cast<NEO::SubDevice *>(device)->getSubDeviceIndex();
        counter->subDeviceCount.Count = device->getParentDevice()->getNumAvailableDevices();
    }

    // Adapter data.
    counter->adapter.Type = LinuxAdapterType::DrmFileDescriptor;
    counter->adapter.DrmFileDescriptor = drm->getFileDescriptor();
    counter->clientData.Linux.Adapter = &(counter->adapter);

    // Gen data.
    counter->clientType.Gen = static_cast<MetricsLibraryApi::ClientGen>(hwHelper.getMetricsLibraryGenId());

    return counter;
}

//////////////////////////////////////////////////////
// PerformanceCountersLinux::enableCountersConfiguration
//////////////////////////////////////////////////////
bool PerformanceCountersLinux::enableCountersConfiguration() {
    // Release previous counters configuration so the user
    // can change configuration between kernels.
    releaseCountersConfiguration();

    // Create oa configuration.
    if (!metricsLibrary->oaConfigurationCreate(
            context,
            oaConfiguration)) {
        DEBUG_BREAK_IF(true);
        return false;
    }

    // Enable oa configuration.
    if (!metricsLibrary->oaConfigurationActivate(
            oaConfiguration)) {
        DEBUG_BREAK_IF(true);
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////
// PerformanceCountersLinux::releaseCountersConfiguration
//////////////////////////////////////////////////////
void PerformanceCountersLinux::releaseCountersConfiguration() {
    // Oa configuration.
    if (oaConfiguration.IsValid()) {
        metricsLibrary->oaConfigurationDeactivate(oaConfiguration);
        metricsLibrary->oaConfigurationDelete(oaConfiguration);
        oaConfiguration.data = nullptr;
    }
}
} // namespace NEO
