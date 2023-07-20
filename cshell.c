// Including all libraries I need to use 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// Making structure for the Log
typedef struct{
    char *name;
    struct tm time;
    int code;
}command;

static command **logCommand;
static int cmdsize = 0;


// Making structure for the Enviorment variable
typedef struct{
    char *name;
    char *value;
}EnvVar;

EnvVar **envVar;
int varsize = 0;

// Prototyping all functions

void exitFunction();
void addLogFunction(command *newcmd);
int printLogFunction();
void freeLogFunction();
int newEnviormentVar(EnvVar *newvar);
int updateEnVar(EnvVar *var);
int enviormentVarCheckFunc(char *name);
char *getVarFunction(char* name);
char *getCommandFunc(void);
void stringparser(char *parse, char **token);
int builtinCommandFunc(char *command);
int isEnvVar(char *cmd);
int printPromptFunc(char **parse);
int changeTheme(char *colour);
int valEnvVar(char* cmd);
int nonBuildCmd(char** parse);


// Main fucntion alos has the code for the script mode 
int main( int argc, char *argv[]) 
{
  time_t rtime;
  int sw;
  if(argc < 2) 
  {
    while (1) 
    {
      char *cmdline,*args[100];
      printf("cshell$ ");
      cmdline = getCommandFunc();
      command *cmd = malloc(sizeof (command));
      time(&rtime);
      cmd->time = *localtime(&rtime);
      stringparser(cmdline, args);
      cmd->name = strdup(args[0]);
      if ((sw = builtinCommandFunc(args[0])) > 0) 
      {
        switch (sw) 
        {
          case 1:
              exitFunction();
          case 2:
              cmd->code = printLogFunction();
              break;
          case 3:
              cmd->code = printPromptFunc(args);
              break;
          case 4:
              cmd->code = changeTheme(args[1]);
              break;
        }
      } 
      else if (isEnvVar(args[0]) && valEnvVar(args[0])) 
      {
        EnvVar *var = malloc(sizeof(EnvVar));
        var->name = strdup(strtok(args[0], "="));
        var->value = strdup(strtok(NULL, "="));
        if(enviormentVarCheckFunc(var->name)==0)
        {
            cmd->code = newEnviormentVar(var);
        }
        else
        {
            cmd->code = updateEnVar(var);
        }
      } 
      else 
      {
        if (!(nonBuildCmd(args))) 
        {
            cmd->code = 0;
        } 
        else 
        {
            
            exitFunction();
            cmd->code = 1;
        }
      }
      addLogFunction(cmd);
    }
  }
  else
  {
    printf("Using script mode\n");
    char *args[100], cmdline[1024];
    if(strstr(argv[1],".txt")==NULL)
    {
        strcat(argv[1],".txt");
    }
    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        printf("Unable to open the file\n");
        exit(EXIT_FAILURE);
    }
    while (fgets(cmdline, sizeof(cmdline),fp)!=NULL)
    {
      command *cmd = malloc(sizeof (command));
      time(&rtime);
      cmd->time = *localtime(&rtime);
      stringparser(cmdline, args);
      cmd->name = strdup(args[0]);
      if ((sw = builtinCommandFunc(args[0])) > 0) 
      {
          switch (sw) 
          {
              case 1:
                  exitFunction();
              case 2:
                  cmd->code = printLogFunction();
                  break;
              case 3:
                  cmd->code = printPromptFunc(args);
                  break;
              case 4:
                  cmd->code = changeTheme(args[1]);
                  break;
          }
      } 
      else if (isEnvVar(args[0]) && valEnvVar(args[0])) 
      {
          EnvVar *var = malloc(sizeof(EnvVar));
          var->name = strdup(strtok(args[0], "="));
          var->value = strdup(strtok(NULL, "="));
          if(enviormentVarCheckFunc(var->name)==0)
          {
              cmd->code = newEnviormentVar(var);
          }
          else
          {
              cmd->code = updateEnVar(var);
          }
      } 
      else 
      {
          if (!(nonBuildCmd(args))) 
          {
              cmd->code = 0;
          } 
          else 
          {
              exitFunction();
              cmd->code = 1;
          }
      }
      addLogFunction(cmd);
    }
  }
  return 0;
}

// Function to exit cShell
void exitFunction()
{
  // Print Bye! before exiting 
  printf("Bye!\n");
  exit(0);
}



