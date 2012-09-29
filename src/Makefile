SHARED_LIB = ${LIB_PREFIX}corefw${LIB_SUFFIX}
LIB_MAJOR = 0
LIB_MINOR = 0

SRCS = array.c		\
       bool.c		\
       box.c		\
       class.c		\
       double.c		\
       file.c		\
       int.c		\
       map.c		\
       object.c		\
       range.c		\
       refpool.c	\
       stream.c		\
       string.c		\
       tcpsocket.c

INCLUDES = ${SRCS:.c=.h}	\
	   corefw.h		\
	   hash.h

include ../buildsys.mk
