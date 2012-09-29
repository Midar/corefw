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

#include <string.h>

#include "stream.h"

static bool
ctor(void *ptr, va_list args)
{
	CFWStream *stream = ptr;

	stream->ops = NULL;

	return true;
}

static void
dtor(void *ptr)
{
	cfw_stream_close(ptr);
}

ssize_t
cfw_stream_read(void *ptr, void *buf, size_t len)
{
	CFWStream *stream = ptr;
	ssize_t ret;

	if (stream == NULL || stream->ops == NULL)
		return -1;

	if ((ret = stream->ops->read(stream, buf, len)) < -1)
		ret = -1;

	return ret;
}

bool
cfw_stream_write(void *ptr, const void *buf, size_t len)
{
	CFWStream *stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return false;

	return stream->ops->write(stream, buf, len);
}

bool
cfw_stream_write_string(void *ptr, const char *str)
{
	return cfw_stream_write(ptr, str, strlen(str));
}

bool
cfw_stream_eof(void *ptr)
{
	CFWStream *stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return true;

	return stream->ops->eof(stream);
}

void
cfw_stream_close(void *ptr)
{
	CFWStream *stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return;

	stream->ops->close(stream);
}

static CFWClass class = {
	.name = "CFWStream",
	.size = sizeof(CFWStream),
	.ctor = ctor,
	.dtor = dtor
};
CFWClass *cfw_stream = &class;
