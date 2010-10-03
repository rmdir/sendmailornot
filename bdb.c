/*
 * Copyright (c) 2010
 *	Joris Dedieu <joris.dedieu@gmail.com>.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <err.h>
#include <pwd.h>
#include <string.h>

#include <db48/db.h>
#include <sys/types.h>


DB *_bdb_open_database(const char *dbpath);
int _bdb_get_mail_count(const char *dbpath, uid_t uid);
int bdb_is_a_spammer(const char *dbpath, int maxspam);
void bdb_create_database(const char *dbpath);
void bdb_update_mail_count(const char *dbpath, uid_t uid, int mailcount);
int bdb_get_score(const char *dbpath, const char *login);
void bdb_dump_users_score(const char *dbpath);
void bdb_set_score(const char *dbpath, const char *login, int score);

DB 
*_bdb_open_database(const char *dbpath)
{
	DB *map;
	if(db_create(&map, NULL, 0) != 0) 
		err(1, "Fail on db_create");

	if(map->open(map, NULL, dbpath, NULL, DB_BTREE, 0, 0) != 0)
		err(1, "Fail on opening %s", dbpath);
	return map;
}

int
_bdb_get_mail_count(const char *dbpath, uid_t uid)
{
	DBT key, data;
	DB *map = _bdb_open_database(dbpath);
	int mailcount = 0;
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &uid;
	key.size = sizeof(uid_t); 
	data.data = &mailcount;
	data.ulen = sizeof(int);
	data.flags = DB_DBT_USERMEM;
	if(map->get(map, NULL, &key, &data, 0) != 0)
		return 0;	
	map->close(map, 0); 
	return mailcount;
}

void 
bdb_create_database(const char *dbpath)
{
	DB *map;
	if(db_create(&map, NULL, 0) != 0) 
		err(1, "Fail on db_create");
	warn("Current database (%s) will be erase\n", dbpath);
	//remove(dbpath);
	if(map->open(map, NULL, dbpath, NULL, DB_BTREE, DB_CREATE | DB_EXCL, 0) != 0)
		err(1, "Fail on database create");
	map->close(map, 0);
}

void
bdb_update_mail_count(const char *dbpath, uid_t uid, int mailcount)
{
	DBT key, data;
	DB *map = _bdb_open_database(dbpath);
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &uid;
	key.size = sizeof(uid_t);
	data.data = &mailcount;
	data.size = sizeof(int);
	if(map->put(map, NULL, &key, &data, 0) != 0)
		err(1,"PUT error");
	map->close(map, 0); 
}

int 
bdb_get_score(const char *dbpath, const char *login) 
{
	struct passwd *p;
	if((p = getpwnam(login)) == NULL)
		err(1, "Can't finds uid for %s", login);
	return _bdb_get_mail_count(dbpath, p->pw_uid);
}

void
bdb_dump_users_score(const char *dbpath) 
{
	struct passwd *p;
	uid_t uid;
	int mailcount;
	DBT key, data;
	DB *map = _bdb_open_database(dbpath);
	DBC *cursor;
	map->cursor(map, NULL, &cursor, 0);	
	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	key.data = &uid;
	key.size = sizeof(uid_t); 
	data.data = &mailcount;
	data.ulen = sizeof(int);
	data.flags = DB_DBT_USERMEM;
	while (cursor->get(cursor, &key, &data, DB_NEXT) == 0) {
		if((p = getpwuid(uid)) == NULL) {
			warn("Can't find user for uid : %d\n", uid);
			continue;
		}
		printf("%s\t%d\n", p->pw_name, mailcount);
	}
	cursor->close(cursor);
	map->close(map, 0); 
}

void
bdb_set_score(const char *dbpath, const char *login, int score)
{	
	struct passwd *p;
	if((p = getpwnam(login)) == NULL)
		err(1, "Can't finds uid for %s", login);
	bdb_update_mail_count(dbpath, p->pw_uid, score);
}

int
bdb_is_a_spammer(const char *dbpath, int maxspam)
{
	uid_t uid;
	int mailcount = 0;
	uid = getuid();
	mailcount = _bdb_get_mail_count(dbpath, uid);
	if(mailcount < 0)
		return -1;
	mailcount++;
	bdb_update_mail_count(dbpath, uid, mailcount);
	printf("%d,%d\n", uid, mailcount);
	if(mailcount > maxspam) 
		return -1;
	return 0;
}


