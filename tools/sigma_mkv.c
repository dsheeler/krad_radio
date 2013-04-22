#include <stdio.h>
#include <unistd.h>

#include <krad_mkv_demux.h>

void do_print(char *string)
{
	printf("%s", string);
}

int main(int argc, char *argv[])
{
	do_print("BEFORE SYSTEM INIT\n");

	// Init krad_system
	krad_system_init();

	// Logging function must contain the ABSOLUTE PATH.
	// Relative paths will work, but will screw up printing
	// to the console.
	krad_system_log_on("/home/sigma/testing.log");

	kr_mkv_t *mkv = kr_mkv_open_file(argv[1]);

	if(!mkv)
	{
		perror("krad_mkv_open_file");
	}

	do_print("AFTER SYSTEM INIT\n");
	return 0;
}
