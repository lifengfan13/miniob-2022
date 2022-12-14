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
// Created by Meiyi
//

#ifndef __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
#define __OBSERVER_SQL_PARSER_PARSE_DEFS_H__

#include <stddef.h>
#include <stdbool.h>

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50

#define PAGENUMSIZE 4
#define TEXTPATCHSIZE 28
#define TEXTSIZE (PAGENUMSIZE+TEXTPATCHSIZE)
#define TEXTPAGESIZE 4096

typedef enum {
  EQUAL_TO,     //"="     0
  LESS_EQUAL,   //"<="    1
  NOT_EQUAL,    //"!="    2
  LESS_THAN,    //"<"     3
  GREAT_EQUAL,  //">="    4
  GREAT_THAN,   //">"     5
  LIKE_TO,
  NOT_LIKE,
  IS_TO,
  IS_NOT,
  NO_OP
} CompOp;

typedef enum {
  IN_TO,
  NOT_INT,
  EXISTS,
  NOT_EXISTS,
}SetOp;

//属性值类型
typedef enum
{
  UNDEFINED,
  CHARS,
  INTS,
  FLOATS,
  DATES,
  TEXTS,
  NULLS
} AttrType;

typedef enum
{
  LENGTH_FUNC,
  ROUND_FUNC,
  DATE_FORMAT_FUNC
} FuncType;

typedef enum
{
  MAX_FUNC,
  MIN_FUNC,
  COUNT_FUNC,
  AVG_FUNC,
  SUM_FUNC,
} AggrType;

typedef enum
{
  ORDER_ASC,
  ORDER_DESC
} OrderType;

//属性结构体
typedef struct {
  char *relation_name;   // relation name (may be NULL) 表名
  char *attribute_name;  // attribute name              属性名
} RelAttr;

//属性值
typedef struct _Value {
  AttrType type;  // type of value
  void *data;     // value
} Value;

//查询类型
typedef struct {
  RelAttr rel_attr;
  AggrType aggr_type;
}AggrAttr;

typedef struct _Condition {
  int left_is_attr;    // TRUE if left-hand side is an attribute
                       // 1时，操作符左边是属性名，0时，是属性值
  Value left_value;    // left-hand side value if left_is_attr = FALSE
  RelAttr left_attr;   // left-hand side attribute
  CompOp comp;         // comparison operator
  int right_is_attr;   // TRUE if right-hand side is an attribute
                       // 1时，操作符右边是属性名，0时，是属性值
  RelAttr right_attr;  // right-hand side attribute if right_is_attr = TRUE 右边的属性
  Value right_value;   // right-hand side value if right_is_attr = FALSE
} Condition;

typedef struct {
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
}JoinCond;

//排序属性结构
typedef struct {
  RelAttr rel_attr;
  OrderType order_type;
} OrderAttr;

// struct of select
typedef struct {
  size_t attr_num;                
  RelAttr attributes[MAX_NUM];  

  size_t aggr_num;                
  AggrAttr aggr_attributes[MAX_NUM]; 

  size_t relation_num;            // Length of relations in Fro clause
  char *relations[MAX_NUM];       // relations in From clause

  size_t join_num;              // Length of conditions in Where clause
  JoinCond join_conditions[MAX_NUM];  // conditions in Where clause

  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause

  size_t order_num;               // order by
  OrderAttr order_attributes[MAX_NUM];

  size_t group_num;               // group by
  RelAttr group_attrs[MAX_NUM];
} Selects;

// struct of InsertRecord
typedef struct {
  size_t value_num;       // Length of values
  Value values[MAX_NUM];  // values to insert
} InsertRecord;

// struct of insert
typedef struct {
  char *relation_name;    // Relation to insert into
  size_t record_num;       // Length of values
  InsertRecord records[MAX_NUM];  // values to insert
} Inserts;

