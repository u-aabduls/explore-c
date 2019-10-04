/*******************************************************************
 * EECS3221: Operating Systems, Summer 2019
 * Assignment 1: Processes, Signals and Interprocess Communication.
 * Student Name: Umar Abdulselam
 * Student EECS account: uaabduls
 * Student ID: 215995616
 *******************************************************************/

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "fork.h"

/* This is the handler of the alarm signal. It just updates was_alarm */
void alrm_handler(int i){
	was_alarm	= 1;
}

/* Prints string s using perror and exits. Also checks errno to make */
/* sure that it is not zero (if it is just prints s followed by newline */
/* using fprintf to the standard error */
void f_error(char *s){
	if (errno != 0){
		perror(s);
	}
	else {
		fprintf(stderr, "%s\n", s);
	}
	exit(1);
}

/* Creates a child process using fork and a function from the exec family */
/* The standard input output and error are replaced by the last three */
/* arguments to implement redirection and piping */
pid_t start_child(const char *path, char *const argv[],
		  int fdin, int fdout, int fderr)
{
	pid_t pid = fork();
	if (pid < 0){
		f_error("Error: failed to fork process.");
	}
	else if (pid == 0){
		if (fdin != 0){
			if (dup2(fdin, 0) < 0){ // make file descriptor 0 point to fdin
				f_error("Error: failed to redirect stdin.");
			}
		}
		if (fdout != 1){
			if (dup2(fdout, 1) < 0){
				f_error("Error: failed to redirect stdout.");
			}
		}
		if (fderr != 2){
			if (dup2(fderr, 2) < 0){
				f_error("Error: failed to redirect stderr.");
			}
		}
		/* close all uneeeded file descriptors, including what fdin, fdout, fderr
		   previously pointed to */
		int i;
		for (i = 3; i <= 9; i++) {
			close(i);
		}
		// execute the process
		execvp(path, argv);
	}
	else { // parent process
		return pid;
	}
	return pid;
}
