#include "../include/pathsearch.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
char* find_path(char* command) //finds the path of the command passed through the argument by searhcing through all the directories in $PATH, returns the path to it, returns null if it cant find it
{
bool found = false; //have we found it yet?
char* path; //the path to return
tokenlist* dirs = get_tokens(getenv("PATH"), ":"); //get a list of directories but spliting them up into tokens from the PATH env var using a colon as the delimeter

for (int i = 0; i < dirs->size; i++) //for each directory token list, look through all of them
{
	DIR* dir;
	if ((dir = opendir(dirs->items[i])) != NULL) //open the directory
	{
		struct dirent* file;					
		while ((file = readdir(dir)) != NULL) //for each file in the directory
		{
			if (file != NULL)
			{
				if (strcmp(file->d_name, command) == 0) //if we found it
				{
				found = true;
				path = malloc(sizeof(char)* (strlen(dirs->items[i])+1));
				strcpy(path,dirs->items[i]); //the path to the file
				}
			}
		
		}
	}
closedir(dir);
}
if (found)
	{
	char* temp = malloc(sizeof(char)*(strlen(path) +2)); //to add the slash and the null terminator
	strcpy(temp,path);
	strncpy(temp+strlen(temp), "/", 2);//adds those 2 bytes
	free(path);
	path = malloc(sizeof(char)*(strlen(temp) + strlen(command) +1));	//now enough spacefor everything
	strcpy(path, temp); //path is the directory command is in
	strcat(path,command); //add the command
	free(temp);
	free_tokens(dirs);
	return path;
	}
else
	{
	free_tokens(dirs);
	return NULL;
	}
}



