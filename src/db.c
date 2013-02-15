/*
*
* Copyright 2012  Samsung Electronics Co., Ltd
*
* Licensed under the Flora License, Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://floralicense.org/license/
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sqlite3.h>

#include "memo-log.h"
#include "memo-db.h"
#include "db-schema.h"
#include "db.h"
#include "db-helper.h"

#define QUERY_MAXLEN        5120
#define NFS_TEST

#define TEXT(s, n) (char *)sqlite3_column_text(s, n)
#define INT(s, n) sqlite3_column_int(s, n)

static char* _d(char *str)
{
    if(str == NULL /*|| strlen(str) == 0*/) return NULL;
    return strdup(str);
}

static int _exec(sqlite3 *db, char *query)
{
    int rc;
    char *errmsg = NULL;

    retvm_if(db == NULL, -1, "DB handler is null");

    rc = sqlite3_exec(db, query, NULL, 0, &errmsg);
    if(rc != SQLITE_OK) {
        DBG("Query: [%s]", query);
        ERR("SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    return 0;
}

static int _create_table(sqlite3 *db)
{
    int rc;

    rc = _exec(db, CREATE_MEMO_TABLE);
    retv_if(rc == -1, -1);

    return 0;
}

static inline void _make_qry_i_cd(char *query, int len, struct memo_data *cd)
{
    char * buf = db_make_insert_query(
        KEY_ITEM_MODE, (void *)cd->has_doodle,
        KEY_CONTENT, db_content_truncate(cd->content),
        KEY_FONT_RESPECT, cd->font_respect,
        KEY_FONT_SIZE, ((cd->font_respect ? cd->font_size : 44)),
        KEY_FONT_COLOR, (cd->font_respect ? cd->font_color : 0xff000000),
        KEY_COMMENT, cd->comment,
        KEY_DOODLE_PATH, cd->doodle_path,
        KEY_INPUT_END);
    if (buf != NULL) {
        snprintf(query, len, "%s", buf);
        free(buf);
    }
}

int insert_data(sqlite3 *db, struct memo_data *cd)
{
    int rc = 0;
    char *query = NULL;
    int len = 512;

    retvm_if(db == NULL, -1, "DB handler is null");
    retvm_if(cd == NULL, -1, "Insert data is null");

    if (cd->content != NULL) {
        len += strlen(cd->content);
    }
    if (cd->comment != NULL) {
        len += strlen(cd->comment);
    }
    query = (char *)malloc(len);
    retv_if(query == NULL, -1);
    _make_qry_i_cd(query, len, cd);
    memo_begin_trans();
    rc = _exec(db, query);
    memo_end_trans();
    free(query);
    retv_if(rc == -1, rc);
    cd->id = sqlite3_last_insert_rowid(db);
    DBG("Memo id : %d", cd->id);
    return cd->id;
}

static inline void _make_qry_d_cd(char *query, int len, struct memo_data *cd)
{
    char *buf = db_make_delete_query(cd->id);
    if (buf != NULL) {
        snprintf(query, len, "%s", buf);
        free(buf);
    }
}

int remove_data(sqlite3 *db, int cid)
{
    int rc;
    char query[QUERY_MAXLEN];

    retvm_if(db == NULL, -1, "DB handler is null");
    retvm_if(cid < 1, -1, "Invalid memo data ID");
    if (has_id(db, cid) != 1) {
        return -1;
    }

    struct memo_data memo;
    memo.id = cid;
    _make_qry_d_cd(query, sizeof(query), &memo);
    memo_begin_trans();
    rc = _exec(db, query);
    memo_end_trans();
    retv_if(rc == -1, rc);
    return 0;
}

static inline void _make_qry_u_cd(char *query, int len, struct memo_data *cd)
{
    char *buf = db_make_update_query(cd->id,
        KEY_FAVORITE, (void *)cd->favorite,
        KEY_CONTENT, db_content_truncate(cd->content),
        KEY_FONT_RESPECT, cd->font_respect,
        KEY_FONT_SIZE, ((cd->font_respect ? cd->font_size : 44)),
        KEY_FONT_COLOR, (cd->font_respect ? cd->font_color : 0xff000000),
        KEY_COMMENT, cd->comment,
        KEY_DOODLE_PATH, cd->doodle_path,
        KEY_INPUT_END);
    if (buf != NULL) {
        snprintf(query, len, "%s", buf);
        free(buf);
    }
}

int update_data(sqlite3 *db, struct memo_data *cd)
{
    int rc;
    char *query = NULL;
    int len = 512;

    retvm_if(db == NULL, -1, "DB handler is null");
    retvm_if(cd == NULL, -1, "Update data is null");

    if (cd->content != NULL) {
        len += strlen(cd->content);
    }
    if (cd->comment != NULL) {
        len += strlen(cd->comment);
    }
    query = (char *)malloc(len);
    retv_if(query == NULL, -1);
    _make_qry_u_cd(query, len, cd);
    memo_begin_trans();
    rc = _exec(db, query);
    memo_end_trans();
    free(query);
    retv_if(rc == -1, rc);
    return 0;
}

static int _get_cd(sqlite3 *db, int cid, struct memo_data *cd)
{
    int rc;
    char query[QUERY_MAXLEN];
    sqlite3_stmt *stmt;
    int idx;

    snprintf(query, sizeof(query), "select content, modi_time, doodle, color, comment, favorite, font_respect, font_size, font_color, doodle_path "
            "from memo where id = %d and delete_time = -1",
            cid);

    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        idx = 0;
        cd->content = _d(TEXT(stmt, idx++));
        //strncpy(cd->written_time, _s(TEXT(stmt, idx++)), DATE_LEN);
        cd->modi_time = INT(stmt, idx++);
        cd->has_doodle = INT(stmt, idx++);
        cd->color = INT(stmt, idx++);
        cd->comment = _d(TEXT(stmt, idx++));
        cd->favorite = INT(stmt, idx++);
        cd->font_respect = INT(stmt, idx++);
        cd->font_size = INT(stmt, idx++);
        cd->font_color = INT(stmt, idx++);
        cd->doodle_path = _d(TEXT(stmt, idx++));
    } else {
        retvm_if(1, -1, "Contact data %d does not exist", cid);
    }
    rc = sqlite3_finalize(stmt);

    return 0;
}

int get_data(sqlite3 *db, int cid, struct memo_data *cd)
{
    int rc;

    retvm_if(db == NULL, -1, "DB handler is null");
    retvm_if(cd == NULL, -1, "Output struct is null");
    retvm_if(cid < 1, -1, "Invalid memo ID");

    rc = _get_cd(db, cid, cd);
    retv_if(rc == -1, rc);

    cd->id = cid;
    return 0;
}

static struct memo_data_list* _get_data_list(sqlite3 *db, const char* query)
{
    int rc;
    sqlite3_stmt *stmt;
    struct memo_data_list *cd = NULL;
    struct memo_data_list *t;
    int idx;

    retvm_if(db == NULL, cd, "DB handler is null");

    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return NULL;
    }

    rc = sqlite3_step(stmt);
    while(rc == SQLITE_ROW) {
        idx=0;
        t = (struct memo_data_list *)calloc(1, sizeof(struct memo_data_list));
        if (t == NULL) {
            continue;
        }
        t->md.id = INT(stmt, idx++);
        t->md.content = _d(TEXT(stmt, idx++));
        //strncpy(t->md.written_time, _s(TEXT(stmt, idx++)), DATE_LEN);
        t->md.modi_time = INT(stmt, idx++);
        t->md.has_doodle = INT(stmt, idx++);
        t->md.color = INT(stmt, idx++);        // jwh : color added
        t->md.comment = _d(TEXT(stmt, idx++));
        t->md.favorite = INT(stmt, idx++);
        t->md.font_respect = INT(stmt, idx++);
        t->md.font_size = INT(stmt, idx++);
        t->md.font_color = INT(stmt, idx++);
        t->md.doodle_path = _d(TEXT(stmt, idx++));

        t->next = NULL;
        t->prev = NULL;
        if (cd != NULL) {
            t->next = cd;
            cd->prev = t;
        }
        cd = t;
        rc = sqlite3_step(stmt);
    }
    rc = sqlite3_finalize(stmt);

    return cd;
}

