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
// Created by WangYunlai on 2021/6/9.
//

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <functional>

#include "sql/operator/aggregation_operator.h"
#include "sql/stmt/select_stmt.h"
#include "storage/common/table.h"
#include "rc.h"

RC AggregationOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("delete operator must has 1 child");
    return RC::INTERNAL;
  }

  Operator *child = children_[0];
  RC rc = child->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }
    aht_.InsertCombine(MakeAggregateKey(tuple), MakeAggregateValue(tuple));
  }
  aht_iterator_ = aht_.Begin();

  return RC::SUCCESS;
}

RC AggregationOperator::next()
{
  if (aht_iterator_ == aht_.End()) {
    return RC::RECORD_EOF;
  }
  AggregateKey key = aht_iterator_.Key();
  AggregateValue val = aht_iterator_.Val();
  ++aht_iterator_;
  // tuple_.set_aggregate_key(&aht_iterator_.Key());
  // tuple_.set_aggregate_val(&aht_iterator_.Val());
  return RC::SUCCESS;
}

RC AggregationOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple * AggregationOperator::current_tuple(){
  return nullptr;
}