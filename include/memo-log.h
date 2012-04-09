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

#ifndef __MEMO_LOG_H__
#define __MEMO_LOG_H__

#include <dlog.h>
#include <stdio.h>

#ifdef LOG_TAG
    #undef LOG_TAG
#endif
#define LOG_TAG "libslp-memo"

#define INFO(fmt, arg...) LOGI(fmt, ##arg)
#define ERR(fmt, arg...) LOGE(fmt, ##arg)
#define DBG(fmt, arg...) LOGD(fmt, ##arg)

#define warn_if(expr, fmt, arg...) do { \
    if(expr) { \
        ERR("(%s) -> "fmt, #expr, ##arg); \
    } \
} while (0)
#define ret_if(expr) do { \
    if(expr) { \
        ERR("(%s) -> %s() return", #expr, __FUNCTION__); \
        return; \
    } \
} while (0)
#define retv_if(expr, val) do { \
    if(expr) { \
        ERR("(%s) -> %s() return", #expr, __FUNCTION__); \
        return (val); \
    } \
} while (0)
#define retm_if(expr, fmt, arg...) do { \
    if(expr) { \
        ERR(fmt, ##arg); \
        ERR("(%s) -> %s() return", #expr, __FUNCTION__); \
        return; \
    } \
} while (0)
#define retvm_if(expr, val, fmt, arg...) do { \
    if(expr) { \
        ERR(fmt, ##arg); \
        ERR("(%s) -> %s() return", #expr, __FUNCTION__); \
        return (val); \
    } \
} while (0)

#endif /* __MEMO_LOG_H__ */
