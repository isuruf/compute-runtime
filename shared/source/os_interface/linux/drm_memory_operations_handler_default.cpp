/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/os_interface/linux/drm_memory_operations_handler_default.h"

#include "shared/source/debug_settings/debug_settings_manager.h"

#include <algorithm>

namespace NEO {

DrmMemoryOperationsHandlerDefault::DrmMemoryOperationsHandlerDefault() = default;
DrmMemoryOperationsHandlerDefault::~DrmMemoryOperationsHandlerDefault() = default;

MemoryOperationsStatus DrmMemoryOperationsHandlerDefault::makeResident(Device *device, ArrayRef<GraphicsAllocation *> gfxAllocations) {
    std::lock_guard<std::mutex> lock(mutex);
    this->residency.insert(gfxAllocations.begin(), gfxAllocations.end());
    return MemoryOperationsStatus::SUCCESS;
}

MemoryOperationsStatus DrmMemoryOperationsHandlerDefault::evictWithinOsContext(OsContext *osContext, GraphicsAllocation &gfxAllocation) {
    std::lock_guard<std::mutex> lock(mutex);
    this->residency.erase(&gfxAllocation);
    return MemoryOperationsStatus::SUCCESS;
}

MemoryOperationsStatus DrmMemoryOperationsHandlerDefault::evict(Device *device, GraphicsAllocation &gfxAllocation) {
    OsContext *osContext = nullptr;
    return this->evictWithinOsContext(osContext, gfxAllocation);
}

MemoryOperationsStatus DrmMemoryOperationsHandlerDefault::isResident(Device *device, GraphicsAllocation &gfxAllocation) {
    std::lock_guard<std::mutex> lock(mutex);
    auto ret = this->residency.find(&gfxAllocation);
    if (ret == this->residency.end()) {
        return MemoryOperationsStatus::MEMORY_NOT_FOUND;
    }
    return MemoryOperationsStatus::SUCCESS;
}

void DrmMemoryOperationsHandlerDefault::mergeWithResidencyContainer(OsContext *osContext, ResidencyContainer &residencyContainer) {
    for (auto gfxAllocation = this->residency.begin(); gfxAllocation != this->residency.end(); gfxAllocation++) {
        auto ret = std::find(residencyContainer.begin(), residencyContainer.end(), *gfxAllocation);
        if (ret == residencyContainer.end()) {
            residencyContainer.push_back(*gfxAllocation);
        }
    }
}

std::unique_lock<std::mutex> DrmMemoryOperationsHandlerDefault::lockHandlerForExecWA() {
    if (DebugManager.flags.MakeAllBuffersResident.get()) {
        return std::unique_lock<std::mutex>(this->mutex);
    }
    return std::unique_lock<std::mutex>();
}

} // namespace NEO