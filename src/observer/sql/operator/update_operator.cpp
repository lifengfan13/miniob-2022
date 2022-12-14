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
// Created by lifengfan on 2022/10/12.
//

#include "common/log/log.h"
#include "sql/operator/update_operator.h"
#include "storage/record/record.h"
#include "storage/common/table.h"
#include "sql/stmt/delete_stmt.h"

RC UpdateOperator::open()
{
  if (children_.size() != 1) {
    LOG_WARN("delete operator must has 1 child");
    return RC::INTERNAL;
  }
  RC rc = RC::SUCCESS;

  rc = children_[0]->open();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  Table *table = update_stmt_->table();
  auto update_fields = update_stmt_->update_fields();

  Operator *child = children_[0];
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
        LOG_WARN("failed to get current record: %s", strrc(rc));
        return rc;
    }
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record record = row_tuple->record();
    Record new_record = update_record(record);
    
    RC rc = table->update_record(nullptr, update_fields, &record, &new_record);
    if (rc != RC::SUCCESS) {
      delete new_record.data();
      LOG_WARN("failed to delete record: %s", strrc(rc));
      return rc;
    }
    delete new_record.data();
  }
  return RC::SUCCESS;
}

Record UpdateOperator::update_record(Record &record){
  // 复制所有字段的值
  auto table = update_stmt_->table();
  Record new_recold(record);
  int record_size = table->table_meta().record_size();
  char *record_data = new char[record_size];

  memcpy(record_data, record.data(), record_size);

  // null_meta 描述 空值元数据
  auto null_meta = table->table_meta().null_meta();
  int bit_map = *(int *)(record_data + null_meta->offset());

  // update correspond field
  for(auto &update: update_stmt_->update_fields()) {
    auto field_meta = update.meta();

    Value value;
    if(update.is_has_subselect()) {
       value = update.select_value();
    }else{
      value = update.value();
    }
    size_t copy_len = field_meta->len();

    if(value.type == NULLS) {
      bit_map |= 1 << field_meta->col_num();
    }else{
       if (field_meta->type() == CHARS) {
        const size_t data_len = strlen((const char *)value.data);
        if (copy_len > data_len) {
          copy_len = data_len + 1;
        }
      }
      if(field_meta->type() == TEXTS) {
        char *text_data = (char *)value.data;
        RC rc = table->update_text_record(record_data + field_meta->offset(), text_data + TEXTPATCHSIZE);
        memcpy(record_data + field_meta->offset() + PAGENUMSIZE, text_data, TEXTPATCHSIZE);
      }else{
        memcpy(record_data + field_meta->offset(), value.data, copy_len);
      }
      bit_map &= ~(1 << field_meta->col_num());
    }
  }
  memcpy(record_data + null_meta->offset(), (char *)&bit_map, null_meta->len());

  new_recold.set_data(record_data);
  return new_recold;
}

RC UpdateOperator::next()
{
  return RC::RECORD_EOF;
}

RC UpdateOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}