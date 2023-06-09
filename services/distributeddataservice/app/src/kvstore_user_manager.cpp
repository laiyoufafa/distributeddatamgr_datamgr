/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "KvStoreUserManager"

#include "kvstore_user_manager.h"
#include "account_delegate.h"
#include "checker/checker_manager.h"
#include "constant.h"
#include "kvstore_utils.h"
#include "log_print.h"
#include "permission_validator.h"

namespace OHOS {
namespace DistributedKv {
using namespace DistributedData;
KvStoreUserManager::KvStoreUserManager(const std::string &userId)
    : appMutex_(),
      appMap_(),
      userId_(userId)
{
    ZLOGI("begin.");
}

KvStoreUserManager::~KvStoreUserManager()
{
    ZLOGI("begin.");
    appMap_.clear();
}

Status KvStoreUserManager::CloseKvStore(const std::string &appId, const std::string &storeId)
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    auto it = appMap_.find(appId);
    if (it != appMap_.end()) {
        return (it->second).CloseKvStore(storeId);
    }

    ZLOGE("store not open.");
    return Status::STORE_NOT_OPEN;
}

Status KvStoreUserManager::CloseAllKvStore(const std::string &appId)
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    auto it = appMap_.find(appId);
    if (it != appMap_.end()) {
        return (it->second).CloseAllKvStore();
    }

    ZLOGE("store not open.");
    return Status::STORE_NOT_OPEN;
}

void KvStoreUserManager::CloseAllKvStore()
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    for (auto &it : appMap_) {
        (it.second).CloseAllKvStore();
    }
}

Status KvStoreUserManager::DeleteKvStore(const std::string &bundleName, pid_t uid, const std::string &storeId)
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    auto it = appMap_.find(bundleName);
    if (it != appMap_.end()) {
        auto status = (it->second).DeleteKvStore(storeId);
        if ((it->second).GetTotalKvStoreNum() == 0) {
            ZLOGI("There is not kvstore, so remove the  app manager.");
            appMap_.erase(it);
        }
        return status;
    }
    KvStoreAppManager kvStoreAppManager(bundleName, uid);
    return kvStoreAppManager.DeleteKvStore(storeId);
}

void KvStoreUserManager::DeleteAllKvStore()
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    for (auto &it : appMap_) {
        (it.second).DeleteAllKvStore();
    }
    appMap_.clear();
}

// Migrate all KvStore DB delegate object when harmony account changed.
Status KvStoreUserManager::MigrateAllKvStore(const std::string &harmonyAccountId)
{
    ZLOGI("begin.");
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    Status status = Status::SUCCESS;
    for (auto &it : appMap_) {
        status = (it.second).MigrateAllKvStore(harmonyAccountId);
        if (status != Status::SUCCESS) {
            ZLOGE("migrate all kvstore for app-%s failed, status:%d.",
                it.first.c_str(), static_cast<int>(status));
            status = Status::MIGRATION_KVSTORE_FAILED;
        }
    }
    return status;
}

std::string KvStoreUserManager::GetDbDir(const std::string &bundleName, const Options &options)
{
    ZLOGI("begin.");
    if (options.kvStoreType == KvStoreType::MULTI_VERSION) {
        return "default";
    }
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    auto it = appMap_.find(bundleName);
    if (it != appMap_.end()) {
        return (it->second).GetDbDir(options);
    }
    return "";
}

void KvStoreUserManager::Dump(int fd) const
{
    const std::string prefix(4, ' ');
    dprintf(fd, "%s--------------------------------------------------------------\n", prefix.c_str());
    dprintf(fd, "%sUserID        : %s\n", prefix.c_str(), userId_.c_str());
    dprintf(fd, "%sApp count     : %u\n", prefix.c_str(), static_cast<uint32_t>(appMap_.size()));
    for (const auto &pair : appMap_) {
        pair.second.Dump(fd);
    }
}

bool KvStoreUserManager::IsStoreOpened(const std::string &appId, const std::string &storeId)
{
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    auto it = appMap_.find(appId);
    return it != appMap_.end() && it->second.IsStoreOpened(storeId);
}

void KvStoreUserManager::SetCompatibleIdentify(const std::string &deviceId) const
{
    std::lock_guard<decltype(appMutex_)> lg(appMutex_);
    for (const auto &item : appMap_) {
        item.second.SetCompatibleIdentify(deviceId);
    }
}
}  // namespace DistributedKv
}  // namespace OHOS
