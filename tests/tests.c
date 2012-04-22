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
#include "array.h"
#include "map.h"

static void
print_map(CFWMap *map)
{
	cfw_map_iter_t iter;

	cfw_map_iter(map, &iter);
	cfw_map_iter_next(&iter);

	fputs("{\n", stdout);

	while (iter.key != NULL) {
		if (iter.obj != NULL)
			printf("\t%s = %s\n", cfw_string_c(iter.key),
			    cfw_string_c(iter.obj));
		else
			printf("\t%s = NULL\n", cfw_string_c(iter.key));

		cfw_map_iter_next(&iter);
	}

	fputs("}\n", stdout);
}

int
main()
{
	CFWRefPool *p;
	CFWArray *a;
	CFWString *s, *s2;
	CFWMap *m;
	size_t i;

	p = cfw_new(cfw_refpool);

	a = cfw_new_p(cfw_array,
	    cfw_new_p(cfw_string, "Hallo"),
	    cfw_new_p(cfw_string, " Welt"),
	    cfw_new_p(cfw_string, "!"), NULL);

	s = cfw_new(cfw_string, NULL);

	for (i = 0; i < cfw_array_size(a); i++)
		cfw_string_append(s, cfw_array_get(a, i));

	cfw_unref(p);

	puts(cfw_string_c(s));

	p = cfw_new(cfw_refpool);
	s2 = cfw_new_p(cfw_string, "ll");
	printf("%zd\n", cfw_string_find(s, s2, cfw_range_all));

	cfw_unref(p);
	cfw_unref(s);

	p = cfw_new(cfw_refpool);

	m = cfw_new_p(cfw_map,
	    cfw_new_p(cfw_string, "Hallo"),
	    cfw_new_p(cfw_string, "Welt!"),
	    cfw_new_p(cfw_string, "Test"),
	    cfw_new_p(cfw_string, "success!"), NULL);

	print_map(m);

	cfw_map_set(m,
	    cfw_new_p(cfw_string, "Hallo"),
	    cfw_new_p(cfw_string, "Test"));

	print_map(m);

	cfw_map_set(m, cfw_new_p(cfw_string, "Hallo"), NULL);
	print_map(m);

	cfw_unref(p);

	return 0;
}
