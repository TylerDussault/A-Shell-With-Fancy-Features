
#include "../include/env.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char* replace_env(char* s)
{
int tester = 0;
int l = strlen(s);
char* nS = malloc(sizeof(char)*l+1); //nS will have the new string
strcpy(nS,s);
int start = -1;
int end = -1;	//start and end of a env var,
for (int i = 0; i < l; i++)
	{
	if (nS[i] == '$')
		start = i;
	if (start != -1 && ((i == l-1) || isspace(nS[i+1]))) //if we have started a env variable, and we have hit a whitespace, or we are at the end of the string, then this must be the end of the env variable
		{
		end = i+1;
		//make a cstring thats just the env variable
		char* env = malloc(sizeof(char) *(end-start)); //enough space for just the var part
		strncpy(env, nS+start+1,end-start); //copy just the path part
		env[end-start-1] = '\0'; //null terminate
		for (int i = 0; i < strlen(env); i++)
			env[i] = toupper(env[i]); //make it all uppercase, as user is not an env var, USER is, but the shell still recognizes it
		
		if (getenv(env) != NULL) //IF ENV IS A VALID ENV VAR
			{
			char* ncpy = malloc(sizeof(char)* (l+1));
			strcpy(ncpy,nS); //copy of nS so we can have the ending of the statement
			char* temp;
			//add eveything before the $
			temp = nS; //pointing at the original
			nS = malloc(sizeof(char)* (start) +1 ); //enough space for everything before the $
			strncpy(nS,temp,start+1); //copy everything
			nS[start] = '\0'; //null terminate
			free(temp);
			//add the env var value
			temp = nS; //hold the original
			nS = malloc(sizeof(char) * (start+1 + strlen(getenv(env))) ); //enough space for everything before the $ + the env var value + null char
			strcpy(nS, temp); //ns is now what it is before, but with enough space for the env var value
			strcat(nS, getenv(env)); //add that env var value
			free(temp);
			//add the rest of the statement, if there is anything left
			if (strlen(nS) != (strlen(ncpy) - (end-start) + strlen(getenv(env))) ) //if there is more to add
				{
				temp = nS;
				nS = malloc(sizeof(char)* ( (l+1) - (end-start) + strlen(getenv(env)))); //enough space for everything
				strcpy(nS,temp); //copy, but now with more space
				strcat(nS, ncpy+end); //add the ending onto that
				free(temp);
				}
			//free stuff
			free(env);
			free(ncpy);
			//reset values so this can loop
			i = end;
			start = end = -1;
			l = strlen(nS);
			}
		else	//env is NOT valid
			{
			free(env);
			free(nS);
			return NULL;
			}
		
		}
	}
return nS;
}
