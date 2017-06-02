#include "merge.decl.h"
#include "merge.h"
#include "main.decl.h"
#include <stdlib.h>
#include <time.h>
// #include <malloc.h>

extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ int numElements;

Merge::Merge() {
        phase = 0;
        activo = false;
        ordenado = false;
        indexLlamoIzq = -1;
        valueLlamoIzq = -1;
        cantFases = 0;
        elementos = -1;
        tiempo = 0;
        myValues=NULL;
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) {
}

/*
   @param	lastChareIzq: índice del último elemento del subarray izquierdo
   @param  lastChareDer: índice del último elemento del subarray derecho
   @param  phaseN: número de fase del Chare padre
   @param  tam: cantidad de elementos que recibe del Chare padre
   @param  values[]: subconjunto de valores del Chare padre
   @param  proxIndex: índice del Chare con quien deberá comparar
   @name initPhase: Recibe un subconjunto de valores, los subdivide en dos partes
                  y le entrega la parte derecha a un nuevo Chare
 */
void Merge::initPhase(int lastChareIzq, int lastChareDer, int phaseN, int tam,int values[],int proxIndex) {
        // Si es un Chare izquierdo libera la mitad del espacio de memoria
        if(tam==0) {
                elementos = elementos / 2;
                myValues = (int *)realloc(myValues,(elementos)*sizeof(int));
        }
        // Si es un Chare derecho, crea un espacio de memoria para sus datos
        else{
                elementos = tam;
                myValues = (int *)malloc(sizeof(int)*elementos);
                if(myValues == NULL) {
                        CkPrintf("[%d] myValues es NULL\n",thisIndex);
                        CkExit();
                }
                memcpy(myValues,values,(elementos)*sizeof(int)); //Se copian los valores en variable local
        }
        int *valuesDer = (int *)malloc(sizeof(int)*(elementos-elementos/2));
        if(valuesDer == NULL) {
                CkPrintf("[%d] valuesDer es NULL\n",thisIndex);
                CkExit();
        }
        memcpy(valuesDer,myValues+elementos/2,(elementos-elementos/2)*sizeof(int));
        phase = phaseN;
        phase++;
        // Si es un Chare izquierdo, agregar índice del Chare a comparar
        if(lastChareDer!=-1) {
                comparar[cantFases]=proxIndex;
                cantFases++;
        }
        // cantidad de Chares que contiene el subarray actual
        cantChares = lastChareIzq-thisIndex+1;
        // nuevo índice del último elemento del subarray izquierdo
        newLastChareIzq = cantChares/2-1+thisIndex;
        if(cantChares < 2) {
                sort(0,elementos-1);
                ordenado = true;
                if(lastChareDer!=-1) {
                        activo = true;
                        thisProxy[comparar[cantFases-1]].requestSwap(phase,thisIndex,myValues[elementos-1]);
                }
        }
        else{
                thisProxy[thisIndex].initPhase(newLastChareIzq,lastChareIzq,phase,0,NULL,newLastChareIzq+1);
                thisProxy[newLastChareIzq+1].initPhase(lastChareIzq,-1,phase,elementos-elementos/2,valuesDer,-1);
                free(valuesDer);
                valuesDer=NULL;
        }
}

/*
   @param phaseN: número de fase del Chare llamante
   @param indexIzq: índice del Chare llamante
   @param lastValueN: elemento más grande del Chare llamante
   @name requestSwap: Comprueba que el Chare llamante y el Chare llamado estén
      en la misma fase y devuelve los valores al Chare llamante.
 */
void Merge::requestSwap(int phaseN,int indexIzq,int lastValueN){
        if((phase == phaseN || !activo) && ordenado) {
                // Si el último del Chare izquierdo es mayor que el primero del Chare derecho, saveValue debe ordenar
                if(lastValueN > myValues[0]) {
                        thisProxy[indexIzq].saveValue(myValues,elementos,true);
                        free(myValues);
                        myValues=NULL;
                }else{
                        thisProxy[indexIzq].saveValue(myValues,elementos,false);
                        free(myValues);
                        myValues=NULL;
                }
        }else{
                // Guardar índice y último valor del Chare izquierdo para retomar cuando finalice
                indexLlamoIzq = indexIzq;
                valueLlamoIzq = lastValueN;
        }
}

/*
   @param valuesN[]: conjunto de valores del Chare derecho
   @param elementosN: cantidad de valores de valuesN[]
   @param ordenar: indicador de si debe ordenar o no el nuevo conjunto de valores
   @name saveValue: recibe valores del Chare derecho y los agrega a su subconjunto
                de datos. Reordena en caso de ser necesario.
 */
void Merge::saveValue(int valuesN[], int elementosN, bool ordenar){
        int *valuestmp = (int *)realloc(myValues,(elementos+elementosN)*sizeof(int));
        myValues=NULL;
        if(valuestmp==NULL) {
                CkPrintf("[%d] valuestmp es NULL\n",thisIndex);
                CkExit();
        }
        else{
                myValues = valuestmp;
                memcpy(myValues+elementos,valuesN,(elementosN)*sizeof(int));
        }
        elementos += elementosN;
        if(ordenar) {
                if(elementos % 2 == 0)
                        merging(0,(elementos-1)/2,elementos-1);
                else
                        merging(0,(elementos-1)/2-1,elementos-1);
        }
        check();
}

/*
   @name check: Chequea si la cantidad de fases de Chare llego a cero:
            Si es mayor a 0 realiza nueva iteración
            Sino chequea si fue llamado por su izquierdo
            Sino finaliza ejecución
 */
void Merge::check(){
        phase--;
        cantFases--;
        if(cantFases > 0) {
                // Realiza siguiente comparación
                thisProxy[comparar[cantFases-1]].requestSwap(phase,thisIndex,myValues[elementos-1]);
        }
        else if(indexLlamoIzq >= 0) {
                // Si fue llamado por otro Chare, retoma la ejecución
                activo = false;
                requestSwap(phase,indexLlamoIzq,valueLlamoIzq);
                indexLlamoIzq = -1;
                valueLlamoIzq = -1;
        }
        else{
                // Si no finaliza la ejecución de este Chare
                activo = false;
                if(thisIndex==0) {
                        mainProxy.terminar(elementos,myValues);
                }
        }
}

/*
   @param low: índice izquierdo: posición del primer elemento izquierdo a comparar
   @param mid: índice derecho: posición del primer elemento derecho a comparar
   @param high: posición del último elemento
   @name merging: Compara dos subconjuntos de elementos y los une en arreglo ordenado
 */
void Merge::merging(int low, int mid, int high) {
        int l1, l2, i;
        int b[elementos];

        for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
                if(myValues[l1] <= myValues[l2]) {
                        b[i] = myValues[l1++];
                }else{
                        b[i] = myValues[l2++];
                }
        }

        while(l1 <= mid) {
                b[i++] = myValues[l1++];
        }

        while(l2 <= high) {
                b[i++] = myValues[l2++];
        }

        for(i = low; i <= high; i++) {
                myValues[i] = b[i];
        }
}

/*
   @param low: índice izquierdo: posición del primer elemento izquierdo a comparar
   @param high: posición del último elemento
   @name sort: inicia proceso de ordenamiento, dividiendo el conjunto de valores
            a la mitad de modo recursivo para luego ordenar y unir con merging()
 */
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
