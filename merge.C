 /*
Tenemos un array de N chares. Cada chare tiene en su interior un vector de M valores.
Dentro de cada chare, el vector de M valores se ordena por quick sort.
Una vez ordenado, el chare X le dice al chare X+1 que le mande su vector de M valores.
El chare X recibe el array de X+1 y comienza a comparar sus valores de la forma que lo hace Merge, es decir:
    compara el x[0] con x+1[0].
        Si x[0] > x+1[0], guarda x+1[0] en un array auxiliar y compara x[0] con x+1[1].
        Si no, guarda x[0] en un array auxiliar y comparar x[1] con x+1[1].

COMPARACION
*/

#include "merge.decl.h"
#include "merge.h"
#include "main.decl.h"
#include <stdlib.h>
#include <time.h>
#include <malloc.h>

extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ int cantChares;
extern /* readonly */ int numElements;

Merge::Merge() {
    phase = 0;
    cantFases = 0;
    activo = false;
    posicion = -1;
    posicionDer = -1;
    indexLlamoIzq = -1;
    elementos = -1;
    posicionAnterior = -1;
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) { }

void Merge::initPhase(int pos, int posDer, int phaseN, int values[],int tam) {
    elementos = tam;
    // CkPrintf("\n[%d]initPhase TAM: %d       pos=%d, posDerINI=%d\n",thisIndex,elementos,pos,posDer);
    // for(int i=0;i<elementos;i++)
    //     CkPrintf("[%d] NoOrdenado[%d]: %d\n",thisIndex,i,myValues[i]);
    int *valuesIzq = (int *)malloc(sizeof(int)*(elementos/2));
    int *valuesDer = (int *)malloc(sizeof(int)*(elementos-elementos/2));
    memcpy(valuesIzq,values,(elementos/2)*sizeof(int));
    memcpy(valuesDer,values+elementos/2,(elementos-elementos/2)*sizeof(int));
    phase = phaseN + 1;
    // CkPrintf("[%d]initPhase con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);

    // Si soy un izquierdo, incremento cantFases, que es la cantidad de consultas a requestSwap que tiene que hacer este hilo
    if(posDer!=-1){
      // CkPrintf("[%d]initPhase ELSE IF0 con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
      cantFases++;
    }
    nuevaCantChares = pos-thisIndex+1;
    nuevaPos = nuevaCantChares/2-1+thisIndex;
    if(nuevaCantChares > 1 || phase == 1){
      posicion = pos;
      posicionDer = posDer;
      // CkPrintf("[%d]initPhase SE CAMBIO POSICIONES pos=%d, posDer=%d, posicion=%d, posicionDer=%d, phase=%d, cantFases=%d, tam=%d, elementos=%d\n",thisIndex,pos,posDer,posicion,posicionDer,phase,cantFases,tam,elementos);
    }
    if(nuevaCantChares > 2){
            CkPrintf("[%d]initPhase Divide con nuevaPos=%d, pos=%d, phase=%d, elementos/2=%d\n",thisIndex,nuevaPos,pos,phase,elementos/2);
            thisProxy[thisIndex].initPhase(nuevaPos,pos,phase,valuesIzq,elementos/2);
            CkPrintf("[%d]initPhase Divide con pos=%d, -1, phase=%d, elementos-elementos/2=%d\n",thisIndex,pos,phase,elementos-elementos/2);
            thisProxy[nuevaPos+1].initPhase(pos,-1,phase,valuesDer,elementos-elementos/2);
            free(valuesIzq);
            free(valuesDer);
    }
    else{
            // Solo entran acá los hilos que deben estar activos e iniciar una comparación
            CkPrintf("[%d]initPhase ELSE0 con pos=%d, posDer=%d, posicion=%d, posicionDer=%d, phase=%d, cantFases=%d, tam=%d, elementos=%d\n",thisIndex,pos,posDer,posicion,posicionDer,phase,cantFases,tam,elementos);
            // if(posDer!=-1){
            //   CkPrintf("[%d]initPhase ELSE IF1 con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
            // }
            if(thisIndex != pos){
              // CkPrintf("[%d]initPhase ELSE IF2 phase++ y copy con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
              phase++;
              cantFases++;
            }

            myValues = (int *)malloc(sizeof(int)*(elementos));
            memcpy(myValues,values,(elementos)*sizeof(int));

            if(nuevaCantChares > 1){
                // CkPrintf("[%d]initPhase ELSE IF3 setPhase con pos=%d, phase=%d, tam=%d, elementos=%d\n",thisIndex,pos,phase,tam,elementos);
                thisProxy[thisIndex+1].setPhase(phase,valuesDer,elementos-elementos/2);
                elementos = elementos/2;
            }else{
                // CkPrintf("[%d]initPhase ELSE IF4 setPhase con pos=%d, phase=%d, tam=%d, elementos=%d\n",thisIndex,pos,phase,tam,elementos);
            }

            activo = true;
            phase--;
            sort(0,elementos-1,myValues);
            phase++;
            // CkPrintf("[%d]initPhase comienza con phase=%d, pos=%d, posDer=%d, posicion=%d, posicionDer=%d\n",thisIndex,phase,pos,posDer,posicion,posicionDer);
            // for(int i = 0; i < elementos; i++)
            //   CkPrintf("[%d]initPhase valorX[%d]=%d\n",thisIndex,i,myValues[i]);
            startCompare(thisIndex+1, posicion);
    }
}

