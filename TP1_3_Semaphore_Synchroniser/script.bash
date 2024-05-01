#!/bin/bash
if [ "$#" -ne 3 ]; then
 echo "Utilisation : launch.sh <nb_traitements> <nb_zones> <entier_cle_IPC>"
 exit 1
fi
nb_t=$1
nb_z=$2
int_ipc=$3
./bin/semInit "$nb_t" "$nb_z" "./pourCle.txt" "$int_ipc"
echo "Lancement des $nb_t processus"
for (( c=0; c<$1; c++ ))
do
  echo "./bin/Pi ./pourCle.txt $int_ipc ${c} $nb_t $nb_z"
  xterm -T "Processus ${c}" -geometry 93x31+$((350*c))+350 -e "./bin/Pi pourCle.txt $int_ipc ${c} $nb_t $nb_z" &
done
wait
./bin/supp"./pourCle.txt" "$int_ipc" "$nb_z"