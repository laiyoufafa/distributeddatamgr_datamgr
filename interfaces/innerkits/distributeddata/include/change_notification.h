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

#ifndef CHANGE_NOTIFICATION_H
#define CHANGE_NOTIFICATION_H

#include <list>
#include "types.h"
#include "parcel.h"

namespace OHOS {
namespace DistributedKv {
class ChangeNotification final : public virtual Parcelable {
public:
    // Constructor of ChangeNotification.
    ChangeNotification(std::vector<Entry> &&insertEntries, std::vector<Entry> &&updateEntries,
                       std::vector<Entry> &&deleteEntries, const std::string &deviceId, bool isClear);

    API_EXPORT ~ChangeNotification();

    // Get all inserted entries in this change.
    API_EXPORT const std::vector<Entry> &GetInsertEntries() const;

    // Get all updated entries in this changing.
    API_EXPORT const std::vector<Entry> &GetUpdateEntries() const;

    // Get all deleted entries in this changing.
    API_EXPORT const std::vector<Entry> &GetDeleteEntries() const;

    // Get the device ID.
    API_EXPORT const std::string &GetDeviceId() const;

    // Check if this change is made by calling the Clear function.
    API_EXPORT bool IsClear() const;

    // Write a parcelable object to the given parcel.
    // The object position is saved into Parcel if asRemote_ is set to
    // true, and this intends to use in kernel data transaction.
    // Returns true if the writing is successful; returns false otherwise.
    API_EXPORT bool Marshalling(Parcel &parcel) const override;

    // Unmarshall the given parcel from this parcelable object.
    API_EXPORT static ChangeNotification *Unmarshalling(Parcel &parcel);

private:
    std::vector<Entry> insertEntries_;

    std::vector<Entry> updateEntries_;

    std::vector<Entry> deleteEntries_;

    std::string deviceId_;

    bool isClear_ = false;
};
}  // namespace DistributedKv
}  // namespace OHOS
#endif  // CHANGE_NOTIFICATION_H