struct memo_data_list* get_all_data_list(sqlite3 *db)
{
    char query[QUERY_MAXLEN];
    retvm_if(db == NULL, NULL, "db handler is null");

    snprintf(query, sizeof(query),    "select "
                                    "id, content, modi_time, doodle, color, comment, favorite, font_respect, font_size, font_color, doodle_path "
                                    "from memo where delete_time = -1 order by create_time asc");

    return _get_data_list(db, query);
}

struct memo_operation_list* get_operation_list(sqlite3 *db, time_t stamp)
{
    char query[QUERY_MAXLEN];
    int rc;
    sqlite3_stmt *stmt;
    time_t create_tm, mod_tm, del_tm;
    struct memo_operation_list *t = NULL;
    struct memo_operation_list *cd = NULL;
    int idx;

    retvm_if(db == NULL, NULL, "db handler is null");
    snprintf(query, sizeof(query),    "select "
                                    "id, create_time, modi_time, delete_time "
                                    "from memo where modi_time > %ld", stamp);
    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return NULL;
    }

    rc = sqlite3_step(stmt);
    while(rc == SQLITE_ROW) {
        idx=0;
        t = (struct memo_operation_list *)malloc(sizeof(struct memo_operation_list));
        if (t == NULL) {
            continue;
        }
        t->id = INT(stmt, idx++);
        create_tm = INT(stmt, idx++);
        mod_tm = INT(stmt, idx++);
        del_tm = INT(stmt, idx++);
        if (del_tm != -1) {
            t->operation = MEMO_OPERATION_DELETE;
        } else if (stamp < create_tm) {
            t->operation = MEMO_OPERATION_ADD;
        } else {
            t->operation = MEMO_OPERATION_UPDATE;
        }
        t->next = cd;
        cd = t;
        rc = sqlite3_step(stmt);
    }
    rc = sqlite3_finalize(stmt);

    return cd;
}

