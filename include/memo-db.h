/*
  * Copyright 2012  Samsung Electronics Co., Ltd
  * 
  * Licensed under the Flora License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  * 
  *     http://www.tizenopensource.org/license
  * 
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */

/*!
 * @defgroup app_lib Application library
 * This is a framework for User interface
 */

/*!
 * @defgroup memo_db_group Memo DB Library
 * @ingroup app_lib
 * Functions to APIs to access memo DB.
 */

/**
 * @file                memo-db.h
 * @ingroup             memo_db_group
 * @brief               This library provides APIs to access memo DB.
 * @author              Canjiang Lu <canjiang.lu@samsung.com>
 * @date                2010-09-15
 * @version             0.1.3-51
 */

#ifndef __MEMO_DB_H__
#define __MEMO_DB_H__

#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def DATE_LEN
 * Maxium length of date
 */
#define DATE_LEN 17

/**
* @struct memo_data
* @brief This structure defines memo information.
*/
typedef struct memo_data {
    unsigned int id; /**< index of memo record */
    char *content; /**< content */
    char written_time[DATE_LEN]; /**< wiritten time */
    time_t modi_time; /**< modify time stamp */
    int has_doodle; /**< has doodle image */
    unsigned int color; /**< background color */
    char *comment;
    int favorite;
    int font_respect; /**< distiguish records synchronized from KIES which has no font information */
    int font_size;
    int font_color;
    char *doodle_path;
} memo_data_t;

/**
 * @struct memo_data_list
 * @brief List for memo data
 */
typedef struct memo_data_list {
    struct memo_data md; /**< memo data */
    struct memo_data_list *prev; /**< Previous list */
    struct memo_data_list *next; /**< Next list */
} memo_data_list_t;

/**
 * @brief Enum values for db operation
 */
enum {
   MEMO_OPERATION_ADD, /**< operation add */
   MEMO_OPERATION_UPDATE, /**< operation update */
   MEMO_OPERATION_DELETE, /**< operation delete */
};

typedef enum {
    MEMO_SORT_INVALID = -1,
    MEMO_SORT_CREATE_TIME, /* descend */
    MEMO_SORT_CREATE_TIME_ASC,
    MEMO_SORT_TITLE, /* descend */
    MEMO_SORT_TITLE_ASC,
    MEMO_SORT_TYPES,
}MEMO_SORT_TYPE;

/**
 * @struct memo_operation_list
 * @brief List for memo data operation
 */
struct memo_operation_list {
    int id; /**< index of memo record */
    int operation; /**< operation type */
    struct memo_operation_list *next; /**< Next list */
};

/**
 * This function init memo database, create db file to the @param dbfile if the dbfile is not null.
 * If the file path is NULL, the db file will be created to the default path.
 *
 * @brief       Initialize Memo-Database
 *
 * @param       [in]   dbfile    the path of user defined db file.
 *
 * @return     On success, 0 is returned. On error, -1 is returned
 *
 * @remarks  The function must be called fristly before calling other functions of memo-db.
 *
 * @exception   None
 *
 * @see memo_fini
 *
 * \par Sample code:
 * \code
 * ...
 * memo_init(NULL);
 * ...
 * \endcode
 */
int memo_init(char *dbfile);

/**
 * This function fini memo database, it will close db and free db resource
 *
 * @brief       Finialize Memo-Database
 *
 * @return     None
 *
 * @remarks  The function must be called finally when the application don't need db
 *
 * @exception   None
 *
 * @see memo_init
 *
 * \par Sample code:
 * \code
 * ...
 * memo_fini();
 * ...
 * \endcode
 */
void memo_fini(void);

/**
 * This function create a pointer to struct memo_data.
 *
 * @brief       Create memo data structure, Ref:  memo_free_data()
 *
 * @return      This function returns  a pointer of memo_data on  success or NULL on failure.
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_free_data
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * md = memo_create_data();
 * if (md == NULL) {
 *     return;
 * }
 * ...
 * \endcode
 */
struct memo_data* memo_create_data(void);