void Merge::setPhase(int phaseN, int values[], int tam){
    // CkPrintf("[%d]initPhase ELSE0 con pos=%d, posDer=%d, posicion=%d, posicionDer=%d, phase=%d, cantFases=%d, tam=%d, elementos=%d\n",thisIndex,pos,posDer,posicion,posicionDer,phase,cantFases,tam,elementos);
    phase = phaseN;
    elementos = tam;
    // CkPrintf("[%d]setPhase tam=%d, memoria=%d\n",thisIndex,tam,(elementos));
    myValues = (int *)malloc(sizeof(int)*(elementos));
    memcpy(myValues,values,(elementos)*sizeof(int));
    // for(int i = 0; i < elementos; i++)
    // CkPrintf("[%d]setPhase >valor[%d]=%d\n",thisIndex,i,myValues[i]);
}

void Merge::startCompare(int indexDer, int posicionN){
    // CkPrintf("[%d]startCompare con [%d], phase=%d, cantFases=%d, posicion=%d, posicionDer=%d ",thisIndex,indexDer,phase,cantFases,posicion,posicionDer);
    // CkPrintf("\telementos=%d, activo=%d\n",elementos,activo);
    posicion = posicionN;
    CkPrintf("[%d]startCompare Comparando con [%d]    (en phase=%d) OK%d\n",thisIndex,indexDer,phase,elementos-1);
    thisProxy[indexDer].requestSwap(phase,thisIndex,myValues[elementos-1]);
}

void Merge::requestSwap(int phaseN, int indexIzq,int lastValueN){
    CkPrintf("[%d]requestSwap (phase=%d)\n",thisIndex,phase);
    if(myValues == NULL)  CkPrintf("[%d]requestSwap  ALERTA ************************************\n",thisIndex);
    if((phase == phaseN || !activo) && myValues != NULL){
        CkPrintf("[%d]requestSwap %d  <-CMP-> [%d] %d\n",thisIndex,myValues[0],indexIzq,lastValueN);
        if(lastValueN > myValues[0]){
            CkPrintf("\t\t[%d]requestSwap ACEPTA. Llama a Chare [%d]\n",thisIndex,indexIzq);
            thisProxy[indexIzq].saveValue(myValues,elementos,true); //2: indice derecho que se modificó cuando acepto
            // thisProxy[indexIzq].acceptSwap(thisIndex+1,myValues,elementos); //2: indice derecho que se modificó cuando acepto

        }else{
            CkPrintf("\t\t\t\t\t[%d]requestSwap le DENIEGA a [%d].\n\n",thisIndex,indexIzq);
            thisProxy[indexIzq].saveValue(myValues,elementos,false); //2: indice derecho que no se modificó
            // thisProxy[indexIzq].denySwap(thisIndex,myValues,elementos); //2: indice derecho que no se modificó
        }
    }
    // Marcar que me llamó, para que vuelva a llamar cuando yo le avise que finalicé esta fase.
    else{
        CkPrintf("\t\t\t\t\t[%d]requestSwap phase=%d No estoy en misma fase que [%d] phase=%d o estoy activo=%d.          \n\n",thisIndex,phase,indexIzq,phaseN,activo);
        indexLlamoIzq = indexIzq;
    }
}

