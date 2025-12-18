#include "utility.h"

/*
Description : Affichage de texte dans le terminal                                       (Question 1)
Paramètre : 
    - msg (const char*), texte à afficher
*/
void display(const char* msg){
    write(STDOUT_FILENO, msg, strlen(msg));
}

/* Description : Lecture de ce qu'écrit l'utilisateur                                      (Question 2a)
Paramètres : 
    - buffer (char*), la zone mémoire où stocker la commande
    - bufferSize (int), taille maximale du buffer
    - cmd_l (ssize_t*), pointeur pour stocker le nombre d'octets lus
Fonctionnement : Supprime le '\n' final pour le remplacer par '\0' afin de faciliter le parsing
*/
void readCommand(char* buffer, int bufferSize, ssize_t *cmd_l) {
    *cmd_l = read(STDIN_FILENO, buffer, bufferSize - 1);
    if (*cmd_l <= 0) return;
    buffer[*cmd_l] = '\0';
    if (buffer[*cmd_l - 1] == '\n') buffer[*cmd_l - 1] = '\0';
}

/* Description : Gestion de la sortie du shell                                             (Question 3)
Paramètres : 
    - command (const char*), la commande rentrée par l'utilisateur
    - l (ssize_t*), nombre d'octets lus (pour détecter Ctrl+D)
Retour : Renvoie True si la commande est "exit" ou si l'utilisateur fait Ctrl+D
*/
bool endCommand(const char *command, ssize_t *l){
    return (*l == 0) || strcmp(command, "exit") == 0;
}

/* Description : Construction du prompt avec code de retour et temps                       (Questions 4 & 5)
Paramètres : 
    - buffer (char*), buffer où le prompt sera écrit
    - cdt (bool), condition indiquant s'il y a un statut à afficher
    - status (int), le statut brut du processus fils
    - duration (int), durée d'exécution en ms
*/
void buildPrompt(char* buffer, bool cdt, int status, int duration){
    if(cdt){
        if(WIFSIGNALED(status))       
            sprintf(buffer, "enseash [sign:%d|%dms] %% ", WTERMSIG(status), duration);
        else if (WIFEXITED(status))   
            sprintf(buffer, "enseash [exit:%d|%dms] %% ", WEXITSTATUS(status), duration);
        else                   
            sprintf(buffer, "enseash %% ");                     
    } else {
        sprintf(buffer, "enseash %% ");
    }
}

/*
Description : Début du chronomètre via la fonction clock_gettime                        (Question 5)
Paramètre : 
    - start (struct timespec *), variable où est stockée le temps de départ
*/
void startTimer(struct timespec *start){
    clock_gettime(CLOCK_REALTIME, start);
}

/*
Description : Arret du chronomètre et calcul de la différence                           (Question 5)
Paramètres : 
    - start (struct timespec *), temps de départ
    - end (struct timespec *), temps de fin
    - time (int*), pointeur où sera stockée la durée finale en ms
*/
void stopTimer(struct timespec *start,struct timespec *end,int *time){
    clock_gettime(CLOCK_REALTIME, end);
    long sec_diff  = end->tv_sec  - start->tv_sec;
    long nsec_diff = end->tv_nsec - start->tv_nsec; 
    *time = (int)(sec_diff * 1000 + nsec_diff / 1000000);
}

/*
Description : Découpage de la commande en arguments et détection des redirections        (Questions 6 & 7)
Paramètres : 
    - cmd (char*), la chaîne de caractères brute
    - args (char**), tableau de chaînes pour les arguments (ex: ls, -l)
    - input_file (char**), pointeur vers le nom du fichier d'entrée (<)
    - output_file (char**), pointeur vers le nom du fichier de sortie (>)
*/
void parseCommand(char* cmd, char** args, char** input_file, char** output_file) {
    int i = 0;
    *input_file = NULL;
    *output_file = NULL;
    char* token = strtok(cmd, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) {
            *input_file = strtok(NULL, " ");
        } else if (strcmp(token, ">") == 0) {
            *output_file = strtok(NULL, " ");
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

/*
Description : Application des redirections de fichiers via descripteurs                 (Question 7)
Paramètres : 
    - in (char*), nom du fichier d'entrée (NULL si aucun)
    - out (char*), nom du fichier de sortie (NULL si aucun)
Fonctionnement : Utilise open() et dup2() pour rediriger STDIN_FILENO ou STDOUT_FILENO
*/
void applyRedirections(char* in, char* out) {
    if (in) {
        int fd = open(in, O_RDONLY);
        if (fd != -1) { dup2(fd, STDIN_FILENO); close(fd); }
    }
    if (out) {
        int fd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd != -1) { dup2(fd, STDOUT_FILENO); close(fd); }
    }
}

/*
Description : Exécution d'une commande simple (avec arguments et redirections)          (Question 2b & 6)
Paramètre : 
    - cmd (char*), la commande à traiter
Appel de fonction : parseCommand() et applyRedirections()
Fonctionnement : Destinée à être appelée dans le fils après un fork()
*/
void executeSimpleCommand(char *cmd) {
    char *args[MAX_ARGS], *in = NULL, *out = NULL;
    parseCommand(cmd, args, &in, &out);
    
    signal(SIGINT, SIG_DFL); // Rétablit le comportement par défaut de Ctrl+C
    applyRedirections(in, out);
    
    execvp(args[0], args);
    display(InvalidCommand);
    exit(EXIT_FAILURE);
}

/*
Description : Gestion de l'exécution de deux commandes liées par un pipe                (Question 8)
Paramètres : 
    - cmd1_str (char*), partie gauche du pipe
    - cmd2_str (char*), partie droite du pipe
    - status (int*), statut de sortie pour le prompt
Fonctionnement : Crée un tube anonyme et deux fils pour lier la sortie de l'un à l'entrée de l'autre
*/
void executePipe(char *cmd1_str, char *cmd2_str, int *status) {
    int pipefd[2];
    if (pipe(pipefd) == -1) return;

    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]); close(pipefd[1]);
        executeSimpleCommand(cmd1_str);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]); close(pipefd[0]);
        executeSimpleCommand(cmd2_str);
    }

    close(pipefd[0]); close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, status, 0);
}