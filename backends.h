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

/* sendmailornot backend API
 * A backend has his own c file
 * plus a conditional entrie in backend.h and in build system
 *
 * A backend must define those prototypes :
 *
 * void CREATE_DB(const char *dbpath)
 * This function create the database if it does not exists.
 * It makes the progarm failed if a problem append
 *
 * int GET_SCORE(const char *dbpath, const char *login)
 * This function returns the score for user "login"
 * It makes the progarm failed if the user does not exist
 *
 * void SET_SCORE(const char *dbpath, const char *login, int score)
 * This function set the value "mailcount" for user "login"
 * It makes the progarm failed if a problem append
 * If the user has no entry in database it will add it
 *
 * void ZERO_SCORES(const char *dbpath)
 * zero score for all users (or remove them from the database)
 * blacklisted users (score == BLACKLIST) are exclude
 *
 * void DUMP_DB(const char *dbpath)
 * print in STDOUT all databases entries 
 * format is "%s\t%d\n", login, score 
 *
 * int STATUS(const char *dbpath, int maxspam)
 * return < 0 if current user score > maxspam
 * else increase current user score with one more mail
 */


#ifdef BACKEND_BDB
#ifndef DBPATH
#define DBPATH	"/var/db/sendmailornot/spammer.db"
#endif /* DBPATH */
#define CREATE_DB	bdb_create_database
#define GET_SCORE	bdb_get_score
#define SET_SCORE	bdb_set_score
#define ZERO_SCORES	bdb_zero_scores
#define	DUMP_DB		bdb_dump_users_score
#define	STATUS		bdb_is_a_spammer
#else /* ! _BACKEND_BDB_ */
#error "No backend defined"
#endif /* BACKEND_BDB */


