/*******************************************************************
 * EECS3221: Operating Systems, Summer 2019
 * Assignment 1: Processes, Signals and Interprocess Communication.
 * Student Name: Umar Abdulselam
 * Student EECS account: uaabduls
 * Student ID: 215995616
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include "fork.h"

#define READ 0
#define WRITE 1
#define ERR 2

int was_alarm = 0;

/* The main program does much of the work. parses the command line arguments */
/* sets up the alarm and the alarm signal handler opens the files and pipes */
/* for redirection etc., invoke start_child, closes files that should be  */
/* closed, waits for the children to finish and reports their status */
/* or exits printing a message and kill its children if they do not die */
/* in time (just the parent exiting may not be enough to kill the children) */

/* Method returns index of pipe if found in cmd line args, returns -1 if not
   found. If pipe is found in cmd line args, it is replaced with NULL. */
int find_pipe(int num_args, char *prse_args[]){
  int i = num_args-1, k = -1;
  while (i > 0) {
    if (strcmp(prse_args[i], "-p-") == 0){
      k = i;
      prse_args[k] = NULL;
    }
    i--;
  }
  return k;
}

int main(int argc, char *argv[])
{
  /* Error checking the given command line arguments */
  if (argc < 3){
    f_error("Error: not enough arguments. Needed at least 3.");
  }
  int pipe_loc = find_pipe(argc, argv); // gets pipe's index & sets it to NULL
  if (pipe_loc == -1){
    f_error("Error: no pipe found.");
  }

  /* Creation of pipe to be used by the two programs */
  int fd[2];
  int pipe_status = pipe(fd);
  if (pipe_status < 0){
    f_error("Error: failed to create pipe.");
  }

  /* Creating and opening the necessary files to read and write to, and also
     getting their file descriptors */
  int testin = open("test.in", O_RDONLY);
  int testout = open("test.out", O_CREAT | O_WRONLY | O_TRUNC, 0777);
  int testerr1 = open("test.err1", O_CREAT | O_WRONLY | O_TRUNC, 0777);
  int testerr2 = open("test.err2", O_CREAT | O_WRONLY | O_TRUNC, 0777);

  /* Error checking for creation and opening the  necessary files */
  if (testin < 0){
    f_error("Error: failed to open test.in file.");
  }
  if (testout < 0){
    f_error("Error: failed to open test.out file.");
  }
  if (testerr1 < 0){
    f_error("Error: failed to open test.err1 file.");
  }
  if (testerr2 < 0){
    f_error("Error: failed to open test.err2 file.");
  }

  /* Creating child process for first program. The process reads from data
     in test.in file, writes output into the read end of the pipe, and prints
     its errors to file test.err1. */
  pid_t pid1 = start_child(argv[1], argv+1, testin, fd[1], testerr1);

  /* Creating child process for second program. The process reads from write
     end of the pipe, writes output into test.out, and prints its errors to
     file test.err2. */
  int k = pipe_loc + 1; // location of second program name in argv
  pid_t pid2 = start_child(argv[k], argv+k, fd[0], testout, testerr2);

  /* Waiting for first program to finish executing and then reporting its
     exit status */
  int pid1_sts;
  waitpid(pid1, &pid1_sts, 0);
  if (WIFEXITED(pid1_sts)){
    int exit_sts1 = WEXITSTATUS(pid1_sts);
		printf("Process %s finished with status %i\n", argv[1], exit_sts1);
  }

  /* closing the write end of the pipe - i.e. the input stream */
  close(fd[WRITE]);

  /* Waiting for second program to finish executing and then reporting its
     exit status */
  int pid2_sts;
  waitpid(pid2, &pid2_sts, 0);
  if (WIFEXITED(pid2_sts)){
    int exit_sts2 = WEXITSTATUS(pid2_sts);
		printf("Process %s finished with status %i\n", argv[k], exit_sts2);
  }

  // while waiting, if the child process takes longer than 3 seconds,
  // we send a signal to wake it up and kill it.

  // struct sigaction action;
  // action.sa_handler = alrm_handler;
  // if (sigemptyset(&action.sa_mask) < 0){
  //   f_error("sigemptyset");
  // }
  // action.sa_flags = 0;

  return 0;
}
