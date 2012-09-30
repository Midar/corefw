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

#ifndef __COREFW_STREAM_H__
#define __COREFW_STREAM_H__

#include <unistd.h>

#include "class.h"
#include "object.h"
#include "string.h"

struct cfw_stream_ops {
	ssize_t (*read)(void*, void*, size_t);
	bool (*write)(void*, const void*, size_t);
	bool (*at_end)(void*);
	void (*close)(void*);
};

typedef struct CFWStream {
	CFWObject obj;
	struct cfw_stream_ops *ops;
	char *cache;
	size_t cache_len;
} CFWStream;

extern CFWClass *cfw_stream;
extern ssize_t cfw_stream_read(void*, void*, size_t);
extern CFWString* cfw_stream_read_line(void*);
extern bool cfw_stream_write(void*, const void*, size_t);
extern bool cfw_stream_write_string(void*, const char*);
extern bool cfw_stream_write_line(void*, const char*);
extern bool cfw_stream_at_end(void*);
extern void cfw_stream_close(void*);
#endif