//-----------------------------------------------------

//--------------------Log functions start---------------------------------

// Helper functions for log
void addLogFunction(command *newcmd)
{
  // case for when the log is empty
  if(cmdsize == 0)
  {
    // Create space for the log
    logCommand = malloc(100* sizeof(command));
    if(logCommand == NULL)
    {
      // if malloc fails
      fprintf(stderr,"error memory allocation\n");
      exit(0);
    }
  }
  // maximum numbers of commands is set to 90 
  if(cmdsize == 90) 
  {
    // Print an error message if the limit is reached
    printf("Maximum number of log size reached");

    // Exit if log limit reached
    exitFunction();
  }
  // add the commands to the log
  logCommand[cmdsize++] = newcmd;
}


int printLogFunction()
{
  // check if log is empty
  if(cmdsize == 0) 
  {
    // Return error message if log is empty
    printf("log is empty\n");
    return 1;
  }
  else
  {
    // Print all the commands log in a loop
    for (int i = 0; i < cmdsize; i++)
    {
      printf("%s",asctime(&logCommand[i]->time));
      printf("%s %d\n",logCommand[i]->name,logCommand[i]->code);
    }
    return 0;
  }
}

// Function to free the log 
void freeLogFunction()
{
  // running the for loop until the end of the log
  for(int i = 0; i < cmdsize; i++)
  {
    // free the log
    free(logCommand[i]);
  }
  // set the size to 0 
  cmdsize = 0;
}

//-------------------Log functions end--------------------

//-------------------Enviorment variable functions start---------------
// Enviorment variable functions 
int newEnviormentVar(EnvVar *newvar)
{
  // Case for the first variable 
  if(varsize == 0)
  {
    envVar = malloc(100* sizeof(EnvVar));
    if(envVar == NULL)
    {
      // print error message if memmory allocation fails
      fprintf(stderr,"Error!! memory allocation\n");
      // Exit if malloc fails
      exitFunction();
    }
  }
  if(varsize == 90) 
  {
    // Print an error message if the limit is reached
    printf("Error!, Cannot define %s", newvar->name);

    // Exit if log limit reached
    exitFunction();
  }
  // add a new variavle 
  envVar[varsize++] = newvar;
  return 0;
}

// Function to change the value of enviorment variable
int updateEnVar(EnvVar *var)
{
  for(int i = 0; i < varsize; i++)
  {
    // check if the variable is same
    if( strcmp(envVar[i]->name, var->name) == 0)
    {
      // update the value and return 0
      envVar[i] = var;
      return 0;
    }
  }
  // returns 1 if fails
  return 1;
}

// function to checks if a variable exists
int enviormentVarCheckFunc(char *name)
{
  for(int i = 0 ;i < varsize; i++)
  {
    // Check if variable exits
    if(strcmp(envVar[i]->name,name)==0)
    {
      // Return 1 if true
      return 1;
    }
  }
  return 0;
}

// Helper function to get the value of variable
char *getVarFunction(char* name)
{
  for(int i = 0; i < varsize; i++)
  {
    // check for the variable name 
    if(strcmp(envVar[i]->name,name)==0)
    {
      // return the value of the variable
      return envVar[i]->value;
    }
  }
  return 0;
}


// Functions to get the user commands
char *getCommandFunc(void)
{
  int len = 0;
  char *buffer = malloc(1024 * sizeof (char));
  int buffsize = 1024;
  char c;
  if(!buffer)
  {
    fprintf(stderr, "Input error\n");
    exitFunction();
  }
  while(1)
  {
    c = getchar();
    if(c == EOF)
    {
      free(buffer);
      return NULL;
    }
    else if( c == '\n')
    {
      buffer[len] = '\0';
      return buffer;
    }
    if(len >= buffsize)
    {
      buffsize += 1024;
      buffer = realloc(buffer,buffsize);
    }
    buffer[len++] = c;
  }
}

// Function to parser the string 
void stringparser(char *parse, char **token)
{
  int i = 0;
  token[i]= strtok(parse," \n\t");
  while( token[i] != NULL && i < 100) 
  {
    i++;
    token[i] = strtok(NULL, " \n\t");
  }
}

