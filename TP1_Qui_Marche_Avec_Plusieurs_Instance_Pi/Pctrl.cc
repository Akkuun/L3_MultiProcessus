
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define ETIQ_DEMANDE_ACCES 1
#define ETIQ_RESSOURCE_LIBEREE 2

using namespace std;

struct msgDepuisPi {
    long etiquette;     // L'étiquette est un entier (la macro)
    long pid;           // Le pid du processus demandant la ressource
};

struct msgDepuisPctrl {
    long pid;           // Ici, l'étiquette est le pid
    char message[1];    // Champ pour assurer la taille minimale de la structure, requis pour la fonction msgrcv qui attend une taille minimale de 1 octet (on peut pas avoir une taille de 0 octet)
};

// Fonction calcul (environ 1 seconde)
void calcul() {
    int i = 0;
    while (i < 1000000000) {
        i++;
    }
}

//Ptrl gère l'accès aux files de messages

/*
 * Pctrl:
        répéter:
            recevoir demande d'accès de Pi
            envoyer accès autorisé à Pi
            recevoir ressource libérée de Pi
    */

int main() {
    // Création de la clé
    key_t cle = ftok("./pourCle.txt", 10);
    if (cle == -1) {
        perror("Problème clé");
        exit(1);
    }
    // Reception de la file de message
    int idPi = msgget(cle, IPC_CREAT | 0666);
    if (idPi == -1) {
        perror("Problème msgget");
        exit(1);
    }

    while (1) {

        // On va recevoir la demande d'accès depuis Pi
        struct msgDepuisPi demandeAcces;
        int resMsgRcv1 = msgrcv(idPi, &demandeAcces, sizeof(demandeAcces), ETIQ_DEMANDE_ACCES, 0);
        if (resMsgRcv1 == -1) {
            perror("Problème msgrcv");
            exit(1);
        }
        printf("Demande d'accès reçue de %ld\n", demandeAcces.pid);

        // On va autoriser l'accès à Pi
        struct msgDepuisPctrl accesAutorise;
        accesAutorise.pid = demandeAcces.pid;
        // On envoie l'accès autorisé à Pi
        // on doit enlever la taille de l'étiquette pour ne pas l'envoyer avec
        int resMsgSnd1 = msgsnd(idPi, &accesAutorise, sizeof(accesAutorise) - sizeof(accesAutorise.pid), 0);
        if (resMsgSnd1 == -1) {
            perror("Problème msgsnd");
            exit(1);
        }
        printf("Accès autorisé à %ld\n", demandeAcces.pid);
        printf("Attente de la libération de la ressource de la part de : %ld ...\n ", demandeAcces.pid);


        // On va recevoir la libération de la ressource de la part de Pi

        struct msgDepuisPi ressourceLiberee;
        int resMsgRcv2 = msgrcv(idPi, &ressourceLiberee, sizeof(ressourceLiberee), ETIQ_RESSOURCE_LIBEREE, 0);
        if (resMsgRcv2 == -1) {
            perror("Problème msgrcv");
            exit(1);
        }
        printf("Ressource libérée par %ld\n", ressourceLiberee.pid);
        printf("Prochaine itération...\n");
    }
    return 0;
}

// Attention car si attend une même etiquette qu'on vient d'envoyer , on risque de l'interpréter comme un message et on va l'extraire
//c'est visible dans le scénrario ou au lieu de renvoyer l'étiquette 2, on renvoie LE PID, ducoup on va l'extraire et on va l'interpréter comme un message