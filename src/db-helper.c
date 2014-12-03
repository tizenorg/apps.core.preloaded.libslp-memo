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
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "db-helper.h"
#include "memo-db.h"

#define SINGLE_QUOTO '\''
#define sncat(to, size, from) \
    strncat(to, from, size-strlen(to)-1)

struct key_value_t
{
    char *key;
    char *value;
};

struct column_t columns[] = {
    {"doodle",          "%d"},  /*  0 - KEY_ITEM_MODE */
    {"favorite",        "%d"},  /*  1 - KEY_FAVORITE */
    {"color",           "%d"},  /*  2 - KEY_COLOR */
    {"content",         "%s"},  /*  3 - KEY_CONTENT */
    {"font_respect",    "%d"},  /*  4 - KEY_FONT_RESPECT */
    {"font_size",       "%d"},  /*  5 - KEY_FONT_SIZE */
    {"font_color",      "%d"},  /*  6 - KEY_FONT_COLOR */
    {"comment",         "%s"},  /*  7 - KEY_COMMENT */
    {"doodle_path",     "%s"},  /*  8 - KEY_DOODLE_PATH */
    {"create_time",     "%d"},  /*  9 - KEY_CREATE_TIME */
    {"modi_time",       "%d"},  /* 10 - KEY_MODI_TIME */
    {"delete_time",     "%d"},  /* 11 - KEY_DELETE_TIME */
    {"written_time",    "%s"},  /* 12 - KEY_WRITTEN_TIME */
};

void string_append_printf(char *str, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char *buf = str + strlen(str);
    vsprintf(buf, fmt, args);
    va_end(args);
}

/*
 * @decription
 *   Limit the maximum content length.
 *
 * @param[in/out]   content
 * @return          the original string pointer
 */
char *db_content_truncate(char *content)
{
    if (content == NULL) { return NULL; }

    if (strlen(content) > MEMO_DB_MAX_CONTENT_LEN)
    {
        content[MEMO_DB_MAX_CONTENT_LEN] = '\0';
    }
    return content;
}

char *db_parse_value(int key, void *val)
{
    char *type = columns[key].type;

    /* judge the val's type is string */
    bool is_string = (type[strlen(type) - 1] == 's' ? true : false);

    if (is_string) {
        return db_make_string_constant((const char *)val);
    } else {
        int len = 32;
        char *buf = (char *)malloc(sizeof(char) * len);
        if (buf == NULL) {
            return NULL;
        }
        snprintf(buf, len, type, val);
        return buf;
    }
}

void db_key_value_array_append(struct key_value_t kv[], int total, int key1, void *val1)
{
    int i = total;
    kv[i].key = columns[key1].name;
    kv[i].value = db_parse_value(key1, val1);
}

int db_parse_key_value(struct key_value_t kv[], int key1, void *val1, va_list args)
{
    int keyn = -1;
    void *valn = NULL;
    int count = 0;

    db_key_value_array_append(kv, count++, key1, val1);

    while(true)
    {
        keyn = va_arg(args, int);
        valn = va_arg(args, void*);

        if(keyn == KEY_INPUT_END) { break; }
        if(valn == NULL) { continue; }

        db_key_value_array_append(kv, count++, keyn, valn);
    }

    return count;
}


