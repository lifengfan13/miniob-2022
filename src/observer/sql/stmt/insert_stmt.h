/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include "rc.h"
#include "sql/stmt/stmt.h"

class Table;
class Db;

class InsertStmt : public Stmt
{
public:

  InsertStmt() = default;
  InsertStmt(Table *table, const InsertRecord *records_, int record_amount);

  StmtType type() const override {
    return StmtType::INSERT;
  }
public:
  static RC create(Db *db, Inserts &insert_sql, Stmt *&stmt);

public:
  Table *table() const {return table_;}
  const InsertRecord *records() const { return records_; }
  int record_amount() const { return record_amount_; }

private:
  Table *table_ = nullptr;
  const InsertRecord *records_ = nullptr;
  int record_amount_ = 0;
};

