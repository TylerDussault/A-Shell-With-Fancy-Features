#pragma once

#include "lexer.h"

char *find_path(char *command); //finds the path of the command passed through the argument by searhcing through all the directories in $PATH, returns the path to it, returns null if it cant find it, 
				//must free return eventually
