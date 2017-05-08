 /*
Tenemos un array de N chares. Cada chare tiene en su interior un vector de M valores.
Dentro de cada chare, el vector de M valores se ordena por quick sort.
Una vez ordenado, el chare X le dice al chare X+1 que le mande su vector de M valores.
El chare X recibe el array de X+1 y comienza a comparar sus valores de la forma que lo hace Merge, es decir:
    compara el x[0] con x+1[0].
        Si x[0] > x+1[0], guarda x+1[0] en un array auxiliar y compara x[0] con x+1[1].
        Si no, guarda x[0] en un array auxiliar y comparar x[1] con x+1[1].

VER MERGE SECUENCIAL
AGREGAR MULTIVALORES
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
    tempos = (int *)malloc(sizeof(int)*numElements);
    numElementsLocal = numElements;
    myValue = thisIndex; //Ascendente
    myValue =  rand() % 100; //Aleatorio
    // for(int i=0;i<numElementsLocal;i++){
    //     myValue = (numElementsLocal)*(numElementsLocal-thisIndex)-i; //Descendente
    //     myValues[i] = myValue;
    // }
    tempo = myValue;
    phase = 0;
    activo = false;
    indexSave = thisIndex;
    posicion = -1;
    posicionDer = -1;
    indexLlamoIzq = -1;
    primero = -1;
    cantFases = 0;
    elementos = -1;
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) { }

void Merge::initPhase(int pos, int posDer, int phaseN, int values[],int tam) {


    elementos = tam;
    CkPrintf("\n[%d]initPhase TAM: %d       pos=%d, posDerINI=%d\n",thisIndex,elementos,pos,posDer);
    // for(int i=0;i<elementos;i++)
    //     CkPrintf("[%d] NoOrdenado[%d]: %d\n",thisIndex,i,myValues[i]);
    // int valuesIzq[elementos/2];
    // int valuesDer[elementos-elementos/2];
    int *valuesIzq = (int *)malloc(sizeof(int)*(elementos/2));
    int *valuesDer = (int *)malloc(sizeof(int)*(elementos-elementos/2));

    memcpy(valuesIzq,values,(elementos/2)*sizeof(int));
    memcpy(valuesDer,values+elementos/2,(elementos-elementos/2)*sizeof(int));
    primero = thisIndex;
    phase = phaseN + 1;
    CkPrintf("[%d]initPhase con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);

    nuevaCantChares = pos-thisIndex+1;
    nuevaPos = nuevaCantChares/2-1+thisIndex;
    if(nuevaCantChares > 2){
            posicion = pos;
            posicionDer = posDer;
            CkPrintf("[%d]initPhase Divide con nuevaPos=%d, pos=%d, phase=%d, elementos/2=%d\n",thisIndex,nuevaPos,pos,phase,elementos/2);
            thisProxy[thisIndex].initPhase(nuevaPos,pos,phase,valuesIzq,elementos/2);
            CkPrintf("[%d]initPhase Divide con pos=%d, -1, phase=%d, elementos-elementos/2=%d\n",thisIndex,pos,phase,elementos-elementos/2);
            thisProxy[nuevaPos+1].initPhase(pos,-1,phase,valuesDer,elementos-elementos/2);
    }
    else{
            // Solo entran acá los hilos que deben estar activos e iniciar una comparación
            CkPrintf("[%d]initPhase ELSE con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
            if(posDer!=-1){
              cantFases++;
            }
            if(thisIndex != pos){
              CkPrintf("[%d]initPhase ELSE IF2 phase++ y copy con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
              phase++;
              cantFases++;
            }
            // if(nuevaCantChares == 1){
                            posicion = pos; //BORRARLO
                            posicionDer = posDer; // VER SI VA ACA
            // }
            // if(posicionDer == -1)
            //   phase++;
            // if(nuevaCantChares == 1 && soyIzq){
            //   CkPrintf("[%d]initPhase nuevaCantChares=1,      con pos=%d, posDer=%d\n",thisIndex,pos,posDer);
            //   // posicion = posicionDer;
            //   posicionDer = posicion+1;
            // }
            // CkPrintf("[%d]setValues tam=%d 999\n",thisIndex,elementos);
            if(elementos>1)
              elementos = elementos/2;
            // CkPrintf("[%d]setValues tam=%d 9992\n",thisIndex,elementos);
            myValues = (int *)malloc(sizeof(int)*(elementos));
            memcpy(myValues,values,(elementos)*sizeof(int));
            for(int i = 0; i < elementos; i++)
              CkPrintf("[%d] valor[%d]=%d 999\n",thisIndex,i,myValues[i]);
            // myValues = (int *)malloc(sizeof(int)*elementos);
            // memcpy(myValues,values,(elementos)*sizeof(int));        //Se copian los valores en variable local

            if(nuevaCantChares>1){
                CkPrintf("[%d]initPhase ELSE IF3 setPhase con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
                thisProxy[thisIndex+1].setPhase(phase,valuesDer,elementos-elementos/2);
            }else{
                CkPrintf("[%d]initPhase ELSE IF4 setPhase con pos=%d, phase=%d, tam=%d\n",thisIndex,pos,phase,elementos);
                // thisProxy[thisIndex+1].setValues(valuesDer,elementos-elementos/2);
            }

            activo = true;
            sort(0,elementos-1,myValues);
            CkPrintf("[%d]initPhase comienza con phase=%d, pos=%d, posDerSC1=%d\n",thisIndex,phase,pos,posDer);
            CkPrintf("[%d]initPhase comienza con phase=%d, posicion=%d, posicionDerSC2=%d\n",thisIndex,phase,posicion,posicionDer);
            startCompare(thisIndex+1,indexSave, true, posicion, primero);
            //     phase--;
            //     sort(0,elementos-1,myValues);
            //     phase++;
    }
}

void Merge::setValues(int values[], int tam){
    CkPrintf("[%d]setValues tam=%d\n",thisIndex,tam);
    elementos = tam;
    myValues = (int *)malloc(sizeof(int)*(elementos-elementos/2));
    memcpy(myValues,values+elementos/2,(elementos-elementos/2)*sizeof(int));
    for(int i = 0; i < elementos; i++)
    CkPrintf("[%d] >valor[%d]=%d\n",thisIndex,i,myValues[i]);
}

void Merge::setPhase(int phaseN, int values[], int tam){
    // activo = false;         // VER SI VA, no haria falta, por el true del init y el false del constructor
    phase = phaseN;
    elementos = tam;
    myValues = (int *)malloc(sizeof(int)*(elementos-elementos/2));
    memcpy(myValues,values+elementos/2,(elementos-elementos/2)*sizeof(int));
    for(int i = 0; i < elementos; i++)
    CkPrintf("[%d] >valor[%d]=%d\n",thisIndex,i,myValues[i]);
    // myValues = (int *)malloc(sizeof(int)*tam);
    // memcpy(myValues,values,(tam)*sizeof(int));        //Se copian los valores en variable local
}

void Merge::startCompare(int indexDer, int indexS, bool seMovioIndexDer, int posicionN, int primeroN){
    CkPrintf("[%d]startCompare con [%d], phase=%d, posicion=%d, posicionDer=%d ",thisIndex,indexDer,phase,posicion,posicionDer);
    CkPrintf("elementos=%d, activo=%d iii\n",elementos,activo);
    indexSave = indexS;
    posicion = posicionN;
    primero = primeroN;
    CkPrintf("[%d]startCompare Comparando con [%d]    (en phase=%d) OK%d\n",thisIndex,indexDer,phase,elementos-1);
    thisProxy[indexDer].requestSwap(phase,tempo,thisIndex,myValues[elementos-1]);
}

void Merge::startComparePhase(int indexDer, int indexS, bool seMovioIndexDer, int posicionN, int primeroN, int phaseN){
    phase = phaseN;
    startCompare(indexDer,indexS,seMovioIndexDer,posicionN,primeroN);
}

void Merge::requestSwap(int phaseN, int valueN, int indexIzq,int lastValueN){
    CkPrintf("[%d]requestSwap (phase=%d)\n",thisIndex,phase);
    if(myValues == NULL)  CkPrintf("[%d]requestSwap  ALERTA ************************************\n",thisIndex);
    if((phase == phaseN || !activo) /*&& myValues != NULL*/){
        CkPrintf("[%d]requestSwap %d  <-CMP-> [%d] %d\n",thisIndex,myValues[0],indexIzq,lastValueN);
        if(lastValueN > myValues[0]){
            // phase = phaseN-1;
            // CkPrintf("\t\t\t\t\t[%d] le ACEPTA a [%d].\n\n",thisIndex,indexIzq);
            CkPrintf("\t\t[%d]requestSwap ACEPTA. Llama a Chare [%d]\n",thisIndex,indexIzq);
            // thisProxy[indexIzq].compareData(values,thisIndex+1); //Le paso los datos

            thisProxy[indexIzq].acceptSwap(myValue,thisIndex+1,myValues,elementos); //2: indice derecho que se modificó cuando acepto

        }else{                           //TEMPO?
            CkPrintf("\t\t\t\t\t[%d]requestSwap le DENIEGA a [%d].\n\n",thisIndex,indexIzq);
            // myValue = valueN;
            // CkPrintf("\t\t[%d] DENIEGA. Llama a Chare [%d]\n",thisIndex,indexIzq);
            thisProxy[indexIzq].denySwap(valueN,thisIndex,myValues,elementos); //2: indice derecho que no se modificó
        }
    }else{
        CkPrintf("\t\t\t\t\t[%d]requestSwap phase=%d No estoy en misma fase que [%d] phase=%d o estoy activo=%d.          \n\n",thisIndex,phase,indexIzq,phaseN,activo);
        // Marcar que me llamó para que vuelva a llamar cuando yo le avise que finalicé esta fase.
        indexLlamoIzq = indexIzq;
    }
}

