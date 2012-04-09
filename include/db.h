/*
 * Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved
 * 
 * This file is part of the libslp-memo
 * Written by Zhibin Zhou <zhibin.zhou@samsung.com>, Canjiang Lu <canjiang.lu@samsung.com>,
 *            Feng Li <feng.li@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.  SAMSUNG make no
 * representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for a particular purpose, or non-
 * infringement. SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.
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

#define MEMO_VCONF_PATH "db/memo/data-change"

#endif /* __LIBSLP_MEMO_DB_H__ */