/**
 * This function gets the data of the record assosiated with id.
 * It will connect to db and search by id.
 *
 *
 * @brief       Get a record data by id
 *
 * @param       [in]    id        the id of the memo record
 *
 * @return     This function returns  a pointer of memo_data on  success or NULL on failure.
 *
 * @remarks  The function must be called after memo_init(), and also  given the correct id.
 *
 * @exception   None
 *
 * @see
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * md = memo_get_data(id);
 * if (md == NULL) {
 *     return;
 * } else {
 *     memo_free_data(md);
 * }
 * ...
 * \endcode
 */
struct memo_data* memo_get_data(int id);

/**
 * This function free the data: struct memo_data.
 * The function must be called after memo_create_data()
 *
 * @brief      Free data struct
 *
 * @param     [in] md a pointer to  struct memo_data*
 *
 * @return      None
 *
 * @remarks When application don't need the memo_data, must call the function to free the memory
 *
 * @exception   None
 *
 * @see memo_create_data
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * md = memo_get_data(id);
 * if (md == NULL) {
 *     return;
 * } else {
 *     memo_free_data(md);
 * }
 * ...
 * \endcode
 */
void memo_free_data(struct memo_data *md);

/**
 * This function add the data: struct memo_data.
 * This function is usually called together with memo_create_data();
 *
 * @brief      Insert a data to DB
 *
 * @param     [in]  md   a pointer to  struct memo_data*
 *
 * @return      Return id (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_mod_data memo_del_data
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * md = memo_create_data();
 * md->color = 1;
 * ...
 * if (memo_add_data(md)  != -1) {
 *     return true;
 * } else {
 *     return false;
 * }
 * ...
 * \endcode
 */
int memo_add_data(struct memo_data *md);

/**
 *  This function modify the data: struct memo_data.
 *  This function is usually called together with memo_get_data();
 *
 * @brief      Update a data to DB
 *
 * @param     [in] md a pointer to  struct memo_data*
 *
 * @return      Return 0 (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_add_data memo_del_data
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * md = memo_get_data(id);
 * if (md == NULL) {
 *     return;
 * } else {
 *     md->id = 2;
 *     memo_mod_data(md);
 *     memo_free_data(md);
 * }
 * ...
 * \endcode
 */
int memo_mod_data(struct memo_data *md);

/**
 *  This function delete the data assosiated with id.
 *
 * @brief      Delete a data to DB
 *
 * @param     [in]    id        the id of the memo record
 *
 * @return      Return 0 (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_add_data memo_mod_data
 *
 * \par Sample code:
 * \code
 * ...
 * if (memo_del_data(id)  != -1) {
 *     return true;
 * } else {
 *     return false;
 * }
 * ...
 * \endcode
 */
int memo_del_data(int id);

/**
 *  This function gets the data list of the memo assosiated.
 *
 *
 * @brief      Get all memo list
 *
 * @return     This function returns  a pointer of memo_data_list on  success or NULL on failure.
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_free_data_list
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data_list *l = NULL;
 * l = memo_get_all_data_list();
 * while (l != NULL) {
 *     l = l->next;
 * }
 * memo_free_data_list(l);
 * ...
 * \endcode
 */
struct memo_data_list* memo_get_all_data_list(void);

/**
 *  This function free the data list of struct memo_data_list
 *  This function must be called when data list get by memo_get_all_data_list is not being needed
 *
 * @brief      Free data list
 *
 * @param     [in] mdl pointer of memo_data_list
 *
 * @return     None
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_get_all_data_list
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data_list *l = NULL;
 * l = memo_get_all_data_list();
 * while (l != NULL) {
 *     l = l->next;
 * }
 * memo_free_data_list(l);
 * ...
 * \endcode
 */
void memo_free_data_list(struct memo_data_list *mdl);

/**
 *  This function gets the modify time of a record associated with id.
 *
 *
 * @brief      Get modify time stamp
 *
 * @param     [in]    id        the id of the memo record
 *
 * @return     This function returns  the latest modify time of a record or -1 if failed.
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_free_data_list
 *
 * \par Sample code:
 * \code
 * ...
 * time_t *tm = -1;
 * tm = memo_get_modified_time(id);
 * if (tm == -1) {
 *     return;
 * } else {
 *     ...
 * }
 * ...
 * \endcode
 */
time_t memo_get_modified_time(int id);

/**
 *  This function gets the records number in db.
 *
 *
 * @brief      Get number of records in db
 *
 * @param     [out]    count        the count of the memo records
 *
 * @return     Return 0 (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see
 *
 * \par Sample code:
 * \code
 * ...
 * int count = 0;
 * if (memo_get_count(&count) == -1) {
 *     return;
 * } else {
 *     ...
 * }
 * ...
 * \endcode
 */
