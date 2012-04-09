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
