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
#include <string.h>

#include "cfwobject.h"
#include "cfwstring.h"

struct CFWString {
	CFWObject obj;
	char *cstr;
	size_t len;
};

static bool
ctor(void *ptr, va_list args)
{
	CFWString *str = ptr;
	const char *cstr = va_arg(args, const char*);

	if (cstr != NULL) {
		if ((str->cstr = strdup(cstr)) == NULL)
			return false;

		str->len = strlen(cstr);
	} else {
		str->cstr = NULL;
		str->len = 0;
	}

	return true;
}

static void
dtor(void *ptr)
{
	CFWString *str = ptr;

	if (str->cstr != NULL)
		free(str->cstr);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFWObject *obj2 = ptr2;
	CFWString *str1, *str2;

	if (obj2->clsptr != cfw_string)
		return false;

	str1 = ptr1;
	str2 = ptr2;

	if (str1->len != str2->len)
		return false;

	return !strcmp(str1->cstr, str2->cstr);
}

static void*
copy(void *ptr)
{
	CFWString *str = ptr;
	CFWString *new;

	if ((new = cfw_new(cfw_string)) == NULL)
		return NULL;

	if ((new->cstr = malloc(str->len + 1)) == NULL) {
		cfw_unref(new);
		return NULL;
	}
	new->len = str->len;

	memcpy(new->cstr, str->cstr, str->len + 1);

	return new;
}

const char*
cfw_string_c(CFWString *string)
{
	return string->cstr;
}

size_t
cfw_string_len(CFWString *string)
{
	return string->len;
}

bool
cfw_string_set(CFWString *str, const char *cstr)
{
	char *copy;

	if ((copy = strdup(cstr)) == NULL)
		return false;

	if (str->cstr != NULL)
		free(str->cstr);

	str->cstr = copy;
	str->len = strlen(copy);

	return true;
}

cfw_unichar
cfw_string_char(CFWString *str, size_t index)
{
	if(len > index)
		return str->cstr[index];
	return NULL;
}

size_t
cfw_string_find(CFWString *strA, CFWString *strB, cfw_range_t range)
{
	char *cstrA = strA->cstr+range.location;
	size_t i, max = MIN(range.length+range.location, strB->len);
	
	if(strA->len == 0)
		return 0;
	
	if(strA->len-range.location < strB->len)
		return SIZE_MAX;
	
	for(i = range.location;
		i <= strA->len-strB->len && i <= max;
		i++)
	{
		if(!memcmp(strA->cstr+i, strB->cstr, max-i))
			return i;
	}
	
	return SIZE_MAX;
}

static CFWClass class = {
	.name = "CFWString",
	.size = sizeof(CFWString),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.copy = copy
};
CFWClass *cfw_string = &class;
