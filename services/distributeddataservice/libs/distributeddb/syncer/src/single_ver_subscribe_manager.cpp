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
#include "single_ver_subscribe_manager.h"
#include "db_common.h"
#include "sync_types.h"

namespace DistributedDB {
void SingleVerSubscribeManager::ClearRemoteSubscribeQuery(const std::string &device)
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    ClearSubscribeQuery(device, remoteSubscribedMap_, remoteSubscribedTotalMap_);
}

void SingleVerSubscribeManager::ClearAllRemoteQuery()
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    remoteSubscribedMap_.clear();
    remoteSubscribedTotalMap_.clear();
}

void SingleVerSubscribeManager::ClearLocalSubscribeQuery(const std::string &device)
{
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    unFinishedLocalAutoSubMap_.erase(device);
    ClearSubscribeQuery(device, localSubscribeMap_, localSubscribeTotalMap_);
}

int SingleVerSubscribeManager::ReserveRemoteSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    int errCode = ReserveSubscribeQuery(device, query, remoteSubscribedMap_, remoteSubscribedTotalMap_);
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s remote reserve err=%d",
        STR_MASK(device), STR_MASK(query.GetIdentify()), errCode);
    return errCode;
}

int SingleVerSubscribeManager::ActiveRemoteSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    std::string queryId = query.GetIdentify();
    int errCode = ActiveSubscribeQuery(device, queryId, remoteSubscribedMap_, remoteSubscribedTotalMap_);
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s remote active err=%d",
        STR_MASK(device), STR_MASK(queryId), errCode);
    return errCode;
}

int SingleVerSubscribeManager::ReserveLocalSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    int errCode = ReserveSubscribeQuery(device, query, localSubscribeMap_, localSubscribeTotalMap_);
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s local reserve err=%d",
        STR_MASK(device), STR_MASK(query.GetIdentify()), errCode);
    return errCode;
}

int SingleVerSubscribeManager::ActiveLocalSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    std::string queryId = query.GetIdentify();
    int errCode = ActiveSubscribeQuery(device, queryId, localSubscribeMap_, localSubscribeTotalMap_);
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s local active err=%d",
        STR_MASK(device), STR_MASK(queryId), errCode);
    if (errCode != E_OK) {
        return errCode;
    }
    if (unFinishedLocalAutoSubMap_.find(device) != unFinishedLocalAutoSubMap_.end() &&
        unFinishedLocalAutoSubMap_[device].find(queryId) != unFinishedLocalAutoSubMap_[device].end()) {
            unFinishedLocalAutoSubMap_[device].erase(queryId);
    }
    return errCode;
}

void SingleVerSubscribeManager::DeleteLocalSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    std::string queryId = query.GetIdentify();
    DeleteSubscribeQuery(device, queryId, localSubscribeMap_, localSubscribeTotalMap_);
}

void SingleVerSubscribeManager::DeleteRemoteSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    std::string queryId = query.GetIdentify();
    DeleteSubscribeQuery(device, queryId, remoteSubscribedMap_, remoteSubscribedTotalMap_);
}

void SingleVerSubscribeManager::PutLocalUnFiniedSubQueries(const std::string &device,
    std::vector<QuerySyncObject> &subscribeQueries)
{
    LOGI("[SingleVerSubscribeManager] put local unfinished subscribe queries, nums=%d", subscribeQueries.size());
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    if (subscribeQueries.size() == 0) {
        unFinishedLocalAutoSubMap_.erase(device);
        return;
    }
    unFinishedLocalAutoSubMap_[device].clear();
    auto iter = unFinishedLocalAutoSubMap_.find(device);
    for (const auto &query : subscribeQueries) {
        iter->second.insert(query.GetIdentify());
    }
}

void SingleVerSubscribeManager::GetAllUnFinishSubQueries(
    std::map<std::string, std::vector<QuerySyncObject>> &allSyncQueries) const
{
    std::shared_lock<std::shared_mutex> lock(localSubscribeMapLock_);
    for (auto &item : unFinishedLocalAutoSubMap_) {
        if (item.second.size() == 0) {
            continue;
        }
        allSyncQueries[item.first] = {};
        auto iter = allSyncQueries.find(item.first);
        for (const auto &queryId : item.second) {
            auto iterTmp = localSubscribeTotalMap_.find(queryId);
            if (iterTmp == localSubscribeTotalMap_.end()) {
                LOGI("[SingleVerSubscribeManager] queryId=%s not in localTotalMap", STR_MASK(queryId));
                continue;
            }
            iter->second.push_back(iterTmp->second.first);
        }
    }
}