void Merge::saveValue(int valuesN[], int elementosN, bool ordenar){

    // CkPrintf("[%d]saveValue ::::::: inicio saveValue :::::::\n",thisIndex);
    // CkPrintf("[%d]saveValue ::::::: inicio saveValue :::::::elementos=%d, elementosN=%d\n",thisIndex,elementos, elementosN);
    myValues = (int *)realloc(myValues,(elementos+elementosN)*sizeof(int));
    memcpy(myValues+elementos,valuesN,(elementosN)*sizeof(int));
    elementos += elementosN;
    if(ordenar){
        // CkPrintf("[%d]saveValue debe ordenar elementos\n",thisIndex);
        sort(0,elementos-1,myValues);
    }
    for(int i = 0; i < elementos; i++){
        // CkPrintf("[%d]saveValue [%d]=%d\n",thisIndex,i,myValues[i]);
    }

    // CkPrintf("[%d]saveValue :::::::::: fin saveValue :::::::::::\n",thisIndex);

    check();
}

void Merge::check(){
    phase--;
    yaActualizoPosicion = false;
    cantFases--;
    CkPrintf("[%d]check\t\t LLEGUE A FASE %d y cantFases=%d\n",thisIndex,phase,cantFases);
    if(phase > 0){
        if(activo){
            if(indexLlamoIzq >= 0){
                activo = false;
                // CkPrintf("[%d]checkIzq ---> Se desactivó en check Izq. Avisar a [%d] que me puede llamar.\n",thisIndex,indexLlamoIzq);
                thisProxy[indexLlamoIzq].reiniciar(false);
                indexLlamoIzq = -1;
            }
            else if(cantFases > 0){
                // CkPrintf("[%d]checkSig ---> Comienza Siguiente Fase: %d.\n",thisIndex,phase);
                reiniciar(true);
            }
            else{
                // CkPrintf("[%d]checkDes ---> Se desactivó definitivamente.\n",thisIndex);
                activo = false;
            }
        }
    }
    else{
        // if(thisIndex==0 && elementos==numElements)
            mainProxy.terminar(myValues);
	    activo = false;
        CkPrintf("\n=============================================================================================\n");
        CkPrintf("[%d]=================================== FIN DEL PROGRAMA =====================================\n",thisIndex);
        CkPrintf("=============================================================================================\n");
    }
}

void Merge::reiniciar(bool meLlamoYo){
    CkPrintf("\n\n\n[%d] > reiniciar \n",thisIndex);
    if(meLlamoYo){
        CkPrintf("[%d]reiniciar Cambia su posicion de %d a %d\n",thisIndex,posicion,posicionDer);
        posicionAnterior = posicion;
        posicion = posicionDer;
        yaActualizoPosicion = true;
        if(cantFases > 1){
                CkPrintf("[%d]reiniciarA solicitando posicion a %d   cantFases=%d\n",thisIndex,posicion+1,cantFases);
                thisProxy[posicion+1].solicitarPosicion(thisIndex,phase);
        }
        else{
            CkPrintf("[%d]reiniciarB4 ---> startCompare, indice %d, cantFases=%d elementos=%d\n",thisIndex,posicionAnterior+1,cantFases,elementos);
            startCompare(posicionAnterior+1,posicion);
        }
    }
    else{
        if(posicionAnterior > -1){
          CkPrintf("[%d]reiniciarC ---> startCompare ELSE1 con posicion %d\n",thisIndex,posicionAnterior+1);
          startCompare(posicionAnterior+1,posicion);
        }
        else{
          CkPrintf("[%d]reiniciarD ---> startCompare ELSE2 con posicion %d\n",thisIndex,thisIndex+1);
          startCompare(thisIndex+1,posicion);
        }
    }
    CkPrintf("[%d] < reiniciar \n\n\n",thisIndex);
}