// Acepta y se mueve el Der
void Merge::acceptSwap(int valueN, int indexDer,int valuesN[],int elementosN){
    // Guardar el valor de la derecha en el indice de guardado
    saveValue(valuesN,elementosN,true); //era false

    check(indexDer);
    // }
}

void Merge::check(int indexDer){
    // CkPrintf("Cambio de fase abajo\n");
    phase--;
    cantFases--;
    // tempo=myValue;
    // CkPrintf("[%d]\t\t LLEGUE A FASE CERO VIEJAAA 1503\n",thisIndex);
    // thisProxy[indexDer-1].setPhase(phase); // Le cambio la fase
    // imprimir("NF");
    CkPrintf("[%d]check\n",thisIndex);
    if(phase > 0){
        if(activo){
            if(indexLlamoIzq >= 0){
                activo = false;
                CkPrintf("[%d]check ---> Se desactivó en check Izq. Avisar a [%d] que me puede llamar.\n",thisIndex,indexLlamoIzq);
                thisProxy[indexLlamoIzq].cambiarPosicion(thisIndex,false);
                // thisProxy[indexLlamoIzq].startCompare(thisIndex,indexLlamoIzq,false);
                indexLlamoIzq = -1;
            }
            else if(cantFases > 0){
                // else if(posicionDer > 0 && activo){
                CkPrintf("[%d]check ---> Comienza Siguiente Fase: %d.\n",thisIndex,phase);
                cambiarPosicion(indexDer,true);
                // cambiarPosicion(indexDer,true);
            }
            else{
                CkPrintf("[%d]check ---> Se desactivó definitivamente.\n",thisIndex);
                activo = false;
                for(int i = 0; i < elementos; i++){
                    CkPrintf("[%d]saveValue [%d]=%d\n",thisIndex,i,myValues[i]);
                }
            }
        }
    }
    else{
        // if(thisIndex==0 && elementos==numElements)
            mainProxy.terminar(myValues);
	    activo = false;
        CkPrintf("\n=============================================================================================\n");
        CkPrintf("[%d] =================================== FIN DEL PROGRAMA ====================================\n",thisIndex);
        CkPrintf("  =============================================================================================\n");
    }
}