void SingleVerSubscribeManager::RemoveRemoteSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    std::string queryId = query.GetIdentify();
    RemoveSubscribeQuery(device, queryId, remoteSubscribedMap_, remoteSubscribedTotalMap_);
}

void SingleVerSubscribeManager::RemoveLocalSubscribeQuery(const std::string &device, const QuerySyncObject &query)
{
    std::unique_lock<std::shared_mutex> lockGuard(localSubscribeMapLock_);
    std::string queryId = query.GetIdentify();
    RemoveSubscribeQuery(device, queryId, localSubscribeMap_, localSubscribeTotalMap_);
    if (unFinishedLocalAutoSubMap_.find(device) != unFinishedLocalAutoSubMap_.end() &&
        unFinishedLocalAutoSubMap_[device].find(queryId) != unFinishedLocalAutoSubMap_[device].end()) {
            unFinishedLocalAutoSubMap_[device].erase(queryId);
            LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s delete from UnFinishedMap",
                STR_MASK(device), STR_MASK(queryId));
            if (unFinishedLocalAutoSubMap_[device].size() == 0) {
                LOGI("[SingleVerSubscribeManager] dev=%s delete from unFinish map", STR_MASK(device));
                unFinishedLocalAutoSubMap_.erase(device);
            }
    }
}

void SingleVerSubscribeManager::GetLocalSubscribeQueries(const std::string &device,
    std::vector<QuerySyncObject> &subscribeQueries) const
{
    std::shared_lock<std::shared_mutex> lock(localSubscribeMapLock_);
    GetSubscribeQueries(device, localSubscribeMap_, localSubscribeTotalMap_, subscribeQueries);
}

void SingleVerSubscribeManager::GetRemoteSubscribeQueries(const std::string &device,
    std::vector<QuerySyncObject> &subscribeQueries) const
{
    std::shared_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    GetSubscribeQueries(device, remoteSubscribedMap_, remoteSubscribedTotalMap_, subscribeQueries);
}

bool SingleVerSubscribeManager::IsRemoteContainSubscribe(const std::string &device, const QuerySyncObject &query) const
{
    std::shared_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    auto iter = remoteSubscribedMap_.find(device);
    if (iter == remoteSubscribedMap_.end()) {
        LOGD("[SingleVerSubscribeManager] dev=%s not in remoteSubscribedMap", STR_MASK(device));
        return false;
    }
    std::string queryId = query.GetIdentify();
    auto subIter = iter->second.find(queryId);
    if (subIter == iter->second.end()) {
        LOGE("[SingleVerSubscribeManager] queryId=%s not in RemoteTotalMap", STR_MASK(queryId));
        return false;
    }
    return true;
}

void SingleVerSubscribeManager::GetRemoteSubscribeQueryIds(const std::string &device,
    std::vector<std::string> &subscribeQueryIds) const
{
    std::shared_lock<std::shared_mutex> lockGuard(remoteSubscribedMapLock_);
    auto iter = remoteSubscribedMap_.find(device);
    if (iter == remoteSubscribedMap_.end()) {
        LOGI("[SingleVerSubscribeManager] dev=%s not in remoteSubscribedMap", STR_MASK(device));
        return;
    }
    for (const auto &queryInfo : iter->second) {
        if (remoteSubscribedTotalMap_.find(queryInfo.first) == remoteSubscribedTotalMap_.end()) {
            LOGE("[SingleVerSubscribeManager] queryId=%s not in RemoteTotalMap", STR_MASK(queryInfo.first));
            continue;
        }
        subscribeQueryIds.push_back(queryInfo.first);
    }
}

int SingleVerSubscribeManager::LocalSubscribeLimitCheck(const std::vector<std::string> &devices,
    QuerySyncObject &query) const
{
    std::shared_lock<std::shared_mutex> lock(localSubscribeMapLock_);
    int devNum = localSubscribeMap_.size();
    for (const auto &device : devices) {
        if (localSubscribeMap_.find(device) != localSubscribeMap_.end()) {
            continue;
        }
        devNum++;
        if (devNum > MAX_DEVICES_NUM) {
            LOGE("[SingleVerSubscribeManager] local subscribe devices is over limit");
            return -E_MAX_LIMITS;
        }
    }
    std::string queryId = query.GetIdentify();
    auto allIter = localSubscribeTotalMap_.find(queryId);
    if (allIter == localSubscribeTotalMap_.end() && localSubscribeTotalMap_.size() >= MAX_SUBSCRIBE_NUM_PER_DB) {
        LOGE("[SingleVerSubscribeManager] all local subscribe sums is over limit");
        return -E_MAX_LIMITS;
    }
    return E_OK;
}

