#!/bin/bash

# ulimit -s unlmited
for ((pe = 1; pe <= 80; pe = pe * 2))
do
   for ((chares = 1; chares <= 32; chares = chares * 2))
   do
       for ((ejecuciones = 1; ejecuciones <= 5; ejecuciones = ejecuciones + 1))
       do
          ./charmrun +p$pe merge $chares 100000000
          echo -n -e "pes: "$pe", chares: "$chares", ejecucion: "$ejecuciones""
          echo -e "\n========================================";
          sleep 3
       done
       echo -e "\n\n======================nueva cant chares====================\n";
   done
   echo -e "\n\n======================nueva cant pes====================\n\n";
done