void Merge::saveValue(int valuesN[], int elementosN, bool ordenar){

    CkPrintf("[%d]saveValue ::::::: inicio saveValue :::::::elementos=%d, elementosN=%d\n",thisIndex,elementos, elementosN);
    CkPrintf("[%d]saveValue sizeof: %d\n",(elementosN)*sizeof(int));
    // if(myValues==NULL) CkPrintf("OTRO ERROR\n");
    myValues = (int *)realloc(myValues,(elementos+elementosN)*sizeof(int));
    // if( ( (int *)realloc(myValues,elementos+elementosN) )==NULL);  CkPrintf("ERROR\n");
    // myValues = (int *)malloc(sizeof(int)*(elementos+elementosN));
    memcpy(myValues+elementos,valuesN,(elementosN)*sizeof(int));
    // for(int i=elementos,j=0;i<elementos+elementosN, j<elementosN;i++,j++)
    //     myValues[i] = valuesN[j];
    elementos += elementosN;
    if(ordenar){
        CkPrintf("[%d]saveValue debe ordenar elementos\n",thisIndex);
        sort(0,elementos-1,myValues);
    }
    CkPrintf("[%d]saveValue --- ya ordenó los elementos ---\n",thisIndex);
    for(int i = 0; i < elementos; i++){
        CkPrintf("[%d]saveValue [%d]=%d\n",thisIndex,i,myValues[i]);
    }

    CkPrintf("[%d]saveValue :::::::::: fin saveValue :::::::::::\n",thisIndex);

}
// void Merge::saveTempo(int indexSave,int valuesN[]){
//     // CkPrintf("[%d] Cambiando TEMPO valor de [%d] a v%d,t%d.\n",thisIndex,indexSave,myValue,tempo);
//     thisProxy[indexSave].saveValue(tempo,false,valuesN,elementos); //DEFAULT
// }

