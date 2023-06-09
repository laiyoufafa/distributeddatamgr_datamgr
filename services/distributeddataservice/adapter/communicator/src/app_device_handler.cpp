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

#include "app_device_handler.h"
#include "log_print.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AppDeviceHandler"

namespace OHOS {
namespace AppDistributedKv {
AppDeviceHandler::AppDeviceHandler()
{
    softbusAdapter_ = SoftBusAdapter::GetInstance();
}

AppDeviceHandler::~AppDeviceHandler()
{
    ZLOGI("destruct");
}
void AppDeviceHandler::Init()
{
    softbusAdapter_->Init();
}

Status AppDeviceHandler::StartWatchDeviceChange(const AppDeviceChangeListener *observer,
    __attribute__((unused)) const PipeInfo &pipeInfo)
{
    return softbusAdapter_->StartWatchDeviceChange(observer, pipeInfo);
}

Status AppDeviceHandler::StopWatchDeviceChange(const AppDeviceChangeListener *observer,
    __attribute__((unused)) const PipeInfo &pipeInfo)
{
    return softbusAdapter_->StopWatchDeviceChange(observer, pipeInfo);
}

std::vector<DeviceInfo> AppDeviceHandler::GetRemoteDevices() const
{
    return softbusAdapter_->GetRemoteDevices();
}

DeviceInfo AppDeviceHandler::GetDeviceInfo(const std::string &networkId) const
{
    return softbusAdapter_->GetDeviceInfo(networkId);
}

DeviceInfo AppDeviceHandler::GetLocalDevice()
{
    return softbusAdapter_->GetLocalDevice();
}

std::string AppDeviceHandler::GetUuidByNodeId(const std::string &nodeId) const
{
    return softbusAdapter_->GetUuidByNodeId(nodeId);
}

DeviceInfo AppDeviceHandler::GetLocalBasicInfo() const
{
    return softbusAdapter_->GetLocalBasicInfo();
}

std::string AppDeviceHandler::GetUdidByNodeId(const std::string &nodeId) const
{
    return softbusAdapter_->GetUdidByNodeId(nodeId);
}

void AppDeviceHandler::UpdateRelationship(const DeviceInfo &deviceInfo, const DeviceChangeType &type)
{
    return softbusAdapter_->UpdateRelationship(deviceInfo, type);
}

std::string AppDeviceHandler::ToUUID(const std::string& id) const
{
    return softbusAdapter_->ToUUID(id);
}

std::string AppDeviceHandler::ToNodeID(const std::string &nodeId, const std::string &defaultId) const
{
    return softbusAdapter_->ToNodeID(nodeId, defaultId);
}

std::string AppDeviceHandler::ToBeAnonymous(const std::string &name)
{
    return SoftBusAdapter::ToBeAnonymous(name);
}
}  // namespace AppDistributedKv
}  // namespace OHOS