int memo_get_count(int *count);

/**
 *  This function gets operation list of all updated record after specified time stamp.
 *
 *
 * @brief      Get operation list
 *
 * @param     [in]    stamp    time stamp
 *
 * @return     This function returns  a pointer of memo_operation_list on  success or NULL on failure.
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_free_operation_list
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * time_t tm = time((time_t *)0);
 * memo_operation_list l = NULL;
 * md = memo_create_data();
 * md->color = 1;
 * ...
 * if (memo_add_data(md)  != -1) {
 *     l = memo_get_operation_list(tm);
 *     if (l != NULL) {
 *         memo_free_operation_list();
 *     }
 * } else {
 *     return false;
 * }
 * ...
 * \endcode
 */
struct memo_operation_list* memo_get_operation_list(time_t stamp);

/**
 *  This function free the data list of struct memo_operation_list
 *  This function must be called when data list get by memo_get_operation_list is not being needed
 *
 * @brief      Free operation list
 *
 * @param     [in] mol pointer of memo_operation_list
 *
 * @return     None
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_get_operation_list
 *
 * \par Sample code:
 * \code
 * ...
 * memo_data *md = NULL;
 * time_t tm = time((time_t *)0);
 * memo_operation_list l = NULL;
 * md = memo_create_data();
 * md->color = 1;
 * ...
 * if (memo_add_data(md)  != -1) {
 *     l = memo_get_operation_list(tm);
 *     if (l != NULL) {
 *         memo_free_operation_list();
 *     }
 * } else {
 *     return false;
 * }
 * ...
 * \endcode
 */
void memo_free_operation_list(struct memo_operation_list *mol);

/**
 *  This function register callback funtion whenever memo record is update.
 *
 *
 * @brief      register callback
 *
 * @param     [in]    cb    callback function
 *
 * @param     [in]    user_data    The data to be passed to cb call.
 *
 * @return     Return 0 (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_unsubscribe_change
 *
 * \par Sample code:
 * \code
 * ...
 * \endcode
 */
int memo_subscribe_change(void (*cb)(void *), void *user_data);

/**
 *  This function unregister callback funtion registered by memo_subscribe_change.
 *
 *
 * @brief      unregister callback
 *
 * @param     [in]    cb    callback function to be unregistered.
 *
 * @return     Return 0 (Success) or -1 (Failed)
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_subscribe_change
 *
 * \par Sample code:
 * \code
 * ...
 * \endcode
 */
int memo_unsubscribe_change(void (*cb)(void *));

/**
 *  This function is used to listen to the update of memo record.
 *
 *
 * @brief      accompanied with memo_end_trans, all update of memo record(update/add/delete) will trigger change callback
 *                registered by memo_subscribe_change
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_end_trans
 *
 * \par Sample code:
 * \code
 * ...
 * memo_begin_trans();
 * memo_add_data(md); // trigger callback
 * memo_end_trans();
 * memo_add_data(md); // not trigger callback
 * \endcode
 */
void memo_begin_trans(void);

/**
 *  This function is used to end listen to the update of memo record and trigger callback.
 *
 *
 * @brief
 *
 * @remarks     None
 *
 * @exception   None
 *
 * @see memo_begin_trans
 *
 * \par Sample code:
 * \code
 * ...
 * memo_begin_trans();
 * memo_add_data(md); // trigger callback
 * memo_end_trans();
 * memo_add_data(md); // not trigger callback
 * \endcode
 */
void memo_end_trans(void);

typedef void (*memo_data_iterate_cb_t) (memo_data_t *md, void *user_data);

int memo_search_data(const char *search_str, int limit, int offset, MEMO_SORT_TYPE sort,
    memo_data_iterate_cb_t cb, void *user_data);

int memo_all_data(memo_data_iterate_cb_t cb, void *user_data);

/* Ugh, the following APIs are under testing and provides no guarantee.
  * If you want to use, please contact with canjiang.lu@samsung.com.
  */
int memo_get_indexes(int *aIndex, int len, MEMO_SORT_TYPE sort);

#ifdef __cplusplus
}
#endif

#endif /* __MEMO_DB_H__ */

