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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>

#include "rc.h"
#include "sql/stmt/stmt.h"


class Field;
class AggrField;
class FieldMeta;
class FilterStmt;
class SortField;
class JoinStmt;
class Db;
class Table;

class SelectStmt : public Stmt
{
public:

  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }
public:
  static RC create(Db *db, const Selects &select_sql, Stmt *&stmt);

public:
  const std::vector<Table *> &tables() const { return tables_; }
  const std::vector<Field> &query_fields() const { return query_fields_; }
  const std::vector<AggrField> &aggr_fields() const { return aggr_fields_; }
  const std::vector<SortField> &sort_fields() const { return sort_fields_; }
  const std::vector<Field> &group_fields() const { return group_fields_; }
  FilterStmt *filter_stmt() const { return filter_stmt_; }
  JoinStmt *join_stmt() const { return join_stmt_; }

  bool is_has_query_fields() {
    return !query_fields_.empty();
  }

  bool is_has_mutil_table() {
    return tables_.size() > 1;
  }

  bool is_inner_join();

  bool is_has_aggregation() {
    return !aggr_fields_.empty();
  } 

  bool is_has_order_by() {
    return !sort_fields_.empty();
  }

  bool is_has_group_by() {
    return !group_fields_.empty();
  }

private:
  std::vector<Field> query_fields_;
   std::vector<AggrField> aggr_fields_;
  std::vector<Table *> tables_;
  FilterStmt *filter_stmt_ = nullptr;
  JoinStmt *join_stmt_ = nullptr;

  std::vector<SortField> sort_fields_;
  std::vector<Field> group_fields_;
};

