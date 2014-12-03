/*
 *
 * Copyright 2012 - 2014  Samsung Electronics Co., Ltd
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <vconf.h>

/* For multi-user support */
#include <tzplatform_config.h>

#include "memo-log.h"
#include "memo-db.h"
#include "db.h"

#ifndef MEMOAPI
#define MEMOAPI __attribute__ ((visibility("default")))
#endif

#define DB_PREFIX_PATH tzplatform_getenv(TZ_SYS_DB)
#define DBNAME ".memo.db"

#define USER_APPS_DIR tzplatform_getenv(TZ_USER_APP)

static void (*g_data_monitor) (void *) = NULL;
static int trans_count = 0;
static DBHandle *db;
static int ref_count = 0;

/******************************
* External API
*******************************/
/**
 * @fn            int memo_init(char *dbfile)
 * @brief        initialize memo db library
 * @param[in]    dbfile    db file path
 * @return        Return 0 (Success) or -1 (Failed)
 */
MEMOAPI int memo_init(char *dbfile)
{
    char *name = NULL;
    char defname[PATH_MAX];

    if(db) {
        ref_count++;
        return 0;
    }

    if(dbfile)
        name = dbfile;

    if(name == NULL) {
        snprintf(defname, sizeof(defname), "%s/%s",
                DB_PREFIX_PATH, DBNAME);
        name = defname;
    }

    DBG("DB name : %s", name);
    db = db_init(name);
    retv_if(db == NULL, -1);
    ref_count++;
    return 0;
}

/**
 * @fn            void memo_fini(void)
 * @brief        terminate memo db library
 * @return        None
 */
MEMOAPI void memo_fini(void)
{
    ref_count--;
    if (ref_count == 0) {
        db_fini(db);
        db = NULL;
    }
}

/**
 * @fn            struct memo_data* memo_create_data()
 * @brief        create memo data struct
 * @return        The pointer of created memo data
 */
MEMOAPI struct memo_data* memo_create_data()
{
    return (struct memo_data *)calloc(1, sizeof(struct memo_data));
}

/**
 * @fn            struct memo_data* memo_get_data(int id)
 * @brief        Get memo data with specific id
 * @param[in]    id    db id
 * @return        The pointer of memo data struct
 */
MEMOAPI struct memo_data* memo_get_data(int id)
{
    int rc;
    struct memo_data *md;

    retvm_if(db == NULL, NULL, "DB Handle is null, need memo_init");
    retvm_if(id < 1, NULL, "Invalid memo data id : %d", id);

    md = memo_create_data();
    retv_if(md == NULL, md);

    rc = get_data(db, id, md);
    if(rc) {
        memo_free_data(md);
        return NULL;
    }

    return md;
}

/**
 * @fn            void memo_free_data(struct memo_data *md)
 * @brief        deallocate memo data
 * @param[in]    md    memo data
 * @return        None
 */
MEMOAPI void memo_free_data(struct memo_data *md)
{
    ret_if(md == NULL);

    if(md->content)
        free(md->content);

    if(md->comment)
        free(md->comment);

    if(md->doodle_path)
        free(md->doodle_path);

    free(md);
}

/**
 * @fn            int memo_add_data(struct memo_data *md)
 * @brief        insert memo data
 * @param[in]    md    memo data struct
 * @return        Return id (Success) or -1 (Failed)
 */
MEMOAPI int memo_add_data(struct memo_data *md)
{
    retvm_if(db == NULL, -1, "DB Handle is null, need memo_init");
    return insert_data(db, md);
}

/**
 * @fn            int memo_mod_data(struct memo_data *md)
 * @brief        Update data in DB
 * @param[in]    md    The pointer of memo data
 * @return        Return 0 (Success) or -1 (Failed)
 */
MEMOAPI int memo_mod_data(struct memo_data *md)
{
    retvm_if(db == NULL, -1, "DB Handle is null, need memo_init");
    retvm_if(md == NULL, -1, "Update data is null");
    retvm_if(md->id < 1, -1, "Invalid memo data ID");
    return update_data(db, md);
}

/**
 * @fn            int memo_del_data(int id)
 * @brief        remove data of specific id from DB
 * @param[in]    id    db id
 * @return        Return 0 (Success) or -1 (Failed)
 */