void SingleVerSubscribeManager::ClearSubscribeQuery(const std::string &device, SubscribeMap &subscribeMap,
    SubscribedTotalMap &subscribedTotalMap)
{
    if (subscribeMap.find(device) == subscribeMap.end()) {
        LOGI("[SingleVerSubscribeManager] dev=%s not in SubscribedMap", STR_MASK(device));
        return;
    }
    for (const auto &queryInfo : subscribeMap[device]) {
        if (subscribedTotalMap.find(queryInfo.first) != subscribedTotalMap.end()) {
            if (subscribedTotalMap[queryInfo.first].second > 0) {
                subscribedTotalMap[queryInfo.first].second--;
            }
            if (subscribedTotalMap[queryInfo.first].second == 0) {
                LOGI("[SingleVerSubscribeManager] queryId=%s delete from TotalMap", STR_MASK(queryInfo.first));
                subscribedTotalMap.erase(queryInfo.first);
            }
        }
    }
    subscribeMap.erase(device);
    LOGI("[SingleVerSubscribeManager] clear dev=%s` remote subscribe queies finished", STR_MASK(device));
}

int SingleVerSubscribeManager::ReserveSubscribeQuery(const std::string &device, const QuerySyncObject &query,
    SubscribeMap &subscribeMap, SubscribedTotalMap &subscribedTotalMap)
{
    std::string queryId = query.GetIdentify();
    auto iter = subscribeMap.find(device);
    auto allIter = subscribedTotalMap.find(queryId);
    // limit check
    if (allIter == subscribedTotalMap.end() && subscribedTotalMap.size() >= MAX_SUBSCRIBE_NUM_PER_DB) {
        LOGE("[SingleVerSubscribeManager] all subscribe sums is over limit");
        return -E_MAX_LIMITS;
    }
    if (iter == subscribeMap.end() && subscribeMap.size() >= MAX_DEVICES_NUM) {
        LOGE("[SingleVerSubscribeManager] subscribe devices is over limit");
        return -E_MAX_LIMITS;
    }
    if (iter != subscribeMap.end() && iter->second.find(queryId) == iter->second.end() &&
        iter->second.size() >= MAX_SUBSCRIBE_NUM_PER_DEV) {
        LOGE("[SingleVerSubscribeManager] subscribe sums is over limit");
        return -E_MAX_LIMITS;
    }
    if (iter != subscribeMap.end() && iter->second.find(queryId) != iter->second.end() &&
        iter->second[queryId] == SubscribeStatus::ACTIVE) {
        LOGE("[SingleVerSubscribeManager] dev=%s,queryId=%s already active in map",
            STR_MASK(device), STR_MASK(queryId));
        return E_OK;
    }

    if (iter == subscribeMap.end()) {
        subscribeMap[device] = std::map<std::string, SubscribeStatus> {};
    }
    bool isNeedInc = false;
    if (subscribeMap[device].find(queryId) == subscribeMap[device].end()) {
        subscribeMap[device][queryId] = SubscribeStatus::NOT_ACTIVE;
        isNeedInc = true;
    }
    if (allIter == subscribedTotalMap.end()) {
        subscribedTotalMap[queryId] = {query, 1};
    } else if (isNeedInc) {
        subscribedTotalMap[queryId].second++;
    }
    return E_OK;
}

int SingleVerSubscribeManager::ActiveSubscribeQuery(const std::string &device, const std::string &queryId,
    SubscribeMap &subscribeMap, SubscribedTotalMap &subscribedTotalMap)
{
    if (subscribedTotalMap.find(queryId) == subscribedTotalMap.end()) {
        LOGE("[SingleVerSubscribeManager] can not find queryId=%s in SubscribeTotalMap", STR_MASK(queryId));
        return -E_INTERNAL_ERROR;
    }
    if (subscribeMap.find(device) == subscribeMap.end()) {
        LOGE("[SingleVerSubscribeManager] can not find dev=%s in localSubscribeMap", STR_MASK(device));
        return -E_INTERNAL_ERROR;
    }
    if (subscribeMap[device].find(queryId) == subscribeMap[device].end()) {
        LOGE("[SingleVerSubscribeManager] can not find dev=%s,queryId=%s in map", STR_MASK(device), STR_MASK(queryId));
        return -E_INTERNAL_ERROR;
    }
    subscribeMap[device][queryId] = SubscribeStatus::ACTIVE;
    return E_OK;
}

