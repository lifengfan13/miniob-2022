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
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include <string.h>
#include "storage/common/field.h"
#include "sql/expr/tuple_cell.h"

class Tuple;

enum class ExprType {
  NONE,
  FIELD,
  VALUE,
  AGGREGATE
};

class Expression
{
public: 
  Expression() = default;
  virtual ~Expression() = default;
  
  virtual RC get_value(const Tuple &tuple, TupleCell &cell) const = 0;
  virtual ExprType type() const = 0;
};

class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field)
  {}

  virtual ~FieldExpr() = default;

  ExprType type() const override
  {
    return ExprType::FIELD;
  }

  Field &field()
  {
    return field_;
  }

  const Field &field() const
  {
    return field_;
  }

  const char *table_name() const
  {
    return field_.table_name();
  }

  const char *field_name() const
  {
    return field_.field_name();
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;
private:
  Field field_;
};

class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  ValueExpr(const Value &value) : tuple_cell_(value.type, (char *)value.data)
  {
    if (value.type == CHARS) {
      tuple_cell_.set_length(strlen((const char *)value.data));
    }else{
      tuple_cell_.set_length(4);
    }
  }

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, TupleCell & cell) const override;
  ExprType type() const override
  {
    return ExprType::VALUE;
  }

  void get_tuple_cell(TupleCell &cell) const {
    cell = tuple_cell_;
  }

private:
  TupleCell tuple_cell_;
};



class AggrExpr : public Expression
{
public:
  AggrExpr() = default;
  AggrExpr(const Table *table, const FieldMeta *field, const AggrType aggr_type) : aggr_field_(table, field, aggr_type)
  {}

  virtual ~AggrExpr() = default;

  ExprType type() const override
  {
    return ExprType::AGGREGATE;
  }

  AggrField &field()
  {
    return aggr_field_;
  }

  const AggrField &field() const
  {
    return aggr_field_;
  }

  const char *table_name() const
  {
    return aggr_field_.table_name();
  }

  const char *field_name() const
  {
    return aggr_field_.field_name();
  }

  const AggrType aggr_type() const {
    return aggr_field_.aggr_type();
  }

  RC get_value(const Tuple &tuple, TupleCell &cell) const override;
private:
  AggrField aggr_field_;
};