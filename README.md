Programmation système

Dans ce mini projet de TP, nous nous proposons de réaliser un micro shell qui permettra de réaliser différentes tâches.



Question 1 :

Dans un premier temps, nous allons simplement devoir afficher un message d'acceuil ainsi qu'un prompte simple.

Pour ce faire, nous allons définir une fonction permettant d'afficher un message sur la console, il s'agit de la fonction display(). Lorsque nous executons le code, nous utilisons une première fois cette fonction pour afficher le message de bienvenue, puis nous rentrons dans une boucle infinie qui affiche le prompt correspondant (celui-ci dépends de plusieurs choses qui serons vues dans des questions plus tard) Grâce à cela, voici ce que nous obtenons en sortie :

<img width="840" height="79" alt="image" src="https://github.com/user-attachments/assets/8bb53781-247f-4ead-8118-db5ca5aa4636" />

Ici nous obtenons bien ce que nous voulions, un message d'acceuil, et un prompt simple.

Question 2 :

Nous souhaitons maintenant que notre shell puisse faire quelque chose, c'est à dire exécuter une commande.

Afin de pouvoir exécuter une commande donnée par l'utilisateur, nous devons lire ce que ce dernier écrit sur la console. C'est ce à quoi sert la fonction readCommand(). Grâce à cette fonction, nous pouvons lire ce que l'utilisateur écrit pour ensuite traiter ce qui est demandé.

Dans un premier temps, nous voulons simplement pouvoir faire des actions telles que des ls ou autre. Nous allons donc créer la fonction executeCommand(). Comme son nom l'indique, cette fonction exécute ce qui lui a été donné en entrée. Afin de faire le tout fonctionner, nous lisons dans la boucle infinie du main avant de traiter la demande. Voici ce que nous pouvons par exemple faire dans la console :

<img width="841" height="146" alt="image" src="https://github.com/user-attachments/assets/56c124d7-2aef-4125-bd0e-d934a289bf23" />

Ici nous avons bien les commandes ls et date qui se sont éxecutées dans notre shell telles qu'il le fallait.

Question 3 :

Nous voulons à présent pouvoir gérer l'arrêt du shell si l'utilisateur écrit "exit", ou qu'il appuie sur "ctrl + d". Afin de faire cela, nous écrivons la nouvelle fonction endCommand(), qui, en fonction de ce qu'il lui est donné en entrée renvoie True ou False. Nous lui définissons donc les deux conditions d'arrêt en temps que conditions, et nous ajoutons un if() juste après avoir lu ce que l'utilisateur écrit dans le main. Ce if() affiche le message de fermeture et quitte le programme si la condition est True.

Voici une capture d'écran de test de plusieurs appels validant le bon fonctionnement des fonctions écrites pour les questions précédentes, ainsi que le test permettant de valider ce que nous venons d'écrire pour la gestion de la fermeture : 

<img width="673" height="268" alt="image" src="https://github.com/user-attachments/assets/97e81349-952f-45ba-8993-a02579349abc" />

Image Questions 4 et 5 :

<img width="1138" height="271" alt="image" src="https://github.com/user-attachments/assets/98fdf5af-64af-499b-bbe3-c44892cb4bb1" />

Image Question 6 : 

<img width="316" height="90" alt="image" src="https://github.com/user-attachments/assets/8e2fb9aa-c1f1-4b4c-8dab-fb605e3057c3" />

Image Questions 7 et 8 : 

<img width="690" height="164" alt="image" src="https://github.com/user-attachments/assets/40e4187f-6488-4b7d-9615-7772091bc3ec" />

