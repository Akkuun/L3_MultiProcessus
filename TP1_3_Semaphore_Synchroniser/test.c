#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_ZONES 100 // nombre maximal de zones

struct message {
    long mtype; // type de message, doit être > 0
    int zone; // numéro de la zone à traiter
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s nbZones processusId\n", argv[0]);
        exit(1);
    }

    int nbZones = atoi(argv[1]); // nombre de zones à traiter
    int processusId = atoi(argv[2]); // identifiant du processus (1, 2, 3, ...)
    int idFile = msgget(IPC_PRIVATE, 0666 | IPC_CREAT); // création de la file de messages
    if (idFile == -1) {
        perror("Erreur lors de la création de la file de messages\n");
        exit(1);
    }

    // envoi des messages pour traiter les zones dans l'ordre (P1, P2, P3, ...)
    for (int i = 0; i < nbZones; i++) {
        struct message msg;
        msg.mtype = processusId;
        msg.zone = i;
        if (msgsnd(idFile, &msg, sizeof(msg.zone), 0) == -1) {
            perror("Erreur lors de l'envoi du message\n");
            exit(1);
        }
    }

    // réception des messages pour traiter les zones dans l'ordre (P1, P2, P3, ...)
    for (int i = 0; i < nbZones; i++) {
        struct message msg;
        if (msgrcv(idFile, &msg, sizeof(msg.zone), processusId, 0) == -1) {
            perror("Erreur lors de la réception du message\n");
            exit(1);
        }

        // traitement de la zone
        printf("Processus %d traite la zone %d\n", processusId, msg.zone);
        // ...
        sleep(1); // simulateur de traitement

        // envoi du message pour passer au processus suivant
        struct message nextMsg;
        nextMsg.mtype = processusId + 1;
        nextMsg.zone = msg.zone;
        if (msgsnd(idFile, &nextMsg, sizeof(nextMsg.zone), 0) == -1) {
            perror("Erreur lors de l'envoi du message\n");
            exit(1);
        }
    }

    // destruction de la file de messages
    if (msgctl(idFile, IPC_RMID, NULL) == -1) {
        perror("Erreur lors de la destruction de la file de messages\n");
        exit(1);
    }

    return 0;
}