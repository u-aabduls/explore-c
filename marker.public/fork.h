extern int was_alarm;
extern void alrm_handler(int i);
extern void f_error(char *s);
extern pid_t start_child(const char *path, char *const argv[],
			 int fdin, int fdout, int fderr);
