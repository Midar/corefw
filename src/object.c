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
#include <assert.h>

#include "object.h"
#include "refpool.h"

void*
cfw_new(CFWClass *class, ...)
{
	CFWObject *obj;

	if ((obj = malloc(class->size)) == NULL)
		return NULL;

	obj->cls = class;
	obj->ref_cnt = 1;

	if (class->ctor != NULL) {
		va_list args;
		va_start(args, class);

		if (!class->ctor(obj, args)) {
			cfw_unref(obj);
			return NULL;
		}

		va_end(args);
	}

	return obj;
}

void*
cfw_create(CFWClass *class, ...)
{
	CFWObject *obj;

	assert(class != cfw_refpool);

	if ((obj = malloc(class->size)) == NULL)
		return NULL;

	obj->cls = class;
	obj->ref_cnt = 1;

	if (class->ctor != NULL) {
		va_list args;
		va_start(args, class);

		if (!class->ctor(obj, args)) {
			cfw_unref(obj);
			return NULL;
		}

		va_end(args);
	}

	if (!cfw_refpool_add(obj)) {
		cfw_unref(obj);
		return NULL;
	}

	return obj;
}

void*
cfw_ref(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return NULL;

	obj->ref_cnt++;

	return obj;
}

void
cfw_unref(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return;

	if (--obj->ref_cnt == 0)
		cfw_free(obj);
}

void
cfw_free(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return;

	if (obj->cls->dtor != NULL)
		obj->cls->dtor(obj);

	free(obj);
}

CFWClass*
cfw_class(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return NULL;

	return obj->cls;
}

bool
cfw_is(void *ptr, CFWClass *cls)
{
	CFWObject *obj = ptr;

	if (obj == NULL || cls == NULL)
		return false;

	return (obj->cls == cls);
}

bool
cfw_equal(void *ptr1, void *ptr2)
{
	CFWObject *obj1 = ptr1, *obj2 = ptr2;

	if (obj1 == obj2)
		return true;

	if (obj1 == NULL || obj2 == NULL)
		return false;

	if (obj1->cls->equal != NULL) {
		return obj1->cls->equal(obj1, obj2);
	} else
		return (obj1 == obj2);
}

uint32_t
cfw_hash(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return 0;

	if (obj->cls->hash != NULL)
		return obj->cls->hash(obj);

	return (uint32_t)(uintptr_t)ptr;
}

void*
cfw_copy(void *ptr)
{
	CFWObject *obj = ptr;

	if (obj == NULL)
		return NULL;

	if (obj->cls->copy != NULL)
		return obj->cls->copy(obj);

	return NULL;
}

static CFWClass class = {
	.name = "CFWObject",
	.size = sizeof(CFWObject),
};
CFWClass *cfw_object = &class;
