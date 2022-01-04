/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#define LOG_TAG "EntryPoint"
#include "js_const_properties.h"
#include "js_util.h"
#include "kv_manager.h"
#include "log_print.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

using namespace OHOS::DistributedData;
using namespace OHOS::DistributedKv;

static napi_value Init(napi_env env, napi_value exports)
{
    const napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createKVManager", KVManager::CreateKVManager)
    };
    napi_status status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    ZLOGI("init createKVManager %{public}d", status);

    status = InitConstProperties(env, exports);
    ZLOGI("init Enumerate Constants %{public}d", status);
    return exports;
}

static __attribute__((constructor)) void RegisterModule()
{
    static napi_module module = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "data.distributedData",
        .nm_priv = ((void *)0),
        .reserved = { 0 }
    };
    napi_module_register(&module);
    ZLOGI("module register data.distributedData");
}