// Deniega y Se mueve el Índice Izquierdo
void Merge::denySwap(int value, int indexDer,int valuesN[], int elementosN){
    // CkPrintf("[%d] en denySwap con indexSave=%d, y con primero=%d\n",thisIndex,indexSave,primero);
    saveValue(valuesN,elementosN,false);
    CkPrintf("[%d]denySwap en deny despues de swap\n",thisIndex);
    check(indexDer);
}

void Merge::cambiarPosicion(int indexDer, bool meLlamoYo){
    CkPrintf("[%d]cambiarPosicion\n",thisIndex);
    if(meLlamoYo){
      llameYo = true;
        CkPrintf("[%d]cambiarPosicion Cambia su posicion %d a %d-------------------------------------------------------------****\n",thisIndex,posicion,posicionDer);
        // posicion = posicionDer;
    }else
      llameYo = false;

    // CkPrintf("[%d] cambiarPosicion------------888-------posicion+1 %d------------posicionDer+1=%d y indexDer=%d-----------------------------------------****\n",thisIndex,posicion+1,posicionDer+1,indexDer);
    if(posicion>-1 && posicion<cantChares-1){
        CkPrintf("[%d]cambiarPosicion va a solicitarPosicionDer a [%d]\n",thisIndex,posicion+1);
        thisProxy[posicion+1].solicitarPosicionDer(thisIndex,indexDer,phase);
        CkPrintf("[%d]cambiarPosicion va a solicitarPosicionDer a [%d]\n",thisIndex,posicion+1);
    }
    else{
        CkPrintf("[%d]cambiarPosicion cambiarPosicion  posicion+1=%d < cantChares=%d\n",thisIndex,posicion+1,cantChares);
        if(indexDer < cantChares)
            startCompare(indexDer,thisIndex,false,posicion,primero);
    }
}

void Merge::solicitarPosicionDer(int indexN, int indexDer, int phaseN){
    CkPrintf("[%d]solicitarPosicionDer phase=%d,    [%d] phaseN=%d  Ingreso\n",thisIndex,phase,indexN,phaseN);

    if(phaseN == phase){
        //CkPrintf("[%d] %d < %d ? ++++++++++++++++p%d cantChares%d indexN%d p%d +++++++++++++++  Env\n",thisIndex,posicion,(cantChares-indexN)/(phase-1),posicion,cantChares,indexN,phase-1);
        thisProxy[indexN].cambiarPosicionDer(posicion,indexDer);
    }
    else
        thisProxy[indexN].cambiarPosicionDer(posicionDer,indexDer);
    // imprimir(str);
}

void Merge::cambiarPosicionDer(int posicionDerN, int indexDer){
    posicionDer = posicionDerN;
    CkPrintf("[%d]cambiarPosicionDer Cambia posicionDer=%d\n",thisIndex,posicionDer);
    CkPrintf("[%d]cambiarPosicionDer cambiarPosicionDer  posicion+1=%d < cantChares=%d\n",thisIndex,posicion+1,cantChares);
    int posicionAuxiliar = posicion + 1;
    if(posicion+1 < cantChares){
        if(llameYo)
          posicion = posicionDer;

        startCompare(posicionAuxiliar,thisIndex,true,posicion,primero);
    }
}

