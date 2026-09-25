# Shell
OS shell with "fancy features" made in C by Matthew Chen, Kian Sweeney, and Tyler Dessault.

## Group Members
- **Matthew Chen**: mc23c@fsu.edu
- **Kian Sweeney**: 
- **Tyler Dussault**: tjd22@fsu.edu
## Division of Labor

### Part 1: Prompt
- **Responsibilities**: The user should be greeted by a prompt each time after a user
executes a command.
- **Assigned to**: Matthew Chen

### Part 2: Environment Variables
- **Responsibilities**: Your objective is to implement a mechanism that automatically expands tokens starting with the dollar sign into their respective values. For instance, given the command ["echo", "$USER"], the token "$USER" should be expanded to its corresponding value, resulting in the output ["echo", "mnguyen"]. This expansion applies universally to any command, ensuring consistent token replacement throughout the shell. To accomplish this, you can utilize the getenv() function, the usage details of which can be found in its man page.
- **Assigned to**: Kian Sweeney

### Part 3: Tilde Expansion
- **Responsibilities**: In Bash, tilde (~) may appear at the beginning of a path, and it expands to the environment variable $HOME. For example, tokens [“ls”, “~/dir1”] should expand to [“ls”, “/home/grads/mnguyen/dir1”]. An output example of tilde expansion is:
- ~ → /home/grads/mnguyen  
You will only have to handle the tilde expansion of a token if "~" a standalone or if it begins with "~/".
- **Assigned to**: Matthew Chen

### Part 4: $PATH Search
- **Responsibilities**: In Bash, when a command like "ls" is entered, the shell is able to execute the corresponding program/executable located at "/usr/bin/ls" through a process called path search. This path search is not magical, but rather a straightforward search conducted within a predefined list of directories. The list of directories is specified in the environment variable $PATH. For commands that do not include a slash (/) and are not built-in functions (covered in part 9), it becomes necessary to search each directory specified in $PATH. It's important to note that $PATH is a string containing multiple directories delimited by a colon. To perform the search, you will need to employ string operations to extract and examine each directory in the $PATH variable. If you type in the command echo $PATH in your terminal you should get something like this:
/home/grads/mnguyen/.bin:/home/grads/mnguyen/.scripts:/usr/local/bin:/opt/sfw/bin:/usr/sfw/bin:/bin:/usr/bin:/usr/ccs/bin:/usr/ucb:.  
In the event that the command is not found in any of the directories listed in $PATH, an error message should be displayed. In Bash, this typically results in the familiar "command not found" error message. Handling this scenario involves proper error detection and reporting, ensuring a clear indication when a command is not available within the directories specified in $PATH.
- **Assigned to**: Kian Sweeney

### Part 5: External Command Execution
- **Responsibilities**: Once you have obtained the path to the program you intend to execute, either because the command included a slash or through the $PATH search, the next step is to execute the external command. However, executing an external command requires more than just a single line of code using the execv() function.
To accomplish this, a two-step process is involved. First, you need to fork() to create a child process. The child process will be responsible for executing the desired command using the execv() function. This separation between the parent and child processes ensures that the execution of the command does not interfere with the operation of the shell itself.
It's important to note that you must handle commands with arguments correctly. This means that commands such as "ls -al" with multiple arguments should be properly processed and executed in the child process.
By following this approach of forking and executing the command within the child process, you can ensure the proper execution of external commands, including those with arguments, within your shell. This separation of processes allows for efficient and accurate command execution while maintaining the stability and functionality of the shell as a whole.
- **Assigned to**: Kian Sweeney

### Part 6: I/O Redirection
- **Responsibilities**: By default, the shell receives input from the keyboard and outputs to the screen. However, with I/O redirection, we can replace the keyboard with input from a specified file and redirect output to a designated file.
The behavior of I/O redirection should adhere to the following guidelines:
cmd > file_out
cmd writes its standard output to the file_out.
If file does not exist, it will be created.
If file already exists, it will be overwritten.
cmd < file_in
cmd receives its standard input from file_in.
An error will be signaled if file does not exist or is not a regular file.
cmd < file_in > file_out
cmd receives standard input from file_in
cmd writes its standard output to file_out
cmd > file_out < file_in
Same as above.
These combinations follow the aforementioned rules for input and output redirection. By implementing I/O redirection, we empower the shell to efficiently manage input and output streams, allowing users to redirect command output to files and read command input from files. This functionality greatly enhances the versatility and flexibility of the shell when interacting with external commands.
- **Assigned to**: Matthew Chen

