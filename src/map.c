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

#include "object.h"
#include "map.h"
#include "hash.h"
#include "string.h"

static struct bucket {
	CFWObject *key, *obj;
	uint32_t hash;
} deleted = { NULL, NULL, 0 };

struct CFWMap {
	CFWObject obj;
	struct bucket **data;
	uint32_t size;
	size_t items;
};

static bool
ctor(void *ptr, va_list args)
{
	CFWMap *map = ptr;
	void *key;

	map->data = NULL;
	map->size = 0;
	map->items = 0;

	while ((key = va_arg(args, void*)) != NULL)
		if (!cfw_map_set(map, key, va_arg(args, void*)))
			return false;

	return true;
}

static void
dtor(void *ptr)
{
	CFWMap *map = ptr;
	uint32_t i;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			cfw_unref(map->data[i]->key);
			cfw_unref(map->data[i]->obj);
			free(map->data[i]);
		}
	}

	if (map->data != NULL)
		free(map->data);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFWObject *obj2 = ptr2;
	CFWMap *map1, *map2;
	uint32_t i;

	if (obj2->cls != cfw_map)
		return false;

	map1 = ptr1;
	map2 = ptr2;

	if (map1->items != map2->items)
		return false;

	for (i = 0; i < map1->size; i++)
		if (map1->data[i] != NULL && map1->data[i] != &deleted &&
		    !cfw_equal(cfw_map_get(map2, map1->data[i]->key),
		    map1->data[i]->obj))
			return false;

	return true;
}

static uint32_t
hash(void *ptr)
{
	CFWMap *map = ptr;
	uint32_t i, hash = 0;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			hash += map->data[i]->hash;
			hash += cfw_hash(map->data[i]->obj);
		}
	}

	return hash;
}

static void*
copy(void *ptr)
{
	CFWMap *map = ptr;
	CFWMap *new;
	uint32_t i;

	if ((new = cfw_new(cfw_map, (void*)NULL)) == NULL)
		return NULL;

	if ((new->data = malloc(sizeof(*new->data) * map->size)) == NULL)
		return NULL;
	new->size = map->size;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			struct bucket *bucket;

			if ((bucket = malloc(sizeof(*bucket))) == NULL)
				return NULL;

			bucket->key = cfw_ref(map->data[i]->key);
			bucket->obj = cfw_ref(map->data[i]->obj);
			bucket->hash = map->data[i]->hash;

			new->data[i] = bucket;
		} else
			new->data[i] = NULL;
	}

	return new;
}

bool
resize(CFWMap *map, uint32_t items)
{
	size_t fullness = items * 4 / map->size;
	struct bucket **ndata;
	uint32_t i, nsize;

	if (items > UINT32_MAX)
		return false;

	if (fullness >= 3)
		nsize = map->size << 1;
	else if (fullness <= 1)
		nsize = map->size >> 1;
	else
		return true;

	if (nsize == 0)
		return false;

	if ((ndata = malloc(nsize * sizeof(*ndata))) == NULL)
		return false;

	for (i = 0; i < nsize; i++)
		ndata[i] = NULL;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			uint32_t j, last;

			last = nsize;

			j = map->data[i]->hash & (nsize - 1);
			for (; j < last && ndata[j] != NULL; j++);

			/* In case the last bucket is already used */
			if (j >= last) {
				last = map->data[i]->hash & (nsize - 1);

				for (j = 0; j < last && ndata[j] != NULL; j++);
			}

			if (j >= last) {
				free(ndata);
				return false;
			}

			ndata[j] = map->data[i];
		}
	}

	free(map->data);
	map->data = ndata;
	map->size = nsize;

	return true;
}

size_t
cfw_map_size(CFWMap *map)
{
	return map->items;
}

void*
cfw_map_get(CFWMap *map, void *key)
{
	uint32_t i, hash, last;

	if (key == NULL)
		return NULL;

	hash = cfw_hash(key);
	last = map->size;

	for (i = hash & (map->size - 1);
	    i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (cfw_equal(map->data[i]->key, key))
			return map->data[i]->obj;
	}

	if (i < last)
		return NULL;

	/* In case the last bucket is already used */
	last = hash & (map->size - 1);

	for (i = 0; i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (cfw_equal(map->data[i]->key, key))
			return map->data[i]->obj;
	}

	return NULL;
}

