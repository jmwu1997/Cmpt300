// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>

#define COMMAND_LENGTH 1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define MAX_DIR 5000
#define HISTORY_LENGTH 10000

/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */


int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	if (length < 0) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
}

// insert history
char historylist[HISTORY_LENGTH][COMMAND_LENGTH];
int num=HISTORY_LENGTH;
int cmdnum=0;
void get_history( char *tokens[]){
	
	char string[COMMAND_LENGTH];
	strcpy(string,tokens[0]);
	for (int i = 0; (tokens[i]!=NULL);i++){
		if (tokens[i+1] == NULL){
		}
		else {
			strcat(string," ");
			strcat(string,tokens[i+1]);
		}

	}
	strcpy(historylist[num],string);
	
}	
	

// print history
void print_history(){
	int count=0;
	int cmdcount=cmdnum;
	char cmd[10];
	for(int x=0;x<=HISTORY_LENGTH;x++){
		  		if (strlen(historylist[x])>0){
					if (count<10){
					sprintf(cmd,"%d\t",cmdcount);
					write (STDOUT_FILENO, cmd,strlen(cmd));
					write (STDOUT_FILENO, historylist[x],strlen(historylist[x]));
					write (STDOUT_FILENO, "\n",strlen("\n"));
					}
					count++;
					cmdcount--;
					//printf("%d \n",count);
			}
	}
	
}



// }

