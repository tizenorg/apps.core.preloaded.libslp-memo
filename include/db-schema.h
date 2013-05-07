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

#ifndef __MEMO_SCHEMA_H__
#define __MEMO_SCHEMA_H__

#define CREATE_MEMO_TABLE " \
create table if not exists memo ( \
id INTEGER PRIMARY KEY autoincrement, \
content TEXT, \
written_time TEXT, \
create_time INTEGER, \
modi_time INTEGER, \
delete_time INTEGER, \
doodle INTEGER, \
color INTEGER, \
comment TEXT, \
favorite INTEGER, \
font_respect INTEGER, \
font_size INTEGER, \
font_color INTEGER, \
doodle_path TEXT \
)"

#endif /* __MEMO_SCHEMA_H__ */
