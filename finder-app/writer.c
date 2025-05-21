#include <stdio.h>
#include <syslog.h>
#include <string.h>

int main(int argc, char *argv[])
{
	/* ensure that all logs are printed in the console */
	openlog(NULL,LOG_PERROR,LOG_USER);
	if (argc != 3)
	{
		syslog(LOG_ERR,"usage : writer <filename> <string to write>");
		closelog();
		return 1;
	}

	//syslog(LOG_DEBUG,"argc=%d - argv[0]=%s argv[1]=%s argv[2]=%s\r\n",argc,argv[0],argv[1],argv[2]);

	/* argv[1] is the filename to be created, and argv[2] is to string to write */
	FILE *file = fopen(argv[1],"w");
	if (file == NULL)
	{
		syslog(LOG_ERR,"Could not create file %s - give-up",argv[1]);
		closelog();
		return 1;
	}	
	/* write the string in the file - normally, should be done in one pass, but just in case ... */
	syslog(LOG_DEBUG,"Writing %s to file %s",argv[2],argv[1]);

	size_t nbBytesWritten = 0;
	size_t nbBytesToWrite = strlen(argv[2]);
	char *p = argv[2];
	while (nbBytesToWrite != 0)
	{
		size_t n = fwrite(p,1,nbBytesToWrite,file);
		/* should never happen that we could not write in file ... */
		if (n == 0)
		{
			syslog(LOG_ERR,"Error while writing file %s after %d bytes",argv[1],(int)nbBytesWritten);
			fclose(file);
			return 1;
		}
		/* just to be sure ...*/
		if (n > nbBytesToWrite)
		{
			syslog(LOG_ERR,"more bytes written than the number of bytes to write !!!");	
			n = nbBytesToWrite;
		}
		nbBytesToWrite -= n;
		nbBytesWritten += n;
	}
	/* we are none */
	fclose(file);
	return 0;
}