// Function to check if the command is a bulit in command 
int builtinCommandFunc(char *command)
{
  // array for all the builtin commands 
  char* buildCmds[] = {"exit","log","print","theme"};
  for(int i = 0; i < 4; i++)
  {
    // chech if it is the built in command 
    if(strcmp(command,buildCmds[i]) == 0)
    {
      // return the command from the array
      return i + 1;
    }
  }
  // return -1 if not a built in command
  return -1;
}

// checks if Argument is in $var format
int isEnvVar(char *cmd)
{
  // if there is a '$' it is a variable 
  if(cmd[0]=='$')
  {
    return 1;
  }
  return 0;
}

// Function to print 
int printPromptFunc(char **parse)
{
  int i = 1;
  while (parse[i] != NULL) 
  {
    // Check if token is an enviorment variable
    if(isEnvVar(parse[i]))
    {
      // Check if argument is an enviorment variable
      if(enviormentVarCheckFunc(parse[i])) 
      {
        // if true get the enviorment variable
        printf("%s ", getVarFunction(parse[i]));
      }
      else
      {
        // Exit otherwise
        exitFunction();
      }
    }
    else 
    {
      // print the token if not enviorment variable
      printf("%s ", parse[i]);
    }
    i++;
  }
  printf("\n");
  return 0;
}
//------------------Print functions and environment variable end--------

//------------------Theme functions Start-----------------
/** Changes the theme of the output */
int changeTheme(char *colour)
{
    char *allowedColours[] = {"red", "green", "blue"};
    if(colour == NULL)
    {
      // 
      printf("unsupported theme\n");
      return 1;
    }
    int clr = 0;
    for(int i = 0; i < 3; i++ )
    {
      if(strcmp(colour,allowedColours[i]) == 0)
      {
        // color from the array
        clr = i + 1;
      }
    }
    // Swith case for themes
    switch(clr)
    {
      case 1:
          printf("\033[1;31m");
          return 0;
      case 2:
          printf("\033[1;32m");
          return 0;
      case 3:
          printf("\033[1;34m");
          return 0;
      default:
          printf("unsupported theme\n");
    }
    return 1;
}

// Function to assign variable a value
int valEnvVar(char* cmd)
{
  char c;
  for(int i = 0;;i++)
  {
    c = cmd[i];
    // check if = is in the cmd 
    if(c == '=')
    {
      return 1;
    }
    else if(c =='\0')
    {
      return 0;
    }
  }
}

// function for non built in commands 
int nonBuildCmd(char** parse)
{
  // using pipe for the non built in commands 
  int pipein[2];
  int pipeout[2];
  if(pipe(pipein) < 0)
  {
    printf("Unable to initialize input pipe!\n");
    return 1;
  }
  if (pipe(pipeout) < 0) 
  {
    printf("Unable to initialize output pipe!\n");
    return 1;
  }
  close(pipein[0]);
  if(parse[1] != NULL) 
  {
    int i =1;
    while (parse[i] != NULL) 
    {
      if (enviormentVarCheckFunc(parse[i])) 
      {
        // using a random variable to get the return value of write
        int a = write(pipein[1], getVarFunction(parse[i]), strlen(parse[i]));
        if(a == -1)
        {
          printf("There is an error");
        }
      } 
      else 
      {
        // using a random variable to get the return value of write
        int b = write(pipein[1], parse[i], strlen(parse[i]));
        if(b == -1)
        {
          printf("There is an error");
        }
      }
      i++;
    }
  }
  close(pipein[1]);
  // using fork
  pid_t p1 = fork();
  if(p1 == -1) 
  {
    printf("Unable to fork a child\n");
    return 1;
  }
  else if(p1 == 0) 
  {
    close(pipein[1]);
    dup2(pipein[0],STDIN_FILENO);
    close(pipein[0]);
    int fail = 0;
    if (execvp(parse[0], parse) < 0) 
    {
        fail = 1;
    }
    close(pipeout[0]);
    // using a random variable to get the return value of write
    int c = write(pipeout[1], &fail, sizeof(fail));
    if(c == -1)
    {
      printf("There is an error");
    }
    close(pipeout[1]);
    exit(0);
  }
  else 
  {
    wait(NULL);
    int ret;
    close(pipeout[1]);
    // using a random variable to get the return value of write
    int d =read(pipeout[0], &ret, sizeof(ret));
    if(d == -1)
    {
      printf("There is an error");
    }
    close(pipeout[0]);
    if(ret == 1) 
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}


