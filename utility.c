#include "utility.h"

/*
Description : Affichage de texte dans le terminal                                       (Question1)
Paramètre : 
    - msg (const char*), texte à afficher
*/
void display(const char* msg){
    write(STDOUT_FILENO, msg, strlen(msg));
}

/*  
Description : Lecture de ce qu'écrit l'utilisateur                                      (Question 2a)
Paramètre : 
    - buffer (const char*), la commande rentrée par l'utilisateur
Fonctionnement : supprime le '\n' quand on appuie sur entrée, pour le remplacer par '\0'
Pas de retour, mais la commande est enregistrée dans la variable passée en appel
*/
void readCommand(char* buffer, int bufferSize) {
    ssize_t cmd = read(STDIN_FILENO, buffer, bufferSize - 1);
    buffer[cmd] = '\0';

    // Formatage de la commande
    if (cmd == 0) {          
    	buffer[0] = '\0';     
        return;
    }
    if (cmd > 1 && buffer[cmd - 1] == '\n') {
        buffer[cmd - 1] = '\0';
    }
}

/*
Description : Gestion de la commande                                                    (Question 2b)
Paramètres : 
    - cmd    (char*), la commande à éxecute
    - status (int*) , 
Appel de fonction : handleArguments() pour gérer les commandes à plusieurs arguments    (Question 6)
Fonctionnement : La commande est executé via la fonction execvp. 
*/
void executeCommand(char *cmd, int* status){
    pid_t pid = fork();
    if (pid < 0) {                                 // Erreur de fork      
        display(ForkError);     
    } else if (pid > 0) {                          // Parent qui attend la fin du programme enfant
        waitpid(pid, status, 0);
    } else {                                       // Enfant qui execute la commande
        char *args[MAX_ARGS];
        handleArguments(cmd,args);
        execvp(cmd, args);
        display(InvalidCommand);
        exit(EXIT_FAILURE);
    }
}

/*  
Description : Gestion de la sortie du shell                                             (Question 3)
Paramètre : 
    - command (const char*), la commande rentrée par l'utilisateur
Retour : Renvoie True si la commande est "exit" ou un controle+d
*/
bool endCommand(const char *command){
    return strcmp(command, "exit") == 0 || command[0] == '\0';
}

/*  
Description : 
    - Obtention du code de retour (ou du signal) de la commande précédente              (Question 4)
    - Affichage du temps d'exécution de la commande                                     (Question 5)
Paramètres : 
    - buffer (char*), c'est là qu'est enregistré le code de retour
    - cdt (bool), la condition de si il y a un code à récupérer ou pas
    - status (int), le status de la commande executée
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
Description : Arret du chronomètre via la fonction clock_gettime                        (Question 5)
Paramètre : 
    - start (struct timespec *), variable où est stockée le temps de départ
    - end (struct timespec *), variable où est stockée le temps de fin
    - time (int*), valeur finale du temps en ms
*/
void stopTimer(struct timespec *start,struct timespec *end,int *time){
    clock_gettime(CLOCK_REALTIME, end);
    // Extraction du temps : temps de fin - temps de départ
    long sec_diff  = end->tv_sec  - start->tv_sec;  // Obtention des secondes
    long nsec_diff = end->tv_nsec - start->tv_nsec; // Obtention des nanosecondes 

    *time = sec_diff * 1000 + nsec_diff / 1000000;
}

/*
Description : Prise en compte des arguments de la commande                              (Question 6)
Paramètre : 
    - start (struct timespec *), variable où est stockée le temps de départ
    - end (struct timespec *), variable où est stockée le temps de fin
    - time (int*), valeur finale du temps en ms
*/
void handleArguments(char *buffer, char **args) {
    int i = 0;
    char *token = strtok(buffer, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
}