void Merge::solicitarPosicion(int indexN, int phaseN){
    if(phaseN == phase && yaActualizoPosicion){
        CkPrintf("[%d]solicitarPosicionDerA phase=%d,  le envia (%d)  [%d] phaseN=%d  IngresoA\n",thisIndex,phase,posicion,indexN,phaseN);
        thisProxy[indexN].cambiarPosicionDer(posicion);
    }
    else{
        CkPrintf("[%d]solicitarPosicionDerB phase=%d,  le envia (%d)   [%d] phaseN=%d  IngresoB\n",thisIndex,phase,posicionDer,indexN,phaseN);
        thisProxy[indexN].cambiarPosicionDer(posicionDer);
    }
}

void Merge::cambiarPosicionDer(int posicionN){
      CkPrintf("[%d]cambiarPosicionDerA Cambia posicionDer de %d a %d\n",thisIndex,posicionDer,posicionN);
      posicionDer = posicionN;
      CkPrintf("[%d]cambiarPosicionDerA ---> startCompare a %d\n",thisIndex,posicionAnterior+1);
      startCompare(posicionAnterior+1,posicion);
}

void Merge::imprimir(char* prefix) {
    prefix[strlen(prefix)-1]='\0';
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d ) \n",prefix,thisIndex, 666, 999, posicion, posicionDer,phase);
}

void Merge::displayValue(int prefixLen, char* prefix) {
    prefix[prefixLen] = '\0';
    if (thisIndex < cantChares - 1) {
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) \n"/*<<<\t\tt %d (p %d --- pd %d --- ph %d )\n"*/,prefix,thisIndex, 666);//posicion, posicionDer,phase);
        thisProxy[thisIndex + 1].displayValue(prefixLen, prefix);
    } else {
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) \n"/*"<<<\t\tt %d (p %d --- pd %d --- ph %d )\n"*/,prefix,thisIndex, 666);//posicion, posicionDer,phase);
        mainProxy.arrayDisplayFinished();
    }
}

void Merge::merging(int low, int mid, int high, int a[]) {
   int l1, l2, i;

   int b[elementos];

   for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
      if(a[l1] <= a[l2])
         b[i] = a[l1++];
      else
         b[i] = a[l2++];
   }

   while(l1 <= mid)
      b[i++] = a[l1++];

   while(l2 <= high)
      b[i++] = a[l2++];

   for(i = low; i <= high; i++){
      a[i] = b[i];
    }

}

void Merge::sort(int low, int high,int a[]) {
    int mid;
    if(low < high) {
        mid = (low + high) / 2;
        sort(low, mid,a);
        sort(mid+1, high,a);
        merging(low, mid, high,a);
   } else {
      return;
   }

}

void Merge::bubbleSort(int list[],int tam) {
   int temp;
   int i,j;
   bool swapped = false;
   // loop through all numbers
   for(i = 0; i < numElements*tam-1; i++) {
      swapped = false;
      // loop through numbers falling ahead
      for(j = 0; j < numElements*tam-1-i; j++) {
         if(list[j] > list[j+1]) {
            temp = list[j];
            list[j] = list[j+1];
            list[j+1] = temp;
            swapped = true;
         }
      }
      // if no number was swapped that means
      //   array is sorted now, break the loop.
      if(!swapped) {
         break;
      }
   }
}

#include "merge.def.h"