char *db_make_insert_query(int key1, void *val1, ...)
{
    struct key_value_t kv[TOTAL_NUM_OF_KEYS];
    int count = 0;
    int i = 0;

    va_list args;
    va_start(args, val1);
    count = db_parse_key_value(kv ,key1, val1, args);
    va_end(args);

    db_key_value_array_append(kv, count++, KEY_CREATE_TIME, (void *)time(NULL));
    db_key_value_array_append(kv, count++, KEY_MODI_TIME, (void *)time(NULL));
    db_key_value_array_append(kv, count++, KEY_DELETE_TIME, (void *)-1);

    /* BEGIN Generate SQL Query */
    /* INSERT INTO memo (key1, key2, ...) VALUES (val1, val2, ...) */
    int len = 256; /* buffer length */
    for (i = 0; i < count; ++i)
    {
        len += strlen(kv[i].key);
        len += strlen(kv[i].value);
    }

    char *buf = (char *)malloc(len);
    if (buf == NULL) {
        for (i = 0; i < count; ++i) /* clean up */
        {
            free(kv[i].value);
        }
        return NULL;
    }

    strncpy(buf, "INSERT INTO memo (", len);

    for (i = 0; i < count; ++i)
    {
        sncat(buf, len, kv[i].key);
        sncat(buf, len, i == count - 1 ? ") VALUES (" : ", ");
    }

    for (i = 0; i < count; ++i)
    {
        sncat(buf, len, kv[i].value);
        sncat(buf, len, i == count - 1 ? ")" : ", ");
    }
    /* END Generate SQL Query */

    for (i = 0; i < count; ++i) /* clean up */
    {
        free(kv[i].value);
    }

    return buf;
}

char *db_make_update_query(int id, int key1, void *val1, ...)
{
    struct key_value_t kv[TOTAL_NUM_OF_KEYS];
    int count = 0;
    int i = 0;

    va_list args;
    va_start(args, val1);
    count = db_parse_key_value(kv ,key1, val1, args);
    va_end(args);

    db_key_value_array_append(kv, count++, KEY_MODI_TIME, (void *)time(NULL));

    /* BEGIN Generate SQL Query */
    /* UPDATE memo SET key1 = val1, key2 = val2, ... WHERE id = %d */
    int len = 256; /* buffer length */
    for (i = 0; i < count; ++i)
    {
        len += strlen(kv[i].key);
        len += strlen(kv[i].value);
    }

    char *buf = (char *)malloc(len);
    if (buf == NULL) {
        for (i = 0; i < count; ++i) /* clean up */
        {
            free(kv[i].value);
        }
        return NULL;
    }

    strncpy(buf, "UPDATE memo SET ", len);

    for (i = 0; i < count; ++i)
    {
        string_append_printf(buf, "%s = %s", kv[i].key, kv[i].value);
        sncat(buf, len, i == count - 1 ? " " : ", ");
    }

    string_append_printf(buf, "WHERE %s = %d", KEY_ID_NAME, id);
    /* END Generate SQL Query */

    for (i = 0; i < count; ++i) /* clean up */
    {
        free(kv[i].value);
    }

    return buf;
}

char *db_make_delete_query(int id)
{
    return db_make_update_query(id,
        KEY_DELETE_TIME, (void *)time(NULL),
        KEY_INPUT_END);
}

static int _string_constant_len(const char *input)
{
    if (input == NULL) {
        return 0;
    }

    int i = 0;
    int len = strlen(input);
    int size = len + 2; /* add enclosing quotes */

    for (i=0; i<len; i++) {
        if (input[i] == SINGLE_QUOTO) {
            size++; /* extra quote */
        }
    }
    return size;
}

/**
 * db_make_string_constant
 *
 * @brief Make string constant from input by rules of sqlite
 *
 * @param   [in] input The original string
 *
 * @return   Pointer to string constant or NULL if failed.  The return value should be freed by caller.
 *
 * @exception    None
 *
 * @remark       A string constant is formed by enclosing the string in single quotes (').
 * A single quote within the string can be encoded by putting two single quotes in a row - as in Pascal.
 * Digest from documentation of sqlite.
 *
 * @see
 *
 */
char *db_make_string_constant(const char *input)
{
    if (input == NULL) {
        return NULL;
    }

    int size = _string_constant_len(input);
    char *p = (char *)calloc(1, size+1);
    if (p == NULL) {
        return NULL;
    }

    int i = 0;
    int j = 0;
    int len = strlen(input);
    p[j++] = SINGLE_QUOTO;
    for (i=0; i<len; i++) {
        p[j++] = input[i];
        if (input[i] == SINGLE_QUOTO) {
            p[j++] = SINGLE_QUOTO;
        }
    }
    p[j++] = SINGLE_QUOTO;
    return p;
}

