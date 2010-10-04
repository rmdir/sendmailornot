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


#include <ctype.h>
#include <err.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>

#ifndef MAXSPAM
#define MAXSPAM 10
#endif

#ifndef BLACKLIST
#define BLACKLIST -1
#endif

#ifndef SENDMAIL
#define SENDMAIL "/usr/sbin/sendmail"
#endif

#include "backends.h"

extern void CREATE_DB(const char *dbpath);
extern int GET_SCORE(const char *dbpath, const char *login);
extern void SET_SCORE(const char *dbpath, const char *login, int score);
extern void ZERO_SCORES(const char *dbpath, int blacklist);
extern void DUMP_DB(const char *dbpath);
extern int STATUS(const char *dbpath, int maxspam);

inline void
usage(void) {
	errx(1, "usage : \n\t sendmailornot [-cZDF] [-m num] [-bdz login] "
	   "[-s /path/to/sendmail ] [-p /path/to/database.db]"
	   "\n\t-c delete an existing db and create a new one"
	   "\n\t-m specify the maximum number of mails an user can send"
	   " (default %d)"
	   "\n\t-F allow to pass a [-f address] sendmail extra command"
	   "\n\t-s specify the path to sendmail (default %s)"
	   "\n\t-p /path/to/database.db (default %s)"
	   "\n\t-b blacklist given user"
	   "\n\t-d dump the user's score"
	   "\n\t-D dump all users score"
	   "\n\t-z zero a single user score"
	   "\n\t-Z zero all users score", MAXSPAM, SENDMAIL, DBPATH);
}


int 
main(int argc, char *argv[]) 
{
	int c, fflag = 0, cflag = 0, bflag = 0, dflag = 0;
	int Dflag = 0, zflag = 0, Zflag = 0, res;
	size_t len;
	int maxspam = MAXSPAM;
	char *from = NULL, *sendmail = SENDMAIL, *command;
	char *dbpath = DBPATH, *user = NULL;
		while ((c = getopt(argc, argv, "d:b:cm:Fp:hf:s:z:DZ")) != -1)
	{
		switch(c)
		{
		case 'c':
			cflag = 1;
			break;
		case 'm':
			maxspam = atoi(optarg);
			break;
		case 'F':
			fflag = 1;
			break;
		case 'p':
			dbpath = optarg;
			break;
		case 'f':
			from = optarg;
			break;
		case 's':
			sendmail = optarg;
			break;
		case 'b':
			bflag = 1;
			user = optarg;
			break;
		case 'd':
			dflag = 1;
			user = optarg;
			break;
		case 'D':
			Dflag = 1;
			break;
		case 'z':
			zflag = 1;
			user = optarg;
			break;
		case 'Z':
		       	Zflag = 1;	
		       	break;
		default :
			usage();
		}
	}
	switch(cflag + bflag + dflag + Dflag + zflag + Zflag) 
	{
	case 0:
		/* send the mail (or not) */
		if(STATUS(dbpath, maxspam) < 0)
			errx(1,"Too many mails sent from your account");
		len = strlen(sendmail) +6;
		if(fflag > 1) {
			len += strlen(from) +4;
			command = (char *) malloc(sizeof(char) * len);
			if(command != NULL)
				snprintf(command, len, "%s -t -i -f %s", sendmail, from);
			else
				err(1,"Memory problem");
		} else { 
			command = (char *) malloc(sizeof(char) * len);
			if(command != NULL)
				snprintf(command, len, "%s -t -i", sendmail);
			else
				err(1,"Memory problem");
		}
	 	//res = system(command);
		res = 0;
		free(command);
		exit(res);
	case 1:
		/* command mode */
		if(cflag > 0) {
			CREATE_DB(dbpath);
			exit(0);
		}
		if(bflag > 0) {
			SET_SCORE(dbpath, user, BLACKLIST);
			warnx("%s balcklisted", user);
			exit(0);
		}
		if(zflag > 0) {
			SET_SCORE(dbpath, user, 0);
			warnx("%s zeroed", user);
			exit(0);
		}
		if(Zflag > 0) {
			ZERO_SCORES(dbpath, BLACKLIST);
			warnx("database zeroed");
			exit(0);
		}
		if(dflag > 0) {
			printf("score: %s\t%d\n", user, GET_SCORE(dbpath, user));
			exit(0);
		}
		if(Dflag > 0) {
			DUMP_DB(dbpath);
			exit(0);
		}
	default:
		warnx("Invalid command combinaison");
		usage();
	}
}







