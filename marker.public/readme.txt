MARKER			        Program Manual			       MARKER

NAME
	marker -- pipes output of a program into stdin of second program

SYNOPSIS
	marker [prog1 [arg1 ...] -p- prog2 [arg1 ...]]

DESCRIPTION
	The marker program connects the output of prog1 to the input of prog2 with
	the use of a pipe.

	The program reads from a file called test.in and uses its data as arguments
	for prog1. Any errors in executing prog1 are written to a file
	called test.err1.

	After piping the output of prog1 to prog2, prog2 will execute and write
	to a file called test.out. Any errors in executing prog2 are written to a
	file called test.err2.

	If the processes created by prog1 or prog2, or both, take longer
	than 3 seconds, all running processes are killed and an error message
	is written to the stderr of the main program.

	The arguments to marker are as follows:

		prog1		program that has I/O in a shell
		prog2		program that has I/O in a shell
		[arg1 ...]	acceptable string of arguments to the invoked program
		-p-		string representing a pipe

EXIT STATUS
	prog1 and prog2 exit and print to the stdout with a message reporting
	the status they exited with.

	If either process created by prog1 and prog2 exit with an error status,
	marker prints to the stdout with a message indicating that at least one of
	the processes did not finish.

	Any other errors that occur are printed to stdout.

EXAMPLE
	The command:

		./marker ls courseDir -p- wc -c

	will start program ls with argument courseDir and pipe its output to
	program wc with argument -c.

	The command:

		./marker cat f1 f2 f3 -p- grep findme

	will start program cat with arguments f1, f2, f3 and pipe its output
	to program grep with argument findme.

BUGS
	None currently known to the author.

AUTHOR
	Umar Abdulselam. Email: uaabduls@my.yorku.ca
