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
