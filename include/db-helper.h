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

