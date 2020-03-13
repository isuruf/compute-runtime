/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "sysman/memory/os_memory.h"

namespace L0 {

class LinuxMemoryImp : public OsMemory {
  public:
    ze_result_t getAllocSize(uint64_t &allocSize) override;
    ze_result_t getMaxSize(uint64_t &maxSize) override;
    ze_result_t getMemHealth(zet_mem_health_t &memHealth) override;
};

ze_result_t LinuxMemoryImp::getAllocSize(uint64_t &allocSize) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t LinuxMemoryImp::getMaxSize(uint64_t &maxSize) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
ze_result_t LinuxMemoryImp::getMemHealth(zet_mem_health_t &memHealth) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

OsMemory *OsMemory::create(OsSysman *pOsSysman) {
    LinuxMemoryImp *pLinuxMemoryImp = new LinuxMemoryImp();
    return static_cast<OsMemory *>(pLinuxMemoryImp);
}

} // namespace L0
