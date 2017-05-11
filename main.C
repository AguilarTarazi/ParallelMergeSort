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
    values = (int *)malloc(sizeof(int)*numElements);
    for(int i=0;i<numElements;i++){
        value = rand() % 100; //Aleatorio
        value = numElements-i; //Descendente
        value = i; //Ascendente
        values[i] = value;
        // CkPrintf("Before: Merge[%d]: %d\n",i,values[i]);
    }
    // We are done with msg so delete it.
    delete msg;

    // Set the mainProxy readonly to point to a
    //   proxy for the Main chare object (this
    //   chare object).
    mainProxy = thisProxy;

    // Create the array of Merge chare objects.
    mergeArray = CProxy_Merge::ckNew(numElements);

    // Display the array and then start the first phase
    // mergeArray.displayValue(7,str);
    // arrayDisplayFinished();
    // char str[15] = "Before";
    // startArrayDisplay(&Main::startNextPhase, str);
    startNextPhase();
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Main::Main(CkMigrateMessage* msg) { }

void Main::startNextPhase() {
    // Comienzan su fase (divide) solo el primero y el del medio.
    int *valuesIzq = (int *)malloc(sizeof(int)*(numElements/2));
    int *valuesDer = (int *)malloc(sizeof(int)*(numElements-numElements/2));
    // CkPrintf("TAM IZQ en MAIN 2: %d\n", sizeof valuesIzq);
    // CkPrintf("TAM DER en MAIN 2: %d\n", sizeof valuesDer);

    memcpy(valuesIzq,values,(numElements/2)*sizeof(int));
    memcpy(valuesDer,values+numElements/2,(numElements-numElements/2)*sizeof(int));

    // for(int i=0;i<numElements/2;i++)
    //     CkPrintf("valuesIzq[%d]=%d\n",i,valuesIzq[i]);
    // CkPrintf("\n");
    // for(int j=0;j<numElements-numElements/2;j++)
    //     CkPrintf("valuesDer[%d]=%d\n",j,valuesDer[j]);

    // Reciben como argumento el indice del "ultimo elemento de su array"
    inicio = CkWallTimer();	//Toma tiempo de inicio
    free(values);
    mergeArray[0].initPhase(cantChares/2-1,cantChares-1,0,valuesIzq,numElements/2);
    mergeArray[cantChares/2].initPhase(cantChares-1,-1,0,valuesDer,numElements-numElements/2);
    free(valuesIzq);
    free(valuesDer);
}

void Main::startArrayDisplay(void (Main::*cbFunc)(void), char* prefix) {
    // Set the function to execute when the display process is over
    //   (postDisplayFunc) and start displaying the values.
    postDisplayFunc = cbFunc;
    mergeArray[0].displayValue(strlen(prefix)+1, prefix);
}

void Main::terminar(int valuesSort[]) {

    fin=CkWallTimer();		//Toma tiempo de fin
    for(int i=0;i<numElements;i++){
        CkPrintf("After: Merge[%d]=%d\n",i,valuesSort[i]);
    }
    // Exit the program
    CkPrintf("\nTiempo: %f\n\n",fin-inicio);	//Imprime tiempos
    CkExit();
}

void Main::arrayDisplayFinished() {
    // If there is a post-display function to call, call it.
    if (postDisplayFunc != NULL) (this->*postDisplayFunc)();
}

void Main::exit() {
    // Exit the program
    CkPrintf("Tiempo: %f\n",fin-inicio);	//Imprime tiempos
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