// struct of delete
typedef struct {
  char *relation_name;            // Relation to delete from
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Deletes;

typedef struct {
  char *attribute_name;           // Attribute to update
  int is_sub_select;
  Value value; 
  Selects sub_select;
}UpdateRecord;

// struct of update
typedef struct {
  char *relation_name;            // Relation to update
  size_t update_num;
  UpdateRecord update_records[MAX_NUM];
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
} Updates;

typedef struct {
  char *name;     // Attribute name
  AttrType type;  // Type of attribute
  size_t length;  // Length of attribute
  int nullable;
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relation_name;           // Relation name
  size_t attribute_count;        // Length of attribute
  AttrInfo attributes[MAX_NUM];  // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relation_name;  // Relation name
} DropTable;

// struct of create_index
typedef struct {
  char *index_name;      // Index name
  char *relation_name;   // Relation name  
  size_t attr_num;           
  char *attributes[MAX_NUM];  // Attribute name list
  bool unique; 
} CreateIndex;

// struct of create_index
typedef struct {
  char *relation_name;   // Relation name  
} ShowIndex;

// struct of  drop_index
typedef struct {
  const char *index_name;  // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

typedef struct {
  const char *relation_name;
  const char *file_name;
} LoadData;

union Queries {
  Selects selection;
  Inserts insertion;
  Deletes deletion;
  Updates update;
  CreateTable create_table;
  DropTable drop_table;
  CreateIndex create_index;
  ShowIndex show_index;
  DropIndex drop_index;
  DescTable desc_table;
  LoadData load_data;
  char *errors;
};

// 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_SHOW_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT
};
// struct of flag and sql_struct
typedef struct Query {
  enum SqlCommandFlag flag;
  union Queries sstr;
} Query;

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

void relation_attr_init(RelAttr *relation_attr, const char *relation_name, const char *attribute_name);
void relation_attr_destroy(RelAttr *relation_attr);

void value_init_integer(Value *value, int v);
void value_init_float(Value *value, float v);
void value_init_string(Value *value, const char *v);
void value_init_date(Value *value, const char *v);
void value_init_null(Value *value);
void value_destroy(Value *value);

void condition_init(Condition *condition, CompOp comp, int left_is_attr, RelAttr *left_attr, Value *left_value,
    int right_is_attr, RelAttr *right_attr, Value *right_value);
void condition_destroy(Condition *condition);

void init_join_condition(JoinCond *join_cond, Condition conditions[], size_t condition_num);
void join_condition_destroy(JoinCond *join_cond);

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type, size_t length, int nullable);
void attr_info_destroy(AttrInfo *attr_info);

void aggr_attr_int(AggrAttr *aggr_attr, RelAttr* rel_attr, AggrType aggr_type);
void aggr_attr_destroy(AggrAttr *aggr_attr);

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr* rel_attr);
void selects_append_aggr_attribute(Selects *selects, AggrAttr *aggr_attr);
void selects_append_relation(Selects *selects, const char *relation_name);
void selects_append_conditions(Selects *selects, Condition conditions[], size_t condition_num);
void selects_append_join_conditions(Selects *selects, JoinCond *join_cond);
void selects_append_order_by(Selects *selects, OrderAttr *order_attr);
void selects_append_group_by(Selects *selects, RelAttr *rel_attr);
void selects_copy(Selects *selects, Selects *sub_selects);
void selects_destroy(Selects *selects);

void inserts_init(Inserts *inserts, const char *relation_name, size_t record_num);
void inserts_record(Inserts *inserts, size_t record_num,Value values[], size_t value_num);
void inserts_destroy(Inserts *inserts);

void deletes_init_relation(Deletes *deletes, const char *relation_name);
void deletes_set_conditions(Deletes *deletes, Condition conditions[], size_t condition_num);
void deletes_destroy(Deletes *deletes);

void updates_init(Updates *updates, const char *relation_name, Condition conditions[], size_t condition_num);
void updates_value_append(Updates *updates, const char *attribute_name, Value *value);
void updates_select_append(Updates *updates, const char *attribute_name, Selects *selects);
void updates_destroy(Updates *updates);

void create_table_append_attribute(CreateTable *create_table, AttrInfo *attr_info);
void create_table_init_name(CreateTable *create_table, const char *relation_name);
void create_table_destroy(CreateTable *create_table);

void drop_table_init(DropTable *drop_table, const char *relation_name);
void drop_table_destroy(DropTable *drop_table);

void create_index_init(CreateIndex *create_index, const char *index_name, const char *relation_name);
void create_index_append(CreateIndex *create_index, const char *attr_name);
void create_index_destroy(CreateIndex *create_index);

void show_index_init(ShowIndex *show_index, const char *relation_name);

void drop_index_init(DropIndex *drop_index, const char *index_name);
void drop_index_destroy(DropIndex *drop_index);

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void load_data_init(LoadData *load_data, const char *relation_name, const char *file_name);
void load_data_destroy(LoadData *load_data);

void query_init(Query *query);
Query *query_create();  // create and init
void query_reset(Query *query);
void query_destroy(Query *query);  // reset and delete

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
