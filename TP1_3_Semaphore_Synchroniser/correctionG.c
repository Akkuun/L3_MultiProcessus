#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <stdlib.h>

/*
 exemple de cr�taion d'un tableau de s�maphores, dont le nombre
 d'�l�ments et la valeur initiale est pass�e en param�tre du
 programme (dans cet exemple, les �lements sont initialis�s � la m�me valeur)

 */


// le sempahore contient un tableau de semaphores, chaque case contient le nombre de
// zones trait�es par le processus  +1 correspondant

/*
 p_att c'est la zone partagée
idSem c'est l'id du tableau de semaphores
(pas l'id du sémaphore que tu vise, celui là tu le précise dans chaque op)
et id_traitement tu l'attribue toi meme quand tu lance le programme pour moi
 */

// si semaphore [3 0] ->  3 traitements,
// le traitement 1 peut faire traiter 3 zone,
// le  traitement 1 peut traiter 2 zones, le traitement 2 est en attente

int main(int argc, char *argv[]) {

    if (argc != 6) {
        printf("Nbre d'args invalide, utilisation :\n");
        printf("%s fichier-pour-cle-ipc entier-pour-cl�-ipc id_traitement nb_traitements nb_zones\n", argv[0]);
        exit(0);
    }

    u_short id_traitement = atoi(argv[3]); // id du traitement
    u_short nb_traitements = atoi(argv[4]); // nombre de traitements
    int nb_zones = atoi(argv[5]);

    printf("Traitement %d\n", id_traitement);

    int cleSem = ftok(argv[1], atoi(argv[2]));

    int idSem = semget(cleSem, 1, 0666);

    srand(time(NULL));

    int idShm = shmget(cleSem, nb_zones * sizeof(int), 0666);

    int *p_att = (int *) shmat(idShm, NULL, 0);
    if (p_att == (int *) -1) {
        perror("erreur shmat : ");
        exit(-1);
    }

    struct sembuf op[] = {
            {(u_short) id_traitement - 1, (short) -1, 0},
            {(u_short) id_traitement,     (short) +1, 0}
    };


    for (int i = 0; i < nb_zones; i++) {
          u if (id_traitement > 0) { // si c'est pas le 1er traitement
            /*
            // la zone est libre ?
             * Si le traitement a comme id 1 ou plus,
             * alors il regarde dans le semaphore id - 1 s'il peut enlever
             * un jeton (si il peut pas ça le bloque tant que y'a pas de jeton à enlever)
             * (Vu que le 1er traitement id=0 attend personne, il a pas de
             * semaphore à aller voir)
             */

            //si il peut enlever le jeton cela signifie que le traitement précédent a fini de traiter la zone
            printf("Je veux faire -1 au sémaphore %u\n", id_traitement - 1);
            //, si ce sémaphore vaut 0 tu peux pas enlever 1 donc t'es en attente
            if (semop(idSem, op, 1) == -1) perror(" erreur semop : ");
            printf("J'ai fait -1 au sémaphore %u\n", id_traitement - 1);
        }
        printf("Je traite la zone %d\n", i);
        sleep((nb_traitements - id_traitement) + rand() % 2);
        p_att[i]++;
        printf("J'ai fini de traiter la zone %d\n", i);
        if (id_traitement < nb_traitements - 1) {
            // si c'est pas le dernier traitement
            //je termine la zone ?
            /*
             *
             * si le traitement est pas le dernier traitement,
             * il ajoute 1 au semaphore qui a son id (vu que le dernier est
             * attendu par personne, ça sert à rien qu'il dise où il en est)
             *
             *
             */
            //on fait °1 au sémaphore id_traitement cela signifie que le traitement a fini de traiter la zone
            printf("J'ai fait +1 au sémaphore %u\n", id_traitement);
            if (semop(idSem, op + 1, 1) == -1) perror(" error semop : ");
        }
        printf("\n");
    }

    int dtres = shmdt((void *) p_att);
    if (dtres == -1) {
        perror("erreur shmdt : ");
        exit(-1);
    }

    printf("Fin, saisissez un char pour terminer\n");
    getchar();

    return 0;
}