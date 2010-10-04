PROG=		sendmailornot
SRCS=		main.c
WITHOUT_MAN=	yes

#for now only one backend define
WITH_BDB=	yes

.ifdef WITH_SENDMAIL
CFLAGS+= -DSENDMAIL=$(WITH_SENDMAIL)
.else
#Little FreeBSD optimisation
#As /usr/sbin/sendmail is a symlink for mailwrapper
#use /usr/libexec/sendmail/sendmail
CFLAGS+= -DSENDMAIL=\"/usr/libexec/sendmail/sendmail\"
.endif

.ifdef WITH_MAXSPAM
CFLAGS+= -DMAXSPAM=$(MAXSPAM)
.endif

.ifdef WITH_BLACKLIST
CFLAGS+= -DBLACKLIST=$(MAXSPAM)
.endif

.ifdef WITH_DBPATH
CFLAGS+= DBPATH=$(WITH_DBPATH)
.endif

.ifdef DEBUG
CFLAGS+=	-g -Wall 
.endif

.ifdef WITH_BDB
CFLAGS+= 	-DBACKEND_BDB -I/usr/local/include
LDFLAGS=	-L/usr/local/lib/db48 -ldb
SRCS+=		bdb.c
.endif


.include "bsd.prog.mk"
