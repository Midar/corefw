/*
 * Copyright (c) 2012, Jonathan Schleifer <js@webkeks.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __COREFW_STRING_H__
#define __COREFW_STRING_H__

#include "class.h"
#include "range.h"

typedef struct CFWString CFWString;
extern CFWClass *cfw_string;
extern size_t cfw_strnlen(const char*, size_t);
extern char* cfw_strdup(const char*);
extern char* cfw_strndup(const char*, size_t);
extern char* cfw_string_c(CFWString*);
extern size_t cfw_string_length(CFWString*);
extern bool cfw_string_set(CFWString*, const char*);
extern void cfw_string_set_nocopy(CFWString*, char*, size_t);
extern bool cfw_string_append(CFWString*, CFWString*);
extern bool cfw_string_append_c(CFWString*, const char*);
extern bool cfw_string_has_prefix(CFWString*, CFWString*);
extern bool cfw_string_has_prefix_c(CFWString*, const char*);
extern bool cfw_string_has_suffix(CFWString*, CFWString*);
extern bool cfw_string_has_suffix_c(CFWString*, const char*);
extern size_t cfw_string_find(CFWString*, CFWString*, cfw_range_t);
extern size_t cfw_string_find_c(CFWString*, const char*, cfw_range_t);

#endif
