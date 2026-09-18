#include "../include/execute.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


bool execute(char* fname, char** args, int argc)
{
for (int i =0; i < argc; i++)
	{
	if (args[i] == NULL || args == NULL)
		printf("BAAAD\n");
	}
pid_t pid = fork(); //clone process
if (pid == -1) //coudlnt fork
	return false;
else if (pid == 0) //this is the child
	{
		if(execv(fname, args) == -1) //couldnt execute
			return false;
		
	}
else	//we are the parent
	waitpid(pid, NULL, 0); //wait for child to end, no settings
//child is done
return true;
}
