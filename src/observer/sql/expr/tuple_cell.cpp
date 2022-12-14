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
// Created by WangYunlai on 2022/07/05.
//

#include <string>
#include <regex>
#include<cmath>
#include <sstream>

#include "sql/expr/tuple_cell.h"
#include "storage/common/field.h"
#include "common/log/log.h"
#include "util/comparator.h"
#include "util/util.h"

void TupleCell::to_string(std::ostream &os) const
{
  switch (attr_type_) {
  case INTS: {
    os << *(int *)data_;
  } break;
  case FLOATS: {
    float v = *(float *)data_;
    os << double2string(v);
  } break;
  case DATES: {
    int value = *(int *)data_;
    char buf[16] = {0};
    snprintf(buf,sizeof(buf),"%04d-%02d-%02d",value/10000,(value%10000)/100,value%100);
    os << buf;
  } break;
  case CHARS: {
    for (int i = 0; i < length_; i++) {
      if (data_[i] == '\0') {
        break;
      }
      os << data_[i];
    }
  } break;
  case TEXTS: {
    to_text(os);
  } break;
  case NULLS: {
    os << "null";
  } break;
  default: {
    LOG_WARN("unsupported attr type: %d", attr_type_);
  } break;
  }
}

void TupleCell::to_text(std::ostream &os) const {
  bool is_less = false;
  for (int i = PAGENUMSIZE; i < length_; i++) {
    if (data_[i] == '\0') {
      is_less = true;
      break;
    }
  }
  data_[length_] = '\0';
  os << data_ + PAGENUMSIZE;

  // for (int i = 0; i < TEXTPAGESIZE; i++) {
  //   if (text_data_[i] == '\0') {
  //     break;
  //   }
  // }
  if(!is_less) {
    text_data_[TEXTPAGESIZE-TEXTPATCHSIZE] = '\0';
    os << text_data_;
  }
}


int TupleCell::compare(const TupleCell &other) const
{
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
    case INTS: return compare_int(this->data_, other.data_);
    case FLOATS: return compare_float(this->data_, other.data_);
    case DATES: return compare_int(this->data_, other.data_);
    case CHARS: return compare_string(this->data_, this->length_, other.data_, other.length_);
    case NULLS: return 0;
    default: {
      LOG_WARN("unsupported type: %d", this->attr_type_);
    }
    }
  }

  if(this->attr_type_ == NULLS || other.attr_type_ == NULLS) {
    return -1;
  }
  
  if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = *(int *)data_;
    return compare_float(&this_data, other.data_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = *(int *)other.data_;
    return compare_float(data_, &other_data);
  }else if (this->attr_type_ == INTS && other.attr_type_ == CHARS) {
    float this_data = *(int *)data_;
    float other_data = std::atof((char *)(other.data_));
    return compare_float(&this_data, &other_data);
  }else if (this->attr_type_ == CHARS && other.attr_type_ == INTS) {
    float this_data = std::atof((char *)(data_));
    float other_data = *(int *)other.data_;
    return compare_float(&this_data, &other_data);
  }else if (this->attr_type_ == FLOATS && other.attr_type_ == CHARS) {
    float other_data = std::atof((char *)(other.data_));
    return compare_float(data_, &other_data);
  }else if (this->attr_type_ == CHARS && other.attr_type_ == FLOATS) {
    float this_data = std::atof((char *)(data_));
    return compare_float(&this_data, other.data_);
  }
  LOG_WARN("not supported");
  return -1; // TODO return rc?
}

int TupleCell::like_match(const TupleCell &other) const
{
  if (this->attr_type_ == other.attr_type_) {
   if(this->attr_type_ != CHARS) {
    LOG_WARN("like match unsupported type: %d", this->attr_type_);
    return -1;
   }
   char *s = (char *)this->data_;
   char *t = (char *)other.data();
   std::string str;
   for(int i =0;i < other.length();i++) {
    if(t[i] == '%') {
      str.push_back('.');
      str.push_back('*');
    }else if(t[i] == '_') {
      str.push_back('.');
    }else{
      str.push_back(t[i]);
    }
   }
   std::regex pattern(str);
   int ret = std::regex_match(s,pattern);
   return !ret;
  } 
  LOG_WARN("not supported");
  return -1; // TODO return rc?
}


bool TupleCell::null_compare(const TupleCell &other, CompOp comp) const{
  switch (comp) {
    case IS_TO: {
      return this->attr_type_ == other.attr_type_ ;
    } break;
    case IS_NOT: {
      return !(this->attr_type_ == other.attr_type_);
    } break;
    default: {
      return false;
    } break;
  }
  return false;
}
TupleCell TupleCell::Add(int i)
{
  int this_data = *(int *)data_;
  *(int *)data_ = this_data + i;
  return *this;
}

TupleCell TupleCell::Div(int num){
  switch (this->attr_type_){
  case INTS:{
    int this_data = *(int *)data_;
    *(float *)data_ = (1.0 * this_data) / num;
  }break;
  case FLOATS:{
    float this_data = *(float *)data_;
    *(float *)data_ = (1.0 * this_data) / num;
  }break;
  case DATES:{
    int this_data = *(int *)data_;
    *(float *)data_ = (1.0 * this_data) / num;
  }break;
  case CHARS:{
     int this_data = *(int *)data_;
    *(float *)data_ = (1.0 * this_data) / num;
  }break;
  case NULLS:
    break;
  default:
    break;
  };
  this->attr_type_ = FLOATS;
  return *this;
}