int has_id(sqlite3 *db, int cid)
{
    int rc;
    int ret = 0;
    char query[QUERY_MAXLEN];
    sqlite3_stmt *stmt;

    retvm_if(db == NULL, ret, "DB handler is null");
    retvm_if(cid < 1, ret, "Invalid memo data ID");

    snprintf(query, sizeof(query), "select id from memo where id = %d", cid);

    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        ret = 1; // exist
    }
    rc = sqlite3_finalize(stmt);

    return ret;
}

time_t get_modtime(sqlite3 *db, int cid)
{
    int rc;
    time_t ret = -1;
    char query[QUERY_MAXLEN];
    sqlite3_stmt *stmt;

    retvm_if(db == NULL, ret, "DB handler is null");
    retvm_if(cid < 1, ret, "Invalid memo data ID");

    snprintf(query, sizeof(query), "select modi_time from memo where id = %d", cid);

    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        ret = INT(stmt, 0);
    }
    rc = sqlite3_finalize(stmt);

    return ret;
}

sqlite3* db_init(char *root)
{
    int rc;
    sqlite3 *db = NULL;

    //rc = sqlite3_open(root, &db); // changed to db_util_open
    rc = db_util_open(root, &db, DB_UTIL_REGISTER_HOOK_METHOD);
    //rc = db_util_open(root, &db, 0);
    if(rc) {
        ERR("Can't open database: %s", sqlite3_errmsg(db));
        //sqlite3_close(db);// changed to db_util_close
        db_util_close(db);
        return NULL;
    }

    rc = _create_table(db);
    if(rc) {
        ERR("Can't create tables: %s", sqlite3_errmsg(db));
        //sqlite3_close(db);// changed to db_util_close
        db_util_close(db);
        return NULL;
    }

    return db;
}

void db_fini(sqlite3 *db)
{
    if(db) {
        //sqlite3_close(db); // changed to db_util_close
        db_util_close(db);
    }
}

/*
* @fn int get_latest_data(sqlite3 *db, struct memo_data *cd)
* @brif Get latest memo data from db
*
* Request by Widget Memo application.
* Added by jy.Lee (jaeyong911.lee@samsung.com)
*/
int get_data_count(sqlite3 *db, int *count)
{
    int rc;
    char query[QUERY_MAXLEN];
    sqlite3_stmt *stmt;
    int idx;
    int ret = 0;

    snprintf(query, sizeof(query), "select count(id) from memo where delete_time = -1");

    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if (SQLITE_OK != rc || NULL == stmt) {
        ERR("SQL error\n");
        sqlite3_finalize(stmt);
        return -1;
    }

    rc = sqlite3_step(stmt);
    if(rc == SQLITE_ROW) {
        idx = 0;
        *count = INT(stmt, idx++);
    } else {
        ret = -1; //retvm_if(1, -1, "data does not exist");
    }
    rc = sqlite3_finalize(stmt);

    return ret;
}

/**
 * @brief     Get indexes from database, if aIndex is NULL this function returns the number of records in database.
 *
 * @param     [in] db              handle of sqlite3 database
 *
 * @param     [out] aIndex         buffer to store retrieved indexes
 *
 * @param     [in] len             length of aIndex, the maximum number of indexes to be retrieved
 *
 * @param     [in] sort            sort type of result, currently not used(create_time descend defaultly)
 *
 * @return    number of retrieved indexes or number of records
 *
 * @remarks   If the number of indexes storded in database is larger than len,
              the trailing indexes will be omitted.
 *
 */
