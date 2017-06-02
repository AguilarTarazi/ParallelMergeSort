/*
Agregados 3 Mayo
cantChares como variable de solo lectura
*/
#include "main.decl.h"
#include "main.decl.h"
#include "main.h"
#include "merge.decl.h"
#include <stdlib.h>
#include <time.h>

/* readonly */ CProxy_Main mainProxy;
/* readonly */ int numElements;

/*
EJEMPLO
@param	nombre del parámetro	descripción de su significado y uso
@name
*/

Main::Main(CkArgMsg* msg) {
    //Initialize member variables
    // Read in any command-line arguments
    start = CkWallTimer();
    cantCheck = 0;
    numElements = 4;
    cantChares = 4;
    if (msg->argc > 2){
        cantChares = atoi(msg->argv[1]);
        numElements = atoi(msg->argv[2]);
        if(numElements<cantChares){
            CkPrintf("\n********************************************************************\n");
            CkPrintf("\nLa cantidad de elementos debe ser mayor o igual que la cantidad de chares.\n");
            CkPrintf("Se ejecuta con igual cantidad de chares y elementos.\n");
            CkPrintf("\n********************************************************************\n");
            cantChares=numElements;
        }
    }
    else{
        CkPrintf("Usar 'name (cantChares) (numElements)'\n");
        CkExit();
    }
    CkPrintf("\nCANTIDAD DE CHARES: %d\nCANTIDAD DE ELEMENTOS: %d\n",cantChares,numElements);
    values = (int *)malloc(numElements*sizeof(int));
    if(values == NULL){
        CkPrintf("values es NULL\n");
        CkExit();
    }
    // values = (int *)malloc(sizeof(int)*numElements);
    for(int i=0;i<numElements;i++){
        value = numElements-i; //Descendente
        value = i; //Ascendente
        value = rand() % 10000; //Aleatorio
        values[i] = value;
    }
    // We are done with msg so delete it.
    // for(int i=numElements-10;i<numElements;i++)
    //     CkPrintf("Before: Merge[%d]: %d\n",i,values[i]);

    delete msg;

    // Set the mainProxy readonly to point to a
    //   proxy for the Main chare object (this
    //   chare object).
    mainProxy = thisProxy;

    // Create the array of Merge chare objects.
    // CkPrintf("HELLO\n");
    mergeArray = CProxy_Merge::ckNew();

    // cat /proc/cpuinfo | grep -E "processor|core id|physical id"
    if(CmiNumPes() == 32 ){
        CkPrintf("Modo de Asignación: Manual para 32 hilos.\n");
        //Asignacion sobre physical 0
        // mergeArray[0].insert(0);
        // mergeArray[16].insert(4);
        //
        // mergeArray[8].insert(1);
        // mergeArray[12].insert(5);
        //
        // mergeArray[24].insert(2);
        // mergeArray[28].insert(3);
        //
        // mergeArray[20].insert(6);
        // mergeArray[4].insert(7);
        //
        // int RR = 8;
        // for (int i = 0; i < cantChares; i++){
        //   if(i != 0 && i != 16 && i != 8 && i != 12 && i != 24 && i != 28 && i != 20 && i != 4)
        //       if(RR < CmiNumPes()){
        //         // while(RR == 0 || RR == 4 || RR == 1 || RR == 5 || RR == 2 || RR == 6 || RR == 3 || RR == 7 )
        //         // while(RR < 8 || RR > 15)
        //         //   RR++;
        //         // if(RR == 8)
        //           // RR = 16;
        //         mergeArray[i].insert(RR++);
        //         CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
        //       }
        //       // Para cuando hay mas chares que procesadores
        //       else{
        //         RR = 0;
        //         mergeArray[i].insert(RR++);
        //         CkPrintf("Chare %d agregado al procesador %d. +\n",i,RR-1);
        //       }
        //   else
        //     CkPrintf("Chare %d agregado *\n",i);
        // }

        //Asignacion sobre physical 1
        // mergeArray[0].insert(8);
        // mergeArray[16].insert(9);
        //
        // mergeArray[8].insert(12);
        // mergeArray[12].insert(13);
        //
        // mergeArray[24].insert(10);
        // mergeArray[28].insert(11);
        //
        // mergeArray[20].insert(14);
        // mergeArray[4].insert(15);


        // int RR = 0;
        // for (int i = 0; i < cantChares; i++){
        //   if(i != 0 && i != 16 && i != 8 && i != 12 && i != 24 && i != 28 && i != 20 && i != 4)
        //       if(RR < CmiNumPes()){
        //         // while(RR == 0 || RR == 4 || RR == 1 || RR == 5 || RR == 2 || RR == 6 || RR == 3 || RR == 7 )
        //         // while(RR < 8 || RR > 15)
        //         //   RR++;
        //         if(RR == 8)
        //           RR = 16;
        //         mergeArray[i].insert(RR++);
        //         CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
        //       }
        //       // Para cuando hay mas chares que procesadores
        //       else{
        //         RR = 0;
        //         mergeArray[i].insert(RR++);
        //         CkPrintf("Chare %d agregado al procesador %d. +\n",i,RR-1);
        //       }
        //   else
        //     CkPrintf("Chare %d agregado *\n",i);
        // }

        // // Asignacion completa1 mismo physical
        // mergeArray[0].insert(1);
        // mergeArray[16].insert(3);
        //
        // mergeArray[8].insert(5);
        // mergeArray[12].insert(4);
        //
        // mergeArray[24].insert(7);
        // mergeArray[28].insert(6);
        //
        // mergeArray[20].insert(2);
        // mergeArray[4].insert(0);
        //
        // mergeArray[1].insert(9);
        // mergeArray[2].insert(16);
        // mergeArray[3].insert(17);
        // mergeArray[5].insert(8);
        // mergeArray[6].insert(20);
        // mergeArray[7].insert(21);
        // mergeArray[9].insert(13);
        // mergeArray[10].insert(18);
        // mergeArray[11].insert(19);
        // mergeArray[13].insert(12);
        // mergeArray[14].insert(22);
        // mergeArray[15].insert(23);
        // mergeArray[17].insert(11);
        // mergeArray[18].insert(24);
        // mergeArray[19].insert(25);
        // mergeArray[21].insert(10);
        // mergeArray[22].insert(28);
        // mergeArray[23].insert(29);
        // mergeArray[25].insert(15);
        // mergeArray[26].insert(26);
        // mergeArray[27].insert(27);
        // mergeArray[29].insert(14);
        // mergeArray[30].insert(30);
        // mergeArray[31].insert(31);

        // Asignacion completa1 mismo core
        // mergeArray[0].insert(1);
        // mergeArray[16].insert(3);
        //
        // mergeArray[8].insert(5);
        // mergeArray[12].insert(4);
        //
        // mergeArray[24].insert(7);
        // mergeArray[28].insert(6);
        //
        // mergeArray[20].insert(2);
        // mergeArray[4].insert(0);
        //
        // mergeArray[1].insert(9);
        // mergeArray[2].insert(16);
        // mergeArray[3].insert(20);
        // mergeArray[5].insert(8);
        // mergeArray[6].insert(17);
        // mergeArray[7].insert(21);
        // mergeArray[9].insert(13);
        // mergeArray[10].insert(18);
        // mergeArray[11].insert(22);
        // mergeArray[13].insert(12);
        // mergeArray[14].insert(19);
        // mergeArray[15].insert(23);
        // mergeArray[17].insert(11);
        // mergeArray[18].insert(24);
        // mergeArray[19].insert(28);
        // mergeArray[21].insert(10);
        // mergeArray[22].insert(25);
        // mergeArray[23].insert(29);
        // mergeArray[25].insert(15);
        // mergeArray[26].insert(26);
        // mergeArray[27].insert(30);
        // mergeArray[29].insert(14);
        // mergeArray[30].insert(27);
        // mergeArray[31].insert(31);

        //Asginacion Agrupada
        mergeArray[0].insert(0);
        mergeArray[16].insert(16);
        mergeArray[8].insert(8);
        mergeArray[24].insert(24);
        mergeArray[12].insert(5);
        mergeArray[28].insert(3);
        mergeArray[20].insert(6);
        mergeArray[4].insert(7);

        mergeArray.doneInserting();
    }
    else{
      CkPrintf("Modo de Asignación: Automática.\n");
      mergeArray = CProxy_Merge::ckNew(cantChares);
    }

    CkPrintf("Fin de Asignación.\n");

    // mergeArray[0].insert(0);
    // mergeArray[10].insert(1);
    // mergeArray[20].insert(2);
    // mergeArray[30].insert(3);
    // mergeArray[15].insert(4);
    // mergeArray[25].insert(5);
    // mergeArray[35].insert(6);
    // int RR = 7;
    // for (int i = 0; i < cantChares; i++){
    //   if(i != 0 && i != 10 && i != 20 && i != 30 && i != 15 && i != 25 && i != 35)
    //     if(RR < CmiNumPes()){
    //       mergeArray[i].insert(RR++);
    //       CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
    //     }
    //     else{
    //       RR = 1;
    //       mergeArray[i].insert(RR++);
    //       CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
    //     }
    //   else
    //     CkPrintf("Chare %d agregado al procesador %d *\n",i,0);
    // }

    // int posIzq = 0;
    // int posDer = cantChares/2;
    // mergeArray[posIzq].insert(0);
    // mergeArray[posDer].insert(0);
    // int RR = 1;
    // for (int i = 0; i < cantChares; i++){
    //   if(i != posIzq && i != posDer)
    //     if(RR < CmiNumPes()){
    //       mergeArray[i].insert(RR++);
    //       CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
    //     }
    //     else{
    //       RR = 0;
    //       mergeArray[i].insert(RR++);
    //       CkPrintf("Chare %d agregado al procesador %d\n",i,RR-1);
    //     }
    //   else
    //     CkPrintf("Chare %d agregado al procesador %d *\n",i,0);
    // }

    startNextPhase();
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Main::Main(CkMigrateMessage* msg) { }

void Main::startNextPhase() {
    // Comienzan su fase (divide) solo el primero y el del medio.
    int *valuesIzq = (int *)malloc(sizeof(int)*(numElements/2));
    if(valuesIzq == NULL){
        CkPrintf("valuesIzq es NULL\n");
        CkExit();
    }
    int *valuesDer = (int *)malloc(sizeof(int)*(numElements-numElements/2));
    if(valuesDer == NULL){
        CkPrintf("valuesDer es NULL\n");
        CkExit();
    }
    memcpy(valuesIzq,values,(numElements/2)*sizeof(int));
    memcpy(valuesDer,values+numElements/2,(numElements-numElements/2)*sizeof(int));
    inicio=CkWallTimer();	//Toma tiempo de inicio
    mergeArray[0].initPhase(cantChares/2-1,cantChares-1,0,numElements/2,valuesIzq,cantChares/2);
    mergeArray[cantChares/2].initPhase(cantChares-1,-1,0,numElements-numElements/2,valuesDer,-1);
    free(valuesIzq);
    valuesIzq=NULL;
    free(valuesDer);
    valuesDer=NULL;
    free(values);

}

void Main::terminar(int tam, int valuesSort[]) {
    fin=CkWallTimer();		//Toma tiempo de fin
    stop = CkWallTimer();
    int n = numElements;
    int error = 0;
    for(int i = 0; i < n-1; i++){
      // if(i < n-1)
        if(valuesSort[i] > valuesSort[i+1])
          error = 1;
    }
    // for(int i=n-5; i<n; i++)
    // CkPrintf("\nAfter: Merge[%d]=%d",i,valuesSort[i]);
    // Exit the program
    CkPrintf("\n[pmerge]========================================================================================================");
    // CkPrintf("\n[pmerge]==================================================================================================================");
    CkPrintf("\n[pmerge Tiempo: %.2f segundos\t| Cantidad de Chares: %d\t| Cantidad de Elementos: %'d ",fin-inicio,cantChares,numElements);	//Imprime tiempos
    // CkPrintf("\n[pmerge]\tTiempo: %.2f segundos\t|\tCantidad de Chares: %d\t|\tCantidad de Elementos: %'d ",fin-inicio,cantChares,numElements);	//Imprime tiempos
    if(error == 1)      CkPrintf("Orden: ERROR");
    else      CkPrintf("\tOrden: OK");
    // CkPrintf("\n=============================================================================================");
    // CkPrintf("\n                                     FIN DEL PROGRAMA                                         ");
    CkPrintf("\n[pmerge]========================================================================================================\n");
    // CkPrintf("\n[pmerge]==========================================================================================================\n");




    // for(int i=tam-15;i<tam;i++){
    //     CkPrintf("After: Merge[%d]=%d\n",i,valuesSort[i]);
    // }
    // // Exit the program
    // CkPrintf("\n========================================");	//Imprime tiempos
    // CkPrintf("\n========================================");	//Imprime tiempos
    // CkPrintf("\nTIEMPO DE CALCULO: %f\n",fin-inicio);	//Imprime tiempos
    // CkPrintf("\n========================================");	//Imprime tiempos
    // CkPrintf("\n========================================");	//Imprime tiempos
    // CkPrintf("\nTIEMPO DE EJECUCION: %f\n\n",stop-start);	//Imprime tiempos
    CkExit();
}


// Because this function is declared in a ".ci" file (main.ci in this
//   case) with the "initproc" keyword, it will be called once at startup
//   (before Main::Main()) on each physical processor.  In this case,
//   it is being used to seed the random number generator on each processor.
void processorStartupFunc() {
    // Seed rand() with a different value for each processor
    srand(time(NULL) + CkMyPe());
}

#include "main.def.h"