TupleCell TupleCell::Add(const TupleCell &other)
{
  switch (other.attr_type_){
  case INTS:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    *(int *)data_ = this_data + other_data;
  }break;
  case FLOATS:{
    float this_data = *(float *)data_;
    float other_data = *(float *)(other.data_);
    *(float *)data_ = this_data + other_data;
  }break;
  case DATES:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    *(int *)data_ = this_data + other_data;
  }break;
  case CHARS:{
    int cell_value = std::atoi((char *)other.data_);
    *(int *)(data_) += cell_value;
  }break;
  case NULLS:
    break;
  default:
    break;
  }
  return *this;
}
TupleCell TupleCell::Max(const TupleCell &other) 
{
  bool less = false;
  switch (other.attr_type_)
  {
  case INTS:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    if(this_data < other_data) {
      less = true;
    }
  }break;
  case FLOATS:{
    float this_data = *(float *)data_;
    float other_data = *(float *)(other.data_);
    if(this_data < other_data) {
      less = true;
    }
  }break;
  case DATES:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    if(this_data < other_data) {
      less = true;
    }
  }break;
  case CHARS:{
    if(compare_string(this->data_, this->length_, other.data_, other.length_) < 0) {
      less = true;
    }
  }break;
  case NULLS:
    
    break;
  default:
    break;
  }
  return less? other : *this;
}
TupleCell TupleCell::Min(const TupleCell &other) 
{
  bool then = false;
  switch (other.attr_type_)
  {
  case INTS:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    if(this_data > other_data) {
      then = true;
    }
  }break;
  case FLOATS:{
    float this_data = *(float *)data_;
    float other_data = *(float *)(other.data_);
    if(this_data > other_data) {
      then = true;
    }
  }break;
  case DATES:{
    int this_data = *(int *)data_;
    int other_data = *(int *)(other.data_);
    if(this_data > other_data) {
      then = true;
    }
  }break;
  case CHARS:{
    if(compare_string(this->data_, this->length_, other.data_, other.length_) > 0) {
      then = true;
    }
  }break;
  case NULLS:
    break;
  default:
    break;
  }
  return then ? other : *this;
}

bool TupleCell::type_conversion(AttrType target_type)
{
  if(attr_type_ != NULLS && attr_type_ != target_type) {
    char *new_data = new char[length_];
    memset(new_data, 0 ,length_);

    if (attr_type_ == INTS && target_type == FLOATS) {
       float val = *(int *)data_;
      *(float *)new_data = val;
    }else if (attr_type_ == INTS && target_type == CHARS) {
      std::string str = std::to_string(*(int *)data_);
      memcpy(new_data, str.c_str(), length_);
    }else if (attr_type_ == FLOATS && target_type == INTS) {
      int val = round(*(float *)data_);
      *(int *)new_data = val;
    }else if (attr_type_ == FLOATS && target_type == CHARS) {
      std::ostringstream oss;
      oss<<*(float *)data_;
      memcpy(new_data, oss.str().c_str(), length_);
    }else if (attr_type_ == CHARS && target_type == INTS) {
      int val = std::atoi((char *)data_);
      *(int *)new_data = val;
    }else if (attr_type_ == CHARS && target_type == FLOATS) {
      float val = std::atof((char *)data_);
      *(float *)new_data = val;
    }else if(attr_type_ == CHARS && target_type == TEXTS){
      attr_type_ = TEXTS;
    }else{
      return false;
    }
  }

  return true;
}

TupleCell TupleCell::create_zero_cell(AttrType attr_type) {
  char *data = new char[4];
  switch (attr_type)
  {
  case INTS: 
    *(int *)data = 0;
    break;
  case FLOATS:
    *(float *)data = 0.0;
    break;
  case DATES:
    *(int *)data = 0;
    break;
  case CHARS:
    for(int i =0;i < 4;i++) {
      data[i] = 0;
    }
    break;
  case NULLS:
    for(int i =0;i < 4;i++) {
      data[i] = 0;
    }
    break;
  default:
    break;
  }
  return TupleCell(attr_type, 4, data);
}

TupleCell TupleCell::create_max_cell(AttrType attr_type) {
  char *data = new char[4];
  switch (attr_type)
  {
  case INTS: 
    *(int *)data = INT32_MAX;
    break;
  case FLOATS:
    *(float *)data = FLT_MAX;
    break;
  case DATES:
    *(int *)data = INT32_MAX;
    break;
  case CHARS:
    for(int i =0;i < 4;i++) {
      data[i] = 127;
    }
    break;
  case NULLS:
    for(int i =0;i < 4;i++) {
      data[i] = 0;
    }
    break;
  default:
    break;
  }
  return TupleCell(attr_type,4,data);
}

TupleCell TupleCell::create_min_cell(AttrType attr_type) {
  char *data = new char[4];
  switch (attr_type)
  {
  case INTS: 
    *(int *)data = INT32_MIN;
    break;
  case FLOATS:
    *(float *)data = FLT_MIN;
    break;
  case DATES:
    *(int *)data = INT32_MIN;
    break;
  case CHARS:
    for(int i =0;i < 4;i++) {
      data[i] = 0;
    }
    break;
  case NULLS:
    for(int i =0;i < 4;i++) {
      data[i] = 0;
    }
    break;
  default:
    break;
  }
  return TupleCell(attr_type,4,data);
}