/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().

        //char buf[MAX_DIR];
        //char *cwd = getcwd(buf, MAX_DIR);
    char cwd[256];
    char *location = getcwd(cwd,sizeof(cwd));

    //copy of location
    char cwdcopy[100];
    strcpy(cwdcopy,cwd);


		write(STDOUT_FILENO, strcat(location,"$ "), strlen( strcat(location,"$ ")));
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);
		

    if (tokens[0]!=NULL){
            
		

        //exit
           if (strcmp(tokens[0],"exit")==0){
                exit(0);
            
           }

        //!!
         if (strcmp(tokens[0],"!!")==0){

           	if (cmdnum==0){
               write(STDIN_FILENO, "There are no previous command", strlen("There are no previous command"));
               write(STDIN_FILENO, "\n", strlen("\n"));
               continue;
            }
            else {
               //write(STDIN_FILENO, "hit", strlen("hit"));
               //write(STDIN_FILENO, "\n", strlen("\n"));
               tokenize_command(historylist[num+1], tokens);
               write(STDOUT_FILENO, historylist[num+1], strlen(historylist[num+1]) );
               write(STDIN_FILENO, "\n", strlen("\n"));
               

			      }
        }
        
       

        //!N
        if (tokens[0][0]=='!'){
          int line = atoi(&tokens[0][1]);
          int newline = (HISTORY_LENGTH-line+1);
          
          if (line < 1|| line > cmdnum){
             write(STDIN_FILENO, "Command not found", strlen("Command not found"));
             write(STDIN_FILENO, "\n", strlen("\n"));
             continue;
           }
           else{
            tokenize_command(historylist[newline], tokens);
            write(STDIN_FILENO, historylist[newline], strlen(historylist[newline])) ;
            write(STDIN_FILENO, "\n", strlen("\n"));
           }
           
        }

        // add to history when !! !n are not called
        if ((strcmp(tokens[0], "!!")!= 0)&&(strcmp(tokens[0], "!n")!=0)){
                       get_history(tokens);
                       cmdnum++;
                       num--;
                       
                    
        }

        //pwd
         if (strcmp(tokens[0],"pwd")==0){
            //char *getcwd(char *buf, size_t size);
             char buf[MAX_DIR];
             char *cwd = getcwd(buf, MAX_DIR);
            //char *cwd = getcwd(buf, sizeof(cwd));
            if (cwd==NULL || tokens[1]!=NULL){
                write(STDOUT_FILENO,"Fail to display working directory.",strlen("Fail to display working directory."));    
                write(STDOUT_FILENO, "\n", strlen("\n"));

            }
            else{
                write(STDOUT_FILENO,cwd,strlen(cwd));
                write(STDOUT_FILENO, "\n", strlen("\n"));
            }
            continue;
        }
        //cd
        if (strcmp(tokens[0],"cd")==0){
            
          
          if (tokens[1]==NULL||strcmp(tokens[1],"~")==0){
              struct passwd* pw = getpwuid(getuid());

              if ((chdir(pw->pw_dir) == -1)||(!pw)){
                write(STDOUT_FILENO, "fail to locate home directory", strlen("fail to locate home directory"));
                return -1;
              }

          }
          else if (chdir(tokens[1])==-1){
                write(STDOUT_FILENO, "directory does not exist.", strlen("directory does not exist."));
                write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          continue;
            
        
	      } 
      //help
      if (strcmp(tokens[0],"help")==0){
          char *output;
          char *exitmsg = " is a builtin command for exiting the shell program.";
          char *pwdmsg = " is a builtin command for displaying the current working directory.";
          char *cdmsg = " is a builtin command for changing the current working directory.";
          char *helpmsg = " is a builtin command for displaying help information on internal command.";
          char *historymsg = " is a builtin command for displaying the 10 most recently entered commands.";
          if (tokens[2]!=NULL){
            output = "cannot input more than one argument.";
                write(STDOUT_FILENO, output, strlen(output));
                write(STDOUT_FILENO, "\n", strlen("\n"));
            }
          else if (tokens[1]==NULL){ 
              write(STDOUT_FILENO, "exit", strlen("exit"));
              write(STDOUT_FILENO, exitmsg, strlen(exitmsg));
              write(STDOUT_FILENO, "\n", strlen("\n"));
              write(STDOUT_FILENO, "pwd", strlen("pwd"));
              write(STDOUT_FILENO, pwdmsg, strlen(pwdmsg));
              write(STDOUT_FILENO, "\n", strlen("\n"));
              write(STDOUT_FILENO, "cd", strlen("cd"));
              write(STDOUT_FILENO, cdmsg, strlen(cdmsg));
              write(STDOUT_FILENO, "\n", strlen("\n"));
              write(STDOUT_FILENO, "help", strlen("help"));
              write(STDOUT_FILENO, helpmsg, strlen(helpmsg));
              write(STDOUT_FILENO, "\n", strlen("\n"));
              write(STDOUT_FILENO, "history", strlen("history"));
              write(STDOUT_FILENO, historymsg, strlen(historymsg));
              write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else if (strcmp(tokens[1],"exit")==0){
            output = strcat(tokens[1],exitmsg);
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else if (strcmp(tokens[1],"pwd")==0){
            output = strcat(tokens[1],pwdmsg);
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else if (strcmp(tokens[1],"cd")==0){
            output = strcat(tokens[1],cdmsg);
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else if (strcmp(tokens[1],"help")==0){
            output = strcat(tokens[1],helpmsg);
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else if (strcmp(tokens[1],"history")==0){
            output = strcat(tokens[1],historymsg);
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          else{
            output = strcat(tokens[1]," is a external command or application.");
            write(STDOUT_FILENO, output, strlen(output));
            write(STDOUT_FILENO, "\n", strlen("\n"));
          }
          continue;
      }

      // history
         if (strcmp(tokens[0],"history")==0){
			     print_history();
          continue;
			   }
    }
		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */        

    pid_t pid = fork();
    int status;
    if (pid < 0){
        fprintf(stderr,"fork Failed");
        exit(-1);
        }
        else if (pid == 0){
        //execvp(tokens[0],tokens);
        if(execvp(tokens[0],tokens) == -1){
            write(STDOUT_FILENO, "Wrong Command!", strlen("Wrong Command!"));
            write(STDOUT_FILENO, "\n", strlen("\n"));
            exit(-1);
            }
        }
        else if(!in_background){
             waitpid(pid,&status,WUNTRACED);
        }
    
        

		//DEBUG: Dump out arguments:
		// for (int i = 0; tokens[i] != NULL; i++) {
		// 	write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
		// 	write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
		// 	write(STDOUT_FILENO, "\n", strlen("\n"));
		// }
	if (in_background) {
			write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));   
    }
    while(waitpid(-1,NULL,WNOHANG)>0);
        
        
	}
	return 0;
}
