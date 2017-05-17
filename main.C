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
/* readonly */ int cantChares;

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
    mergeArray = CProxy_Merge::ckNew(cantChares);

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
    // for(int i=tam-15;i<tam;i++){
    //     CkPrintf("After: Merge[%d]=%d\n",i,valuesSort[i]);
    // }
    // Exit the program
    CkPrintf("\n========================================");	//Imprime tiempos
    CkPrintf("\nTIEMPO DE CALCULO: %f\n",fin-inicio);	//Imprime tiempos
    // CkPrintf("\nTIEMPO DE EJECUCION: %f\n\n",stop-start);	//Imprime tiempos
    CkExit();
}

void Main::barrier(){
    cantCheck++;
    // CkPrintf("cantCheck: %d\n",cantCheck);
    if(cantCheck==cantChares){
        // CkPrintf("LISTO\n");
        mergeArray.listo();
    }
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
