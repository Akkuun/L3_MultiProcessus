#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

// Fonction calcul (environ 1 seconde)
void calcul() {
    int i = 0;
    while(i<1000000000) {
        i++;
    }
}

// dans le conxtex de ce rdv , on a 1 semaphore avuec une varaible partagée : le nombre de places restantes (compteur)initilié à n

// on a par exemple : semaphore = [2] pour 2 place restantes donc on lance 2 fois rdv.

//on a 3 opérations possibles sur le semaphore : P, V, Z
// P : bloque l'appelant si la valeur du sémaphore est inférieure à n
// V : incrémente la valeur du sémaphore de taille n et débloque les attentes
// Z : attend que le sémaphore soit nul




int main(int argc, char * argv[]){

    if (argc != 1){
        printf("./rdv \n");
        exit(1);
    }


    key_t cle = ftok("./pourCle.txt", 10); // On crée une clé pour le sémaphore
    if(cle == -1) {
        perror("Problème clé \n");
        exit(1);
    }

    int idSem = semget(cle,1, IPC_CREAT|0666); // retourne l'identifiant de l'ensemble de sémaphores associé à la valeur de clé key
    if(idSem == -1) {
        perror("Problème semget \n");
        exit(1);
    }

    calcul();

    union semun { // permet de manipuler les valeurs des sémaphores
        unsigned short *array ; /* cmd = GETALL ou SETALL */

    }rdv;

    rdv.array = (unsigned short *) malloc(sizeof(unsigned short)*1);// declaration de la taille du tableau de sémaphores (1 car 1 seul semaphore)

    if(semctl(idSem, 0, GETALL, rdv) == -1){ //permet d'exécuter des opérations de contrôle sur un ensemble de sémaphores, on utilise GETALL pour récupérer les valeurs des sémaphores
        perror("Problème semctl \n");
        exit(1);

    }

    printf("Nombre de places restantes : %d \n", rdv.array[0]);
// p : Prendre ou prelever
    //Opération P bloque l'appelant si la valeur du sémaphore à l'indice i es inférieure à n
    struct sembuf opp;
    opp.sem_num=0; // numero du sémaphore
    opp.sem_op =-1; // opération du semaphore, donc on décrémente
    opp.sem_flg=0; // options
    if (semop(idSem,&opp,1)!=0){
        perror("Problème semop \n");
        exit(1);
    }


    if(semctl(idSem, 0, GETALL, rdv) == -1){ //permet de créer, supprimer, get, set, on fait GETALL pour récuperer à nouveau la valeur de la varaible
        perror("Problème semctl \n");
        exit(1);
    }

    printf("Nombre de places après la prise : %d \n", rdv.array[0]);


    //Opération Z attend que le smaphore à l'indice i soit nul
    //condition d'attente pour le rdv
    struct sembuf opz;
    opz.sem_num=0;
    opz.sem_op = 0;
    opz.sem_flg=0;
    if (semop(idSem,&opz,1)!=0){
        perror("Problème semop \n");
        exit(1);
    }





    //
    //Opération V incrémente la valeur du sémaphore à l'indice i de taille n et débloque les attentes
    //Quand on est le joueur qui prends le dernier sémaphore disponible
    /*
    struct sembuf opv;
    opv.sem_num = 0;
    opv.sem_op += 1;
    opv.sem_flg = 0;
    if (semop(idSem,&opv,1)!=0){
        perror("Problème semop \n");
        exit(1);
    }

    printf("On est les dernier, faut réveiller tous les autres hehe \n");
    */


    printf("On commence à jouer \n");






}