#include "systemcalls.h"
#include "unistd.h"
#include "syslog.h"
#include "stdio.h"
#include "fcntl.h"
#include "stdlib.h"
#include <sys/wait.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int r;
    // sanity check 
    openlog("MonProgramme", LOG_PID | LOG_PERROR, LOG_USER);
    
    if (cmd == NULL)
    {
        syslog(LOG_ERR,"could not execute NULL command");
        return false;
    }    
    // execute the command
     r = system(cmd);

     // if the command was executed successfully, its return code is 0
     if (r == 0)
     {
        syslog(LOG_USER,"command %s executed successfully",cmd);
        return true;
     }
     else
     {
        syslog(LOG_ERR,"command %s returned an error",cmd);
        return false;
     }
    
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    syslog(LOG_USER,"executing fork");
    pid_t pid = fork();
    
    // if could not fork, reutrn false
    if (pid == -1)
    {
        syslog(LOG_ERR,"Could not execute fork");
        return false;
    }
    // if we are in the child process, execute the command
    else if (pid == 0)
    {
        syslog(LOG_USER,"I am the child process : executing command %s",command[0]);
        execv(command[0],&command[0]);
        // in case of success, the execv command does not return. So, if we are here, there is an error ...
        // the child process should exit in error
        syslog(LOG_USER,"I am the child process :could not execute the command : give-up");
        exit(-1);
    }
    // if we are in the parent, wait that the child terminates
    else
    {
        int status;
        syslog(LOG_USER,"I am the parent process : waiting for termination of child process (pid=%d)",pid);
        
        // wait until the child has terminated
        pid_t terminatedPid = waitpid(pid,&status,0);

        // if waitpid failed to execute, give-up
        if (terminatedPid == -1)
        {
            syslog(LOG_ERR,"waitpid() returned an error - give-up");
            return false;
        }
        // if the child process has been terminated with an error code, retun false
        else if (status != 0)
        {
            syslog(LOG_ERR,"child process exited with error %d",status);
            return false;
        }
        // nominal situation : the child process has terminated successfully
        else
        {
            syslog(LOG_USER,"child process exited successfully");
            return true;
        }
    }


    va_end(args);

    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    if (outputfile == NULL)
    {
        syslog(LOG_ERR,"outputfile is NULL - give-up");
        return false;
        
    }
    // open output file
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) 
    {
        syslog(LOG_ERR,"could not open output file - five-up");
        return false;
    }


    syslog(LOG_USER,"executing fork");
    pid_t pid = fork();
    
    // if could not fork, reutrn false
    if (pid == -1)
    {
        syslog(LOG_ERR,"Could not execute fork");
        return false;
    }
    // if we are in the child process, execute the command
    else if (pid == 0)
    {
        syslog(LOG_USER,"I am the child process : redirecting standard output to file %s",outputfile);
        
        // @note : we use dup2(oldfd,newfd) with newfd=1 which is the file descriptor the the stndard output
        // => this will close the standard output, and assign file descriptor=1 (i.e. standard output) to the 
        //     outputfile
        if (dup2(fd, 1) < 0) 
        { 
            syslog(LOG_ERR,"dup2 returned an error : give-up"); 
            exit(-1); 
        }

        syslog(LOG_USER,"I am the child process : executing command %s",command[0]);
        execv(command[0],&command[0]);
        // in case of success, the execv command does not return. So, if we are here, there is an error ...
        // the child process should exit in error
        syslog(LOG_USER,"I am the child process :could not execute the command : give-up");
        close(fd);
        exit(-1);
    }
    // if we are in the parent, wait that the child terminates
    else
    {
        int status;
        syslog(LOG_USER,"I am the parent process : waiting for termination of child process (pid=%d)",pid);
        
        // wait until the child has terminated
        pid_t terminatedPid = waitpid(pid,&status,0);

        // close outputfile
        syslog(LOG_USER,"closing output file");
        close(fd);

        // if waitpid failed to execute, give-up
        if (terminatedPid == -1)
        {
            syslog(LOG_ERR,"waitpid() returned an error - give-up");
            return false;
        }
        // if the child process has been terminated with an error code, retun false
        else if (status != 0)
        {
            syslog(LOG_ERR,"child process exited with error %d",status);
            return false;
        }
        // nominal situation : the child process has terminated successfully
        else
        {
            syslog(LOG_USER,"child process exited successfully");
            return true;
        }
    }

    va_end(args);

    return true;
}
