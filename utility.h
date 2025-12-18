#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>

#define ForkError "Erreur de fork\n"
#define InvalidCommand "La commande saisie est introuvable \n"
#define MAX_ARGS 10

void display(const char* msg);
void readCommand(char* buffer, int bufferSize, ssize_t *cmd_l);
bool endCommand(const char *command, ssize_t *l);
void buildPrompt(char* buffer, bool cdt, int status, int duration);
void startTimer(struct timespec *start);
void stopTimer(struct timespec *start,struct timespec *end,int *time);
void parseCommand(char* cmd, char** args, char** input_file, char** output_file);
void executeSimpleCommand(char *cmd);
void executePipe(char *cmd1_str, char *cmd2_str, int *status);