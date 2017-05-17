#include "merge.decl.h"
#include "merge.h"
#include "main.decl.h"
#include <stdlib.h>
#include <time.h>
// #include <malloc.h>

extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ int cantChares;
extern /* readonly */ int numElements;

Merge::Merge() {
    phase = 0;
    activo = false;
    indexLlamoIzq = -1;
    cantFases = 0;
    elementos = -1;

}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) { }

void Merge::initPhase(int pos, int posDer, int phaseN, int tam,int values[],int proxIndex) {
    elementos = tam;
    int *valuesIzq = (int *)malloc(sizeof(int)*(elementos/2));
    int *valuesDer = (int *)malloc(sizeof(int)*(elementos-elementos/2));
    if(valuesIzq == NULL){
        CkPrintf("[%d] valuesIzq es NULL\n",thisIndex);
        CkExit();
    }
    if(valuesDer == NULL){
        CkPrintf("[%d] valuesDer es NULL\n",thisIndex);
        CkExit();
    }
    memcpy(valuesIzq,values,(elementos/2)*sizeof(int));
    memcpy(valuesDer,values+elementos/2,(elementos-elementos/2)*sizeof(int));
    phase = phaseN;
    phase++;
    if(posDer!=-1){
        comparar[cantFases]=proxIndex;
        cantFases++;
    }
    newcantChares = pos-thisIndex+1;
    newPos = newcantChares/2-1+thisIndex;
    if(newcantChares < 2){
        myValues = (int *)malloc(sizeof(int)*elementos);
        if(myValues == NULL){
            CkPrintf("[%d] myValues es NULL\n",thisIndex);
            CkExit();
        }
        memcpy(myValues,values,(elementos)*sizeof(int));        //Se copian los valores en variable local
        sort(0,elementos-1);
        // mainProxy.barrier();
        if(posDer!=-1){
            activo = true;
            startCompare(comparar[cantFases-1]);
        }
    }
    else{
        thisProxy[thisIndex].initPhase(newPos,pos,phase,elementos/2,valuesIzq,newPos+1);
        thisProxy[newPos+1].initPhase(pos,-1,phase,elementos-elementos/2,valuesDer,-1);
        free(valuesIzq);
        valuesIzq=NULL;
        free(valuesDer);
        valuesDer=NULL;
    }
}

void Merge::listo(){
    if(activo){
        startCompare(comparar[cantFases-1]);
    }
}

void Merge::startCompare(int indexDer){
    thisProxy[indexDer].requestSwap(phase,thisIndex,myValues[elementos-1]);
}

void Merge::requestSwap(int phaseN,int indexIzq,int lastValueN){
    if((phase == phaseN || !activo) && myValues!=NULL){
        if(lastValueN > myValues[0]){
            thisProxy[indexIzq].saveValue(myValues,elementos,true);
            free(myValues);
            myValues=NULL;
        }else{
            thisProxy[indexIzq].saveValue(myValues,elementos,false);
            free(myValues);
            myValues=NULL;
        }
    }else{
        indexLlamoIzq = indexIzq;
    }
}

void Merge::saveValue(int valuesN[], int elementosN, bool ordenar){
    int *valuestmp = (int *)realloc(myValues,(elementos+elementosN)*sizeof(int));
    myValues=NULL;
    if(valuestmp==NULL){
            CkPrintf("[%d] valuestmp es NULL\n",thisIndex);
            CkExit();
    }
    else{
        myValues = valuestmp;
        memcpy(myValues+elementos,valuesN,(elementosN)*sizeof(int));
    }
    elementos += elementosN;
    if(ordenar){
        sort(0,elementos-1);
    }
    check();
}

void Merge::check(){
    phase--;
    cantFases--;
    if(phase > 0){
        if(activo){
            if(cantFases > 0){
                startCompare(comparar[cantFases-1]);
            }
            else if(indexLlamoIzq >= 0){
                activo = false;
                thisProxy[indexLlamoIzq].startCompare(thisIndex);
                indexLlamoIzq = -1;
            }
            else{
                activo = false;
            }
        }
    }
    else{
        if(thisIndex==0){
            mainProxy.terminar(elementos,myValues);
            // CkPrintf("\n=============================================================================================\n");
            // CkPrintf("====================================== FIN DEL PROGRAMA =====================================\n");
            // CkPrintf("=============================================================================================\n");
        }
	    activo = false;
    }
}



void Merge::merging(int low, int mid, int high) {
    int l1, l2, i;
    int b[elementos];

    for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
        if(myValues[l1] <= myValues[l2]){
            b[i] = myValues[l1++];
        }else{
            b[i] = myValues[l2++];
        }
    }

    while(l1 <= mid){
        b[i++] = myValues[l1++];
    }

    while(l2 <= high){
        b[i++] = myValues[l2++];
    }

    for(i = low; i <= high; i++){
        myValues[i] = b[i];
    }
}

void Merge::sort(int low, int high) {
    int mid;
    if(low < high) {
        mid = (low + high) / 2;
        sort(low, mid);
        sort(mid+1, high);
        merging(low, mid, high);
    }else {
      return;
    }
}

#include "merge.def.h"
