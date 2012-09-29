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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "object.h"
#include "string.h"
#include "hash.h"

struct CFWString {
	CFWObject obj;
	char *data;
	size_t len;
};

char*
cfw_strdup(const char *s)
{
	char *copy;
	size_t len;

	len = strlen(s);

	if ((copy = malloc(len + 1)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	memcpy(copy, s, len);
	copy[len] = 0;

	return copy;
}

char*
cfw_strndup(const char *s, size_t max)
{
	char *copy;
	size_t len;

	len = strlen(s);

	if (len > max)
		len = max;

	if ((copy = malloc(len + 1)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	memcpy(copy, s, len);
	copy[len] = 0;

	return copy;
}

static bool
ctor(void *ptr, va_list args)
{
	CFWString *str = ptr;
	const char *cstr = va_arg(args, const char*);

	if (cstr != NULL) {
		str->data = NULL;
		if ((str->data = cfw_strdup(cstr)) == NULL)
			return false;

		str->len = strlen(cstr);
	} else {
		str->data = NULL;
		str->len = 0;
	}

	return true;
}

static void
dtor(void *ptr)
{
	CFWString *str = ptr;

	if (str->data != NULL)
		free(str->data);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFWObject *obj2 = ptr2;
	CFWString *str1, *str2;

	if (obj2->cls != cfw_string)
		return false;

	str1 = ptr1;
	str2 = ptr2;

	if (str1->len != str2->len)
		return false;

	return !memcmp(str1->data, str2->data, str1->len);
}

static uint32_t
hash(void *ptr)
{
	CFWString *str = ptr;
	size_t i;
	uint32_t hash;

	CFW_HASH_INIT(hash);

	for (i = 0; i < str->len; i++)
		CFW_HASH_ADD(hash, str->data[i]);

	CFW_HASH_FINALIZE(hash);

	return hash;
}

static void*
copy(void *ptr)
{
	CFWString *str = ptr;
	CFWString *new;

	if ((new = cfw_new(cfw_string, NULL)) == NULL)
		return NULL;

	if ((new->data = malloc(str->len + 1)) == NULL) {
		cfw_unref(new);
		return NULL;
	}
	new->len = str->len;

	memcpy(new->data, str->data, str->len + 1);

	return new;
}

const char*
cfw_string_c(CFWString *str)
{
	return str->data;
}

size_t
cfw_string_length(CFWString *string)
{
	return string->len;
}

bool
cfw_string_set(CFWString *str, const char *cstr)
{
	char *copy;
	size_t len;

	if (str != NULL) {
		if ((copy = cfw_strdup(cstr)) == NULL)
			return false;

		len = strlen(copy);
	} else {
		copy = NULL;
		len = 0;
	}

	if (str->data != NULL)
		free(str->data);

	str->data = copy;
	str->len = len;

	return true;
}

void
cfw_string_set_nocopy(CFWString *str, char *cstr, size_t len)
{
	if (str->data != NULL)
		free(str->data);

	str->data = cstr;
	str->len = len;
}

bool
cfw_string_append(CFWString *str, CFWString *append)
{
	char *new;

	if ((new = realloc(str->data, str->len + append->len + 1)) == NULL)
		return false;

	memcpy(new + str->len, append->data, append->len);
	new[str->len + append->len] = 0;

	str->data = new;
	str->len += append->len;

	return true;
}

size_t
cfw_string_find(CFWString *str, CFWString *substr, cfw_range_t range)
{
	size_t i;

	if (range.start > str->len)
		return SIZE_MAX;

	if (range.length == SIZE_MAX)
		range.length = str->len - range.start;

	if (range.start + range.length > str->len || substr->len > range.length)
		return SIZE_MAX;

	for (i = range.start; i <= range.start + range.length - substr->len;
	    i++)
		if (!memcmp(str->data + i, substr->data, substr->len))
			return i;

	return SIZE_MAX;
}

static CFWClass class = {
	.name = "CFWString",
	.size = sizeof(CFWString),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.hash = hash,
	.copy = copy
};
CFWClass *cfw_string = &class;
