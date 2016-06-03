/*
 * main.c
 *
 *  Created on: 2012-5-4
 *      Author: amos
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <android/log.h>

#include "injectso.h"


void die(int ln,const char* file){
	char temp[256];
	snprintf(temp, sizeof(temp), "failed at %d in %s: %s\n", ln, file,strerror(errno));
	fprintf(stderr, temp);
	exit(-1);
}

int main(int argc, char **argv) {

	if(argc != 3)
		return 0;

	int pid = atoi(argv[1]);
	__android_log_print(ANDROID_LOG_VERBOSE, "injectso", "start inject_process");
	inject_process(pid, argv[2], "test", "Hook start !!", strlen("Hook start !!") );

	return 0;
}
