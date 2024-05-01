#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>


void calcul() {
    int i = 0;
    while(i<1000000000) {
        i++;
    }
}

int main(int argc, char * argv[]){

    if (argc!=3) {
        printf("nbZones qui-suis-je\n");
        exit(0);
    }

    int nbZones = atoi(argv[1]);
    int id = atoi(argv[2]);

    key_t cle = ftok("./pourCle.txt", 5);
    if(cle == -1) {
        perror("Problème clé \n");
        exit(1);
    }

    int idSem = semget(cle,1, IPC_CREAT|0666);
    if(idSem == -1) {
        perror("Problème semget \n");
        exit(1);
    }

    int idMem=shmget(cle, nbZones*sizeof(int), IPC_CREAT|0666); //mémoire partagée
    if(idMem == -1){
        perror("shmget \n");
        exit(1);
    }

    int *image;
    if((image = (int *)shmat(idMem, NULL, 0))==(int *)-1){
        perror("shmat \n");
        exit(1);
    }


    for (int i = 0; i < nbZones; i++){
        image[i] = id;
        //P id
        printf("%d \n", id);
        struct sembuf opp;
        opp.sem_num=i;//indice de la sem dans le tableau
        opp.sem_op =- id;
        opp.sem_flg=0;
        printf("hehe");
        if (semop(idSem,&opp,1)!=0){
            perror("Problème semop \n");
            exit(1);
        }

        calcul();

        //V id + 1
        struct sembuf opv;
        opv.sem_num = i;
        opv.sem_op += id +1;
        opv.sem_flg = 0;
        if (semop(idSem,&opv,1)!=0){
            perror("Problème semop \n");
            exit(1);
        }

    }

    for(int i = 0; i < nbZones; i++){
        printf("[%d]", image[i]);
    }
    printf("\n");

    if(shmdt((int *)image)==-1){
        perror("Problème shmdt");
        exit(1);
    }

}