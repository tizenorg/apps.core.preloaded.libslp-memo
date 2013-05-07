/*
*
* Copyright 2012  Samsung Electronics Co., Ltd
*
* Licensed under the Flora License, Version 1.1 (the "License");
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

#ifndef __MEMO_DB_HELPER_H__
#define __MEMO_DB_HELPER_H__

#define MEMO_DB_MAX_CONTENT_LEN 1500

#define KEY_ID_NAME  "id"

enum key_public_t
{
    KEY_INPUT_END = -1,

    /* common */
    KEY_ITEM_MODE,  /* 0-text, 1-doodle */
    KEY_FAVORITE,
    KEY_COLOR,

    /* about text */
    KEY_CONTENT,
    KEY_FONT_RESPECT,
    KEY_FONT_SIZE,
    KEY_FONT_COLOR,

    /* about doodle */
    KEY_COMMENT,
    KEY_DOODLE_PATH,

    END_KEY_PUBLIC,
};

enum key_private_t
{
    KEY_CREATE_TIME = END_KEY_PUBLIC,
    KEY_MODI_TIME,
    KEY_DELETE_TIME,
    KEY_WRITTEN_TIME,

    END_KEY_PRIVATE,
    TOTAL_NUM_OF_KEYS = END_KEY_PRIVATE,
};

struct column_t
{
    char *name;
    char *type;
};

char *db_content_truncate(char *content);

char *db_make_insert_query(int key1, void *val1, ...);
char *db_make_update_query(int id, int key1, void *val1, ...);
char *db_make_delete_query(int id);
char *db_make_string_constant(const char *input);

#endif /* __MEMO_DB_HELPER_H__ */