void SingleVerSubscribeManager::DeleteSubscribeQuery(const std::string &device, const std::string &queryId,
    SubscribeMap &subscribeMap, SubscribedTotalMap &subscribedTotalMap)
{
    if (subscribeMap.find(device) == subscribeMap.end()) {
        LOGE("[SingleVerSubscribeManager] can not find dev=%s in map", STR_MASK(device));
        return;
    }
    if (subscribeMap[device].find(queryId) == subscribeMap[device].end()) {
        LOGE("[SingleVerSubscribeManager] can not find dev=%s,queryId=%s in map", STR_MASK(device), STR_MASK(queryId));
        return;
    }
    SubscribeStatus queryStatus = subscribeMap[device][queryId];
    // not permit to delete the query when something wrong this time,because it is subscribed successfully last time
    if (queryStatus == SubscribeStatus::ACTIVE) {
        LOGE("[SingleVerSubscribeManager] dev=%s,queryId=%s is active, no need to del",
            STR_MASK(device), STR_MASK(queryId));
        return;
    }
    subscribeMap[device].erase(queryId);
    auto iter = subscribedTotalMap.find(queryId);
    if (iter == subscribedTotalMap.end()) {
        LOGE("[SingleVerSubscribeManager] can not find queryId=%s in SubscribeTotalMap", STR_MASK(queryId));
        return;
    }
    iter->second.second--;
    if (iter->second.second <= 0) {
        LOGI("[SingleVerSubscribeManager] del queryId=%s from SubscribeTotalMap", STR_MASK(queryId));
        subscribedTotalMap.erase(queryId);
    }
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s remove from SubscribeMap success",
        STR_MASK(device), STR_MASK(queryId));
}

void SingleVerSubscribeManager::RemoveSubscribeQuery(const std::string &device, const std::string &queryId,
    SubscribeMap &subscribeMap, SubscribedTotalMap &subscribedTotalMap)
{
    auto iter = subscribeMap.find(device);
    if (iter == subscribeMap.end()) {
        LOGE("[SingleVerSubscribeManager] dev=%s not in SubscribedMap", STR_MASK(device));
        return;
    }
    if (iter->second.find(queryId) == subscribeMap[device].end()) {
        LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s not in SubscribedMap", STR_MASK(device), STR_MASK(queryId));
        return;
    }
    iter->second.erase(queryId);
    auto allIter = subscribedTotalMap.find(queryId);
    if (allIter == subscribedTotalMap.end()) {
        LOGI("[SingleVerSubscribeManager] queryId=%s not in TotalMap", STR_MASK(queryId));
        return;
    }
    allIter->second.second--;
    if (allIter->second.second <= 0) {
        subscribedTotalMap.erase(queryId);
        LOGI("[SingleVerSubscribeManager] queryId=%s delete from TotalMap", STR_MASK(queryId));
    }
    LOGI("[SingleVerSubscribeManager] dev=%s,queryId=%s remove from SubscribedMap success",
        STR_MASK(device), STR_MASK(queryId));
}

void SingleVerSubscribeManager::GetSubscribeQueries(const std::string &device, const SubscribeMap &subscribeMap,
    const SubscribedTotalMap &subscribedTotalMap, std::vector<QuerySyncObject> &subscribeQueries) const
{
    auto iter = subscribeMap.find(device);
    if (iter == subscribeMap.end()) {
        LOGD("[SingleVerSubscribeManager] dev=%s not in localSubscribeMap", STR_MASK(device));
        return;
    }
    for (const auto &queryInfo : iter->second) {
        auto iterTmp = subscribedTotalMap.find(queryInfo.first);
        if (iterTmp == subscribedTotalMap.end()) {
            LOGE("[SingleVerSubscribeManager] queryId=%s not in localTotalMap", STR_MASK(queryInfo.first));
            continue;
        }
        subscribeQueries.push_back(iterTmp->second.first);
    }
}
} // namespace DistributedDB