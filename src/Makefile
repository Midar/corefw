SHARED_LIB = ${LIB_PREFIX}corefw${LIB_SUFFIX}
STATIC_LIB = libcorefw.a
LIB_MAJOR = 0
LIB_MINOR = 0

SRCS = cfwarray.c	\
       cfwclass.c	\
       cfwobject.c	\
       cfwrange.c	\
       cfwstring.c

INCLUDES = ${SRCS:.c=.h} \
	   corefw.h

include ../buildsys.mk