### Part 7: Piping
- **Responsibilities**: Beyond simple I/O redirection, we will explore the concept of piping, a more sophisticated form of I/O manipulation. Contrary to regular I/O redirection, piping involves the simultaneous execution of multiple commands, with the input and output of these commands interconnected. This setup allows the output generated by the initial command to be seamlessly passed as input to the subsequent command.
In this project, we will handle a maximum of two pipes in any given command. Piping behavior should follow the following guidelines:
cmd1 | cmd2
cmd1 redirects its standard output to the standard input of cmd2.
cmd1 | cmd2 | cmd3
cmd1 redirects its standard output to the standard input of cmd2.
cmd2 redirects its standard output to the standard input of cmd3.
By implementing piping functionality, we enable the seamless flow of data between commands, enhancing the flexibility and power of the shell. Piping allows for the creation of command pipelines, where the output of one command becomes the input for the next. This feature promotes the construction of complex and efficient command sequences, enabling sophisticated data processing and manipulation within the shell environment.
- **Assigned to**: Tyler Dussault

### Part 8: Background Processing
- **Responsibilities**: Background processing behavior should adhere to the following guidelines:
cmd &
Execute cmd in the background.
Upon execution start, print [Job number] [cmd's PID].
Upon completion, print [Job number] + done [cmd's command line].
cmd1 | cmd2 &
Execute cmd1 | cmd2 in the background.
Upon execution start, print [Job number] [cmd2's PID].
Upon completion, print [Job number] + done [cmd1 | cmd2's command line].
Background processing also supports redirection functionalities:  
cmd > file &
cmd writes its standard output to file in the background.
cmd < file &
cmd receives its standard input from file in the background.
cmd < file_in > file_out &
cmd receives its standard input from file_in and writes its standard output to file_out in the background.
Additionally, all background processes executed by the shell must be kept track of with a relative job number starting from 1 and incrementing so forth. Job numbers will not be reused. You can also assume that there will not be more than 10 background processes running concurrently.
- **Assigned to**: Tyler Dussault

### Part 9: Internal Command Execution
- **Responsibilities**: Having completed external command execution, the next aspect to address is the implementation of internal commands, often referred to as built-in functions. These functions are natively supported by the shell and will be integrated into your implementation.
exit
If any background processes are still running, you must wait for them to finish.
You can assume that each command is less than 200 characters long.
Display the last three valid commands.
If there were less than three valid commands, print the last valid one.
If there are no valid commands, say so.
cd PATH
Changes the current working directory.
If no arguments are supplied, change the current working directory to $HOME.
Signal an error if more than one argument is present.
Signal an error if the target is not a directory.
Signal an error if the target does not exist.
jobs
Outputs a list of active background processes.
If there are no active background processes, say so.
Format:
[Job number]+ [CMD's PID] [CMD's command line]
- **Assigned to**: Tyler Dussault

### Extra Credit
- **Responsibilities**: [Description]
- **Assigned to**: Kian Sweeney, Matthew Chen, Tyler Dussault

## File Listing
```
shell/
│
├── src/
│ ├── main.c
│ └── shell.c
│
├── include/
│ └── shell.h
│
├── README.md
└── Makefile
```
## How to Compile & Execute

### Requirements
- **Compiler**: e.g., `gcc` for C/C++, `rustc` for Rust.
- **Dependencies**: List any libraries or frameworks necessary (rust only).

### Compilation
For a C/C++ example:
```bash
make
```
This will build the executable in ...
### Execution
```bash
make run
```
This will run the program ...

## Development Log
- Set up starter files and README (Matthew Chen)

### [Matthew Chen]

| Date       | Work Completed / Notes |
|------------|------------------------|
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |

### [Kian Sweeney]

| Date       | Work Completed / Notes |
|------------|------------------------|
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |
| YYYY-MM-DD | [Description of task]  |


### [Tyler Dessault]

| Date       | Work Completed / Notes |
|------------|------------------------|
| 2026-09-23 | Bug testing for parts 1-6  |
| 2026-09-25 | Completed parts 7, 8, and 9  |


## Meetings
Document in-person meetings, their purpose, and what was discussed.

| Date       | Attendees            | Topics Discussed | Outcomes / Decisions |
|------------|----------------------|------------------|-----------------------|
| 2026-9-10 | Matthew, Kian, Tyler | Division of labor, first implementations  | Starting the project, setting up the repository, division of labor  |
| 2026-09-17 | Matthew, Kian, Tyler | Updates, future plans, current implementation   | Other group members finish their parts |



## Bugs

## Extra Credit
All three extra credit opportunities were completed:
- Support unlimited number of pipes
- Support piping and I/O redirection in a single command
- Shell-ception: Execute your shell from within a running shell process repeatedly