// void Merge::cambiarPosicion(int indexDer, bool meLlamoYo){
//     CkPrintf("[%d] cambiarposicion\n",thisIndex);
//     if(meLlamoYo){
//         CkPrintf("[%d] Cambia su posicion %d a %d-------------------------------------------------------------****\n",thisIndex,posicion,posicionDer);
//         posicion = posicionDer;
//     }
//
//     // CkPrintf("[%d] cambiarPosicion------------888-------posicion+1 %d------------posicionDer+1=%d y indexDer=%d-----------------------------985-------------****\n",thisIndex,posicion+1,posicionDer+1,indexDer);
//     if(posicion>-1 && posicion<cantChares-1){
//         CkPrintf("[%d] va a solicitarPosicionDer a [%d], proceso=%d\n",thisIndex,posicion+1,getpid());
//         thisProxy[posicion+1].solicitarPosicionDer(thisIndex,indexDer,phase);
//         CkPrintf("[%d] va a solicitarPosicionDer a [%d], proceso=%d\n",thisIndex,posicion+1,getpid());
//     }
//     else{
//         CkPrintf("[%d] cambiarPosicion  posicion+1=%d < cantChares=%d\n",thisIndex,posicion+1,cantChares);
//         if(indexDer < cantChares)
//             startCompare(indexDer,thisIndex,false,posicion,primero);
//     }
// }
//
// void Merge::solicitarPosicionDer(int indexN, int indexDer, int phaseN){
//     CkPrintf("Entrando a solicitarPosicionDer::: [%d] phase=%d,    [%d] phaseN=%d   9867 Ingreso\n",thisIndex,phase,indexN,phaseN);
//
//     if(phaseN == phase){
//         //CkPrintf("[%d] %d < %d ? ++++++++++++++++p%d cantChares%d indexN%d p%d +++++++++++++++ 986A Env\n",thisIndex,posicion,(cantChares-indexN)/(phase-1),posicion,cantChares,indexN,phase-1);
//         thisProxy[indexN].cambiarPosicionDer(posicion,indexDer);
//     }
//     else
//         thisProxy[indexN].cambiarPosicionDer(posicionDer,indexDer);
//     // imprimir(str);
// }
//
// void Merge::cambiarPosicionDer(int posicionDerN, int indexDer){
//     posicionDer = posicionDerN;
//     CkPrintf("[%d] Cambia posicionDer=%d      9867\n",thisIndex,posicionDer);
//     CkPrintf("[%d] cambiarPosicionDer  posicion+1=%d < cantChares=%d\n",thisIndex,posicion+1,cantChares);
//     if(posicion+1 < cantChares)
//         startCompare(posicion+1,thisIndex,true,posicion,primero);
// }

void Merge::imprimir(char* prefix) {
    // if (thisIndex < cantChares - 1) {
    prefix[strlen(prefix)-1]='\0';
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d ) \n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
        // thisProxy[thisIndex + 1].imprimir(prefix);
    //  else {
        // CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
    // }
}

// Display the value
void Merge::displayValue(int prefixLen, char* prefix) {
    prefix[prefixLen] = '\0';
    if (thisIndex < cantChares - 1) {
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) \n"/*<<<\t\tt %d (p %d --- pd %d --- ph %d )\n"*/,prefix,thisIndex, myValue);//, tempo, posicion, posicionDer,phase);
        thisProxy[thisIndex + 1].displayValue(prefixLen, prefix);
    } else {
        CkPrintf("%s Merge[%d] =\t\t>>> v(%d) \n"/*"<<<\t\tt %d (p %d --- pd %d --- ph %d )\n"*/,prefix,thisIndex, myValue);//, tempo, posicion, posicionDer,phase);
        mainProxy.arrayDisplayFinished();
    }
}

void Merge::merging(int low, int mid, int high, int a[]) {
   int l1, l2, i;
   // int a[4];
   // for(int j=0;j<4;j++)
   //      a[j]=values[j];
   int b[numElementsLocal];

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
      // tempos[i] = a[i];
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
