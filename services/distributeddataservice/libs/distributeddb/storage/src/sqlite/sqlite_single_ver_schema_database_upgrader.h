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

#ifndef SQLITE_SINGLE_VER_SCHEMA_DATABASE_UPGRADER_H
#define SQLITE_SINGLE_VER_SCHEMA_DATABASE_UPGRADER_H

#include "sqlite_single_ver_database_upgrader.h"
#include "single_ver_schema_database_upgrader.h"

namespace DistributedDB {
class SQLiteSingleVerSchemaDatabaseUpgrader final : public SQLiteSingleVerDatabaseUpgrader,
    public SingleVerSchemaDatabaseUpgrader {
public:
    // An invalid SchemaObject indicate no schema
    SQLiteSingleVerSchemaDatabaseUpgrader(sqlite3 *db, const SchemaObject &newSchema,
        const SecurityOption &securityOpt, bool isMemDB);
    ~SQLiteSingleVerSchemaDatabaseUpgrader() override {};
protected:
    // Get an empty string with return_code E_OK indicate no schema but everything normally
    int GetDatabaseSchema(std::string &dbSchema) const override;

    // Set or update schema into database file
    int SetDatabaseSchema(const std::string &dbSchema) override;

    int UpgradeValues() override;
    int UpgradeIndexes(const IndexDifference &indexDiffer) override;
};
} // namespace DistributedDB
#endif // SQLITE_SINGLE_VER_SCHEMA_DATABASE_UPGRADER_H