MEMOAPI int memo_del_data(int id)
{
    char buf[128] = {0};
    retvm_if(db == NULL, -1, "DB Handle is null, need memo_init");
    retvm_if(id < 1, -1, "Invalid memo data ID");
    memo_data_t *md = memo_get_data(id);
    /* delete doodle */
    if(md->has_doodle == 1) {
	    snprintf(buf, 128, "%s/org.tizen.memo/data/doodle/%d.png", USER_APPS_DIR, id);
	    if(remove(buf) != 0) {
		ERR("Remove file error\n");
		memo_free_data(md);
		return -1;
	    }
    }
	memo_free_data(md);
    return remove_data(db, id);
}

/**
 * @fn            struct memo_data_list* memo_get_all_data_list()
 * @brief        Get the all data list
 * @return        the header of struct memo_data_list linked list
 */
MEMOAPI struct memo_data_list* memo_get_all_data_list(void)
{
    struct memo_data_list *mdl;

    retvm_if(db == NULL, NULL, "DB Handle is null, need memo_init");

    mdl = get_all_data_list(db);
    return mdl;
}

/**
 * @fn            void memo_free_data_list(struct memo_data_list *mdl)
 * @brief        deallocate memo data list
 * @param[in]    mdl        the pointer of memo_data_list struct
 * @return        None
 */
MEMOAPI void memo_free_data_list(struct memo_data_list *mdl)
{
    struct memo_data_list *t, *d;

    t = mdl;
    while(t) {
        d = t;
        t = t->next;
        if(d->md.content) free(d->md.content);
        free(d);
    }
}

/**
 * @fn            time_t memo_get_modified_time(int id)
 * @brief        Get modified time
 * @param[in]    id    db id
 * @return        modified time
 */
MEMOAPI time_t memo_get_modified_time(int id)
{
    retvm_if(db == NULL, -1, "DB Handle is null, need memo_init");

    return get_modtime(db, id);
}

/**
 * @fn            int memo_get_count(int *count)
 * @brief        Get number of memo
 * @return        0 on success
                 or -1 on fail
 *
 * Requested by widget memo
 * Added by jy.Lee (jaeyong911.lee@samsung.com)
 */
MEMOAPI int memo_get_count(int *count)
{
    int rc;
    retvm_if(db == NULL, -1, "DB Handle is null, need memo_init");
    retvm_if(count == NULL, -1, "count pointer is null");

    rc = get_data_count(db, count);
    if(rc) {
        return -1;
    }

    return 0;
}

MEMOAPI struct memo_operation_list* memo_get_operation_list(time_t stamp)
{
    struct memo_operation_list *mol;

    retvm_if(db == NULL, NULL, "DB Handle is null, need memo_init");

    mol = get_operation_list(db, stamp);
    return mol;
}

MEMOAPI void memo_free_operation_list(struct memo_operation_list *mol)
{
    struct memo_operation_list *t, *d;

    t = mol;
    while(t) {
        d = t;
        t = t->next;
        free(d);
    }
}

static void _on_data_change(keynode_t *node, void *user_data)
{
    if (g_data_monitor != NULL) {
        g_data_monitor(user_data);
    }
}

MEMOAPI int memo_subscribe_change(void (*cb)(void *), void *user_data)
{
    g_data_monitor = cb;
    vconf_notify_key_changed(VCONFKEY_MEMO_DATA_CHANGE, _on_data_change, user_data);
    return 0;
}

MEMOAPI int memo_unsubscribe_change(void (*cb)(void *))
{
    vconf_ignore_key_changed(VCONFKEY_MEMO_DATA_CHANGE, _on_data_change);
    g_data_monitor = NULL;
    return 0;
}

MEMOAPI void memo_begin_trans(void)
{
    trans_count++;
}

MEMOAPI void memo_end_trans(void)
{
    int value = 0;
    trans_count--;
    if (trans_count == 0) {
        if(vconf_get_int(VCONFKEY_MEMO_DATA_CHANGE, &value)) {
            LOGD("vconf_get_int FAIL\n");
        } else {
            if (value == 0) {
                vconf_set_int(VCONFKEY_MEMO_DATA_CHANGE, 1);
            } else {
                vconf_set_int(VCONFKEY_MEMO_DATA_CHANGE, 0);
            }
        }
    }
}

MEMOAPI int memo_get_indexes(int *aIndex, int len, MEMO_SORT_TYPE sort)
{
    return get_indexes(db, aIndex, len, sort);
}

MEMOAPI int memo_search_data(const char *search_str, int limit, int offset, MEMO_SORT_TYPE sort,
    memo_data_iterate_cb_t cb, void *user_data)
{
    return search_data(db, search_str, limit, offset, sort, cb, user_data);
}

MEMOAPI int memo_all_data(memo_data_iterate_cb_t cb, void *user_data)
{
    return all_data(db, cb, user_data);
}

