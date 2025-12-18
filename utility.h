#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>

#define ForkError "Erreur de fork\n"
#define InvalidCommand "La commande saisie est introuvable \n"
#define MAX_ARGS 10
void display(const char* msg);
void readCommand(char* buffer, int bufferSize);
void executeCommand(char *cmd,int* status);
bool endCommand(const char *command);
void buildPrompt(char* buffer, bool cdt, int status, int duration);
void startTimer(struct timespec *start);
void stopTimer(struct timespec *start,struct timespec *end,int *time);
void handleArguments(char *buffer, char **args);