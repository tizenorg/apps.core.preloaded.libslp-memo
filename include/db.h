/*
 * Copyright (c) 2012 - 2014 Samsung Electronics Co., Ltd. All rights reserved
 *
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
 *
 */

#ifndef __LIBSLP_MEMO_DB_H__
#define __LIBSLP_MEMO_DB_H__

//#include <sqlite3.h> // changed to db-util.h
#include "db-util.h"
#include "memo-db.h"

sqlite3* db_init(char *);
void db_fini(sqlite3 *);

int insert_data(sqlite3 *, struct memo_data *);
int remove_data(sqlite3 *, int id);
int update_data(sqlite3 *, struct memo_data *);

int get_data(sqlite3 *, int , struct memo_data *);
struct memo_data_list* get_all_data_list(sqlite3 *);
struct memo_operation_list* get_operation_list(sqlite3 *db, time_t stamp);
int get_data_count(sqlite3 *db, int *count);

int has_id(sqlite3 *, int id);
time_t get_modtime(sqlite3 *, int id);
int get_indexes(sqlite3 *db, int *aIndex, int len, MEMO_SORT_TYPE sort);
int search_data(sqlite3 *db, const char *search_str, int limit, int offset, MEMO_SORT_TYPE sort,
        memo_data_iterate_cb_t cb, void *user_data);
int all_data(sqlite3 *db, memo_data_iterate_cb_t cb, void *user_data);

#define DBHandle sqlite3
//#define VCONFKEY_MEMO_DATA_CHANGE "db/memo/data-change"

#endif /* __LIBSLP_MEMO_DB_H__ */
