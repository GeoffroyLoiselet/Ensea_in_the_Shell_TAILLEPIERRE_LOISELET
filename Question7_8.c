#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

#define MSG_Welcome "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\n"
#define MSG_Exit "Fin du programme\n"
#define MSG_ForkError "Erreur de fork\n"
#define MSG_InvalidCommand "La commande saisie est introuvable \n"
#define BUFFER_SIZE 128
#define PROMPT_SIZE 64
#define MAX_ARGS 64

char buffer[BUFFER_SIZE];
ssize_t command_len;

void print(const char* msg) {
    write(STDOUT_FILENO, msg, strlen(msg));
}

void readCommand(char* buffer, ssize_t* cmd_len) {
    *cmd_len = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
    if (*cmd_len == 0) {
        buffer[0] = '\0';
        return;
    }
    buffer[*cmd_len] = '\0';
    if (buffer[*cmd_len - 1] == '\n') {
        buffer[*cmd_len - 1] = '\0';
    }
}

int main() {
    int duration = 0;
    signal(SIGINT, SIG_IGN);
    char prompt_msg[PROMPT_SIZE];
    int status;
    int has_status = 0;

    print(MSG_Welcome);

    while (1) {
        if (has_status) {
            if (WIFSIGNALED(status)) {
                sprintf(prompt_msg, "enseash [sign:%d|%dms] %% ",
                        WTERMSIG(status), duration);
            } else if (WIFEXITED(status)) {
                sprintf(prompt_msg, "enseash [exit:%d|%dms] %% ",
                        WEXITSTATUS(status), duration);
            }
        } else {
            sprintf(prompt_msg, "enseash %% ");
        }

        print(prompt_msg);
        readCommand(buffer, &command_len);

        if ((command_len == 0) || strcmp(buffer, "exit") == 0) {
            print(MSG_Exit);
            break;
        }

        struct timespec t_start, t_end;
        clock_gettime(CLOCK_REALTIME, &t_start);

        pid_t pid = fork();

        if (pid < 0) {
            print(MSG_ForkError);
        } 
        else if (pid > 0) {
            waitpid(pid, &status, 0);
            clock_gettime(CLOCK_REALTIME, &t_end);

            long sec_diff = t_end.tv_sec - t_start.tv_sec;
            long nsec_diff = t_end.tv_nsec - t_start.tv_nsec;
            duration = sec_diff * 1000 + nsec_diff / 1000000;
            has_status = 1;
        } 
        else {
            char *args[MAX_ARGS];
            char *input_file = NULL;
            char *output_file = NULL;
            int i = 0;
            int pipefd[2];
            pid_t pid2;
            int pipe_found = 0;

            char *token = strtok(buffer, " ");
            while (token != NULL && i < MAX_ARGS - 1) {
                if (strcmp(token, "<") == 0) {
                    token = strtok(NULL, " ");
                    input_file = token;
                } 
                else if (strcmp(token, ">") == 0) {
                    token = strtok(NULL, " ");
                    output_file = token;
                }
                else if (strcmp(token, "|") == 0) {
                    pipe_found = 1;
                    break;
                } 
                else {
                    args[i++] = token;
                }
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            if (args[0] == NULL) {
                exit(EXIT_SUCCESS);
            }

            // Redirection stdin
            if (input_file) {
                int fd = open(input_file, O_RDONLY);
                if (fd < 0) exit(EXIT_FAILURE);
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Redirection stdout
            if (output_file) {
                int fd = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if (fd < 0) exit(EXIT_FAILURE);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Si un pipe est trouvé
            if (pipe_found) {
                pipe(pipefd);  // Créer un pipe

                pid2 = fork();  // Créer un deuxième processus

                if (pid2 < 0) {
                    print(MSG_ForkError);
                } 
                else if (pid2 > 0) {  // Processus parent
                    close(pipefd[0]);  // Fermeture du côté lecture du pipe

                    // Rediriger stdout du parent vers le pipe
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);

                    // Exécuter la première commande
                    execvp(args[0], args);
                    print(MSG_InvalidCommand);
                    exit(EXIT_FAILURE);
                } 
                else {  // Processus enfant
                    close(pipefd[1]);  // Fermeture du côté écriture du pipe

                    // Rediriger stdin du second processus vers le pipe
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);

                    // Exécuter la deuxième commande
                    execvp(args[i + 1], &args[i + 1]);
                    print(MSG_InvalidCommand);
                    exit(EXIT_FAILURE);
                }
            } 
            else {
                // Si pas de pipe, exécuter normalement
                signal(SIGINT, SIG_DFL);
                execvp(args[0], args);
                print(MSG_InvalidCommand);
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}

