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

#include <stdio.h>

#include "object.h"
#include "refpool.h"
#include "string.h"
#include "int.h"
#include "array.h"
#include "map.h"

static void
print_map(CFWMap *map)
{
	cfw_map_iter_t iter;

	cfw_map_iter(map, &iter);

	fputs("{\n", stdout);

	while (iter.key != NULL) {
		printf("\t%s = ", cfw_string_c(iter.key));

		if (cfw_is(iter.obj, cfw_string))
			printf("%s\n", cfw_string_c(iter.obj));
		else if (cfw_is(iter.obj, cfw_int))
			printf("%jd\n", cfw_int_value(iter.obj));

		cfw_map_iter_next(&iter);
	}

	fputs("}\n", stdout);
}

int
main()
{
	CFWRefPool *pool;
	CFWArray *array;
	CFWString *str, *str2;
	CFWMap *map;
	size_t i;

	pool = cfw_new(cfw_refpool);

	array = cfw_create(cfw_array,
	    cfw_create(cfw_string, "Hallo"),
	    cfw_create(cfw_string, " Welt"),
	    cfw_create(cfw_string, "!"), (void*)NULL);

	str = cfw_new(cfw_string, (void*)NULL);

	for (i = 0; i < cfw_array_size(array); i++)
		cfw_string_append(str, cfw_array_get(array, i));

	cfw_unref(pool);

	puts(cfw_string_c(str));

	pool = cfw_new(cfw_refpool);
	str2 = cfw_create(cfw_string, "ll");
	printf("%zd\n", cfw_string_find(str, str2, cfw_range_all));

	cfw_unref(pool);
	cfw_unref(str);

	pool = cfw_new(cfw_refpool);

	map = cfw_create(cfw_map,
	    cfw_create(cfw_string, "Hallo"),
	    cfw_create(cfw_string, "Welt!"),
	    cfw_create(cfw_string, "Test"),
	    cfw_create(cfw_string, "success!"),
	    cfw_create(cfw_string, "int"),
	    cfw_create(cfw_int, INTMAX_C(1234)), NULL);

	print_map(map);

	cfw_map_set(map,
	    cfw_create(cfw_string, "Hallo"),
	    cfw_create(cfw_string, "Test"));

	print_map(map);

	cfw_map_set(map, cfw_create(cfw_string, "Hallo"), NULL);
	print_map(map);

	cfw_unref(pool);

	return 0;
}
