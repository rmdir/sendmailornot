PROG=		sendmailornot
SRCS=		main.c
WITHOUT_MAN=	yes

.ifdef DEBUG
CFLAGS+=	-g -Wall 
.endif

.ifdef WITH_BDB
CFLAGS+= 	-DBACKEND_BDB -I/usr/local/include
LDFLAGS=	-L/usr/local/lib/db48 -ldb
SRCS+=		bdb.c
.endif

.include "bsd.prog.mk"
