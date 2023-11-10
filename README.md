# Shell
This code basicly reconstruct the work ofthe shell.

### The meaning of the existence of each function:
1) f_new_process - to create a new process, if we encounter parentheses (i.e. if we encounter '(' )
2) f_kill_process - to destroy an open process-parentheses (i.e. if they met ')' )
3) f_add_to_stack - adding a command to the stack
4) f_fd_read - to understand that we are dealing with a read descriptor
5) f_fd_write - to understand that we are dealing with a record descriptor
6) f_file - to indicate that we have a file
7) f_check_and - to indicate that the first part of the logical operation has ended &&
9) f_check_or - to indicate that the first part of the logical operation has ended ||
10) f_l - for command < 
11) f_r - for a command >
12) f_rr - for a command >>
13) f_pipe - for a command |

Different refinements that we do to make the shell work correctly:
1. Redirects to the file can only be made at the end of the pipeline
2. File names should not contain spaces
3. Additionally implemented were the commands || and &&
4. For 1 launch of the program we have 1 line of commands
5. Parenthesis was not implemented