int get_indexes(sqlite3 *db, int *aIndex, int len, MEMO_SORT_TYPE sort)
{
    int rc = 0;
    sqlite3_stmt *stmt = NULL;
    char query[128] = {0};
    int i = 0;
    const char *str_sort = "order by create_time desc";

    retvm_if(db == NULL, 0, "db handler is null");
    retvm_if(len < 0, 0, "index buffer length invalid");

    if (aIndex == NULL) { /* return number of records */
        get_data_count(db, &i);
        return i;
    }

    /* ugh, get sort type dynamically */
    snprintf(query, sizeof(query), "select id from memo where delete_time = -1 %s limit %d", str_sort, len);
    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if ((rc == SQLITE_OK) && (stmt != NULL)) {
        rc = sqlite3_step(stmt);
        while((rc==SQLITE_ROW)) { /* loop times depends on limit keyword, aIndex will not overflow */
            aIndex[i++] = INT(stmt, 0);
            rc = sqlite3_step(stmt);
        }
    }
    rc = sqlite3_finalize(stmt);
    return i;
}


static const char *_get_sort_exp(MEMO_SORT_TYPE sort)
{
    const char *exp = "create_time DESC"; /* default sort type */
    switch (sort) {
    case MEMO_SORT_CREATE_TIME:
        exp = "create_time DESC";
        break;
    case MEMO_SORT_CREATE_TIME_ASC:
        exp = "create_time ASC";
        break;
    case MEMO_SORT_TITLE:
        exp = "CASE WHEN comment IS NOT NULL THEN comment ELSE content END DESC";
        break;
    case MEMO_SORT_TITLE_ASC:
        exp = "CASE WHEN comment IS NOT NULL THEN comment ELSE content END ASC";
        break;
    default:
        break;
    }
    return exp;
}

int search_data(sqlite3 *db, const char *search_str, int limit, int offset, MEMO_SORT_TYPE sort,
    memo_data_iterate_cb_t cb, void *user_data)
{
    retvm_if(db == NULL, -1, "db handler is NULL");
    retvm_if(search_str == NULL, -1, "search string is NULL");

    int rc = 0;
    sqlite3_stmt *stmt = NULL;
    char query[QUERY_MAXLEN] = {0};
    int idx = 0;
    memo_data_t *md = (memo_data_t *)calloc(1, sizeof(memo_data_t));
    retvm_if(md == NULL, -1, "calloc failed");

    snprintf(query, sizeof(query),
        "SELECT id, content, modi_time, doodle, comment, font_respect, font_size, font_color "
        "FROM memo WHERE delete_time = -1 AND "
        "CASE WHEN comment IS NOT NULL THEN comment like '%%%s%%' "
        "ELSE content LIKE '%%%s%%' END "
        "ORDER BY %s LIMIT %d OFFSET %d",
        search_str, search_str, _get_sort_exp(sort), limit, offset);
    LOGD("[query] : %s\n", query);
    rc = sqlite3_prepare(db, query, -1, &stmt, NULL);
    if ((rc == SQLITE_OK) && (stmt != NULL)) {
        rc = sqlite3_step(stmt);
        while((rc==SQLITE_ROW)) {
            idx=0;
            md->id = INT(stmt, idx++);
            md->content = TEXT(stmt, idx++);
            md->modi_time = INT(stmt, idx++);
            md->has_doodle = INT(stmt, idx++);
            md->comment = TEXT(stmt, idx++);
            md->font_respect = INT(stmt, idx++);
            md->font_size = INT(stmt, idx++);
            md->font_color = INT(stmt, idx++);
            cb(md, user_data); /* callback */
            rc = sqlite3_step(stmt);
        }
    }
    rc = sqlite3_finalize(stmt);
    free(md);
    return 0;
}

int all_data(sqlite3 *db, memo_data_iterate_cb_t cb, void *user_data)
{
    retvm_if(db == NULL, -1, "db handler is NULL");
    retvm_if(cb == NULL, -1, "iterator callback is NULL");

    int rc = 0;
    sqlite3_stmt *stmt = NULL;
    int idx = 0;
    memo_data_t *md = (memo_data_t *)calloc(1, sizeof(memo_data_t));
    retvm_if(md == NULL, -1, "calloc failed");

    rc = sqlite3_prepare(db,
        "SELECT id, content, modi_time, doodle, comment, font_respect, font_size, font_color "
        "FROM memo where delete_time = -1 order by create_time desc",
        -1, &stmt, NULL);
    if ((rc == SQLITE_OK) && (stmt != NULL)) {
        rc = sqlite3_step(stmt);
        while((rc==SQLITE_ROW)) {
            idx=0;
            md->id = INT(stmt, idx++);
            md->content = TEXT(stmt, idx++);
            md->modi_time = INT(stmt, idx++);
            md->has_doodle = INT(stmt, idx++);
            md->comment = TEXT(stmt, idx++);
            md->font_respect = INT(stmt, idx++);
            md->font_size = INT(stmt, idx++);
            md->font_color = INT(stmt, idx++);
            cb(md, user_data); /* callback */
            rc = sqlite3_step(stmt);
        }
    }
    rc = sqlite3_finalize(stmt);
    free(md);
    return 0;
}