void*
cfw_map_get_c(CFWMap *map, const char *key)
{
	CFWString *str;
	void *ret;

	if ((str = cfw_new(cfw_string, key)) == NULL)
		return NULL;

	ret = cfw_map_get(map, str);

	cfw_unref(str);

	return ret;
}

bool
cfw_map_set(CFWMap *map, void *key, void *obj)
{
	uint32_t i, hash, last;

	if (key == NULL)
		return false;

	if (map->data == NULL) {
		if ((map->data = malloc(sizeof(*map->data))) == NULL)
			return false;

		map->data[0] = NULL;
		map->size = 1;
		map->items = 0;
	}

	hash = cfw_hash(key);
	last = map->size;

	for (i = hash & (map->size - 1);
	    i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (cfw_equal(map->data[i]->key, key))
			break;
	}

	/* In case the last bucket is already used */
	if (i >= last) {
		last = hash & (map->size - 1);

		for (i = 0; i < last && map->data[i] != NULL; i++) {
			if (map->data[i] == &deleted)
				continue;

			if (cfw_equal(map->data[i]->key, key))
				break;
		}
	}

	/* Key not in dictionary */
	if (i >= last || map->data[i] == NULL || map->data[i] == &deleted ||
	    !cfw_equal(map->data[i]->key, key)) {
		struct bucket *bucket;

		if (obj == NULL)
			return true;

		if (!resize(map, map->items + 1))
			return false;

		last = map->size;

		for (i = hash & (map->size - 1); i < last &&
		    map->data[i] != NULL && map->data[i] != &deleted; i++);

		/* In case the last bucket is already used */
		if (i >= last) {
			last = hash & (map->size - 1);

			for (i = 0; i < last && map->data[i] != NULL &&
			    map->data[i] != &deleted; i++);
		}

		if (i >= last)
			return false;

		if ((bucket = malloc(sizeof(*bucket))) == NULL)
			return false;

		if ((bucket->key = cfw_copy(key)) == NULL) {
			free(bucket);
			return false;
		}

		bucket->obj = cfw_ref(obj);
		bucket->hash = cfw_hash(key);

		map->data[i] = bucket;
		map->items++;

		return true;
	}

	if (obj != NULL) {
		void *old = map->data[i]->obj;
		map->data[i]->obj = cfw_ref(obj);
		cfw_unref(old);
	} else {
		cfw_unref(map->data[i]->key);
		cfw_unref(map->data[i]->obj);

		free(map->data[i]);
		map->data[i] = &deleted;

		map->items--;

		if (!resize(map, map->items))
			return false;
	}

	return true;
}

bool
cfw_map_set_c(CFWMap *map, const char *key, void *obj)
{
	CFWString *str;
	bool ret;

	if ((str = cfw_new(cfw_string, key)) == NULL)
		return false;

	ret = cfw_map_set(map, str, obj);

	cfw_unref(str);

	return ret;
}

void
cfw_map_iter(CFWMap *map, cfw_map_iter_t *iter)
{
	iter->_map = map;
	iter->_pos = 0;

	cfw_map_iter_next(iter);
}

void
cfw_map_iter_next(cfw_map_iter_t *iter)
{
	CFWMap *map = iter->_map;

	for (; iter->_pos < map->size &&
	    (map->data[iter->_pos] == NULL ||
	    map->data[iter->_pos] == &deleted); iter->_pos++);

	if (iter->_pos < map->size) {
		iter->key = map->data[iter->_pos]->key;
		iter->obj = map->data[iter->_pos]->obj;
		iter->_pos++;
	} else {
		iter->key = NULL;
		iter->obj = NULL;
	}
}

static CFWClass class = {
	.name = "CFWMap",
	.size = sizeof(CFWMap),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.hash = hash,
	.copy = copy
};
CFWClass *cfw_map = &class;
