#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>


// Pi souhaite acceder a la file de message, il doit demander l'accès au controleur (Pctrl)

#define ETIQ_DEMANDE_ACCES 1 //etiquette pour demander l'accès
#define ETIQ_RESSOURCE_LIBEREE 2 //etiquette pour signaler que la ressource est libre

using namespace std;

struct msgDepuisPi {
    long etiquette;     // L'étiquette est un entier (la macro)
    long pid;           // Le pid du processus demandant la ressource
};

struct msgDepuisPctrl {
    long pid;           // Le pid du processus demandant la ressource
};

void calcul() {
    int i = 0;
    while (i < 1000000000) {
        i++;
    }
}


// dans cette variante on va faire en sorte de lancer plusieurs Pi pour voir si on a des problèmes
int main(int argc, char *argv[]) {





    key_t cle = ftok("./pourCle.txt", 10); // On crée une clé pour la file de message

    if (cle == -1) {
        perror("Erreur ftok : ");
        exit(2);
    }

    // On crée la file de message
    int idPctrl = msgget(cle, IPC_CREAT | 0666);
    if (idPctrl == -1) {
        perror("Problème msgget");
        exit(1);
    }
    printf("File de message créée\n");


    //On va définir la structure de la demande d'accès
    struct msgDepuisPi demandeAcces;
    demandeAcces.etiquette = ETIQ_DEMANDE_ACCES;
    demandeAcces.pid = getpid();

    printf("Le pid de Pi est : %ld\n", demandeAcces.pid);


    // On envoie la demande d'accès à Pctrl
    int resMsgSnd1 = msgsnd(idPctrl, &demandeAcces, sizeof(demandeAcces) - sizeof(demandeAcces.etiquette), 0);
    if (resMsgSnd1 == -1) {
        perror("Problème msgsnd");
        exit(1);
    }
    printf("Demande d'accès envoyée, attente d'autorisation...\n");

    // On attend la réponse de Pctrl

    struct msgDepuisPctrl accesAutorise;
    int resMsgRcv1 = msgrcv(idPctrl, &accesAutorise, sizeof(accesAutorise), getpid(), 0);
    if (resMsgRcv1 == -1) {
        perror("Problème msgrcv");
        exit(1);
    }


    printf("Accès autorisé\n");

    //on va faire le calcul puis libérer la ressource

    calcul();

    //On va libérer la ressource

    struct msgDepuisPi ressourceLiberee;

    ressourceLiberee.etiquette = ETIQ_RESSOURCE_LIBEREE;
    ressourceLiberee.pid = getpid();
    //envoie du message pour liberer la ressource
    int resMsgSnd2 = msgsnd(idPctrl, &ressourceLiberee,
                            sizeof(ressourceLiberee) - sizeof(ressourceLiberee.etiquette), 0);
    if (resMsgSnd2 == -1) {
        perror("Problème msgsnd");
        exit(1);
    }
    printf("Ressource libérée\n");

    printf("Fin de Pi\n");
    return 0;
}
/*
 *
 * Scenario:
 *
 * au lieu de renvoyer l'étiqutte
 *
 *
 *

 *
 *
 */
