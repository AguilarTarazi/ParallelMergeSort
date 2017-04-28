/*
  Falta fusionar con el Deny.
*/

#include "merge.decl.h"
#include "merge.h"
#include "main.decl.h"
#include <stdlib.h>
#include <time.h>
extern /* readonly */ CProxy_Main mainProxy;
extern /* readonly */ int numElements;

Merge::Merge() {
  myValue =  rand() % 100; //Aleatorio
  myValue = thisIndex; //Ascendente
  myValue = (numElements-1)-thisIndex; //Descendente
  if(thisIndex==0) myValue = 84;
  if(thisIndex==1) myValue = 28;
  if(thisIndex==2) myValue = 82;
  if(thisIndex==3) myValue = 32;
  tempo = myValue;
  phase = 0;
  activo = false;
  indexSave = thisIndex;
  posicion = -1;
  posicionDer = -1;
  indexLlamoIzq = -1;
  primero = -1;
  cantFases = 0;
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) { }

void Merge::initPhase(int pos, int posDer, int phaseN) {
  primero = thisIndex;
  phase = phaseN;
  phase++;
  if(posDer!=-1)
    cantFases++;
  activo = true;
  // CkPrintf("Chare [%d] con POS %d, phase = %d\n",thisIndex,pos,phase);
  newNumElements = pos-thisIndex+1;
  newPos = newNumElements/2-1+thisIndex;
  if(newNumElements < 3){
    if(thisIndex != pos){
      phase++;
      cantFases++;
    }
    // CkPrintf("Chare [%d] comienza con phase = %d, y pos %d\n",thisIndex,phase,pos);
    posicion = pos;
    posicionDer = posDer; // VER SI VA ACA
    if(newNumElements==1)
      posicion = posicionDer;
    if(newNumElements>1){
      thisProxy[thisIndex+1].setPhase(phase);
    }
    startCompare(thisIndex+1,indexSave, true, posicion, primero);
  }
  else{
    thisProxy[thisIndex].initPhase(newPos,pos,phase);
    thisProxy[newPos+1].initPhase(pos,-1,phase);
  }
}

void Merge::setPhase(int phaseN){
  phase = phaseN;
}

void Merge::startCompare(int indexDer, int indexS, bool seMovioIndexDer, int posicionN, int primeroN){
  CkPrintf("[%d] startCompare con indexDer=%d, phase=%d, posicion=%d, posicionDer=%d\n",thisIndex,indexDer,phase,posicion,posicionDer);
  //  imprimir(str);
  if(!seMovioIndexDer){
    // CkPrintf("[%d] tempo = myValue, %d<---%d\n",thisIndex,tempo,myValue);
    tempo = myValue;
  }
  // CkPrintf("\n>>>Comparando [%d]--->v%d,t%d con [%d]    (en phase=%d, posicion=%d, posicionDer=%d)\n",thisIndex,myValue,tempo,indexDer,phase,posicion,posicionDer);
  // CkPrintf("\n[%d] Comparando con [%d]    (en phase=%d) \n",thisIndex,indexDer,phase);
  indexSave = indexS;
  posicion = posicionN;
  primero = primeroN;
  thisProxy[indexDer].requestSwap(phase,tempo,thisIndex);
}

void Merge::requestSwap(int phaseN, int valueN, int indexIzq){
  CkPrintf("\t\t\t\t[%d]--->v%d:ph%d requestSwap con [%d]--->v%d:ph%d 983\n",indexIzq,valueN,phaseN,thisIndex,myValue,phase);
  // CkPrintf("Chare [%d] en requestSwap. phase:%d, phaseN:%d\n",thisIndex,phase,phaseN);
  if(phase == phaseN || !activo){
    // CkPrintf("Chare [%d] cambio fase. Comparando Valores %d y %d\n",thisIndex,valueN,myValue);
    if(valueN > myValue){
      phase = phaseN-1;
      CkPrintf("\t\t\t\t\t[%d] le ACEPTA a [%d].\n",thisIndex,indexIzq);
      // CkPrintf("\t\t[%d] ACEPTA. Llama a Chare [%d]\n",thisIndex,indexIzq);
      thisProxy[indexIzq].acceptSwap(myValue,thisIndex+1); //2: indice derecho que se modificó cuando acepto
    }else{                           //TEMPO?
      CkPrintf("\t\t\t\t\t[%d] le DENIEGA a [%d].\n",thisIndex,indexIzq);
      // myValue = valueN;
      // CkPrintf("\t\t[%d] DENIEGA. Llama a Chare [%d]\n",thisIndex,indexIzq);
      thisProxy[indexIzq].denySwap(valueN,thisIndex); //2: indice derecho que no se modificó
    }
  }else{
    CkPrintf("\t\t\t\t\t[%d] No estoy en misma fase que [%d] o estoy activo.          983\n",thisIndex,indexIzq);
    // Marcar que me llamó para que vuelva a llamar cuando yo le avise que finalicé esta fase.
    indexLlamoIzq = indexIzq;
  }
}

// Acepta y se mueve el Der
void Merge::acceptSwap(int valueN, int indexDer){
  // Guardar el valor de la derecha en el indice de guardado
  if(indexSave == thisIndex)
    saveValue(valueN,false);
  else
    thisProxy[indexSave].saveValue(valueN,false);
  indexSave++;

  // Si el indice derecho no llego al final, seguir comparando
  if(indexDer <= posicion) {
    // CkPrintf("[%d]--->v%d,t%d El indice derecho no se pasó del final. Comparar con el próximo derecho: [%d].\n",thisIndex,myValue,tempo,indexDer);
    startCompare(indexDer,indexSave,true,posicion,primero); //Se mantiene el Izq y Compara con indexDer (que ya avanzó)
  }
  else {
    // CkPrintf("[%d]: El indice derecho llegó al final. (indexDer=%d > posicion=%d)\n",thisIndex,indexDer,posicion);
    if(indexSave < posicion){
      // CkPrintf("[%d]: Son %d elementos. El indice de guardado no llegó al final. Copiar valores restantes.\n",thisIndex,posicion+1-thisIndex);
      int indexIzq = thisIndex;
      while(indexSave <= posicion)
      if(indexIzq == thisIndex){
        indexIzq++;
        saveTempo(indexSave++,false);
      }else
        thisProxy[indexIzq++].saveTempo(indexSave++,false);  //Copia valores restantes
    }
    else{
      // CkPrintf("[%d]: Son solo %d elementos. El indice de guardado llegó al final. (((indexSave==%d)))\n",thisIndex,posicion+1-thisIndex,indexSave);
      thisProxy[indexSave].saveValue(tempo,true);  //Copia valor restante
    }
    check(indexDer);
  }
}

void Merge::check(int indexDer){
  phase--;
  cantFases--;
  // thisProxy[indexDer-1].setPhase(phase); // Le cambio la fase
  CkPrintf("[%d]: --------------NUEVA FASE: %d--------------9867 (posicion=%d, posicionDer=%d)\n",thisIndex,phase,posicion,posicionDer);
  // imprimir("NF");
  if(phase > 0){
   if(activo){
      if(indexLlamoIzq >= 0){
        activo = false;
        CkPrintf("[%d] Se desactivó en check Izq. Avisar a [%d] que me puede llamar. 983Izq*************************\n",thisIndex,indexLlamoIzq);
        thisProxy[indexLlamoIzq].cambiarPosicion(thisIndex,false);
        // thisProxy[indexLlamoIzq].startCompare(thisIndex,indexLlamoIzq,false);
        indexLlamoIzq = -1;
      }
      else if(cantFases > 0){
      // else if(posicionDer > 0 && activo){
        CkPrintf("[%d] Comienza Siguiente Fase: %d.   983**********************************\n",thisIndex,phase);
        cambiarPosicion(indexDer,true);
      }
      else{
        CkPrintf("[%d] Se desactivó definitivamente.\n",thisIndex);
        activo = false;
      }
   }
  }
  else{
    CkPrintf("[%d] Se desactivó en check else 981\n",thisIndex);
    activo = false;
    // checkines = (posicion-thisIndex);
    // if(thisIndex==0){
      // CkPrintf("\n==================================================================\n[%d] ======================  FIN DEL PROGRAMA ======================\n==================================================================\n",thisIndex);
      // imprimir("===FIN===");
    // }
  }
}

void Merge::saveValue(int value, bool deboCopiarTempo){
  // int random =  rand() % 100;
  // Si son solo dos elementos, le guardo el valor del izquierdo
  if(deboCopiarTempo){
    CkPrintf("[%d] tempo = myValue, %d<---%d sV1\n",thisIndex,tempo,value);
    tempo = value;
  }
  else{
    CkPrintf("[%d] tempo = myValue, %d<---%d sV2\n",thisIndex,tempo,myValue);
    tempo = myValue;
  }
  // CkPrintf("[%d]>> Cambiando valor de %d a %d. (%d)        988\n",thisIndex,myValue,value,random);
  myValue = value;
  // char str[2];
  // sprintf(str,"%d",random);
  // strcat(str,")_AFTER ");
  // if(indexSave==numElements-1 && phase==0) //Tiene un error porque se va a cumplir para todos los de la derecha

  CkPrintf("[%d] posicion=%d y indexSave=%d  7777\n",thisIndex,posicion,indexSave);
  // if(thisIndex==0 && posicion==numElements-1 && indexSave==numElements-1)
    // imprimir("saveValue");
    if(thisIndex == numElements-1 && phase == 0 && indexSave == numElements-1){
      // CkPrintf("s\n==================================================================\n[%d] ======================  FIN DEL PROGRAMA ======================\n==================================================================\n",thisIndex);
      mainProxy.terminar();
      // CkPrintf("[%d] posicion=%d y indexSave=%d  777\n",thisIndex,posicion,indexSave);
      // imprimir(str);
    }
}
void Merge::saveTempo(int indexSave, bool ok){
  CkPrintf("[%d] Cambiando TEMPO valor de [%d] a v%d,t%d.\n",thisIndex,indexSave,myValue,tempo);
  // tempo = myValue;
  // if(ok)
    // thisProxy[indexSave].saveValue(tempo,true);
  // else
    thisProxy[indexSave].saveValue(tempo,false); //DEFAULT
}
void Merge::cambiarPosicion(int indexDer, bool meLlamoYo){
  // CkPrintf("[%d] cambiarPosicion-------------------------------llamadopor=%d------------------------------98****\n",thisIndex,indexDer);
  // primero = indexDer; // == posicionDer??? +1???
  if(meLlamoYo){
    // CkPrintf("[%d] Cambia su posicion %d a %d-------------------------------------------------------------****\n",thisIndex,posicion,posicionDer);
    posicion = posicionDer;
  }

  CkPrintf("[%d] cambiarPosicion------------888-------posicion+1 %d------------posicionDer+1=%d y indexDer=%d-----------------------------985-------------****\n",thisIndex,posicion+1,posicionDer+1,indexDer);
  if(posicion>-1 && posicion<numElements-1)
      thisProxy[posicion+1].solicitarPosicionDer(thisIndex,indexDer,phase);
  else{
    CkPrintf("[%d] cambiarPosicion------PHASE=====%d-----888--------posicion+1 %d------------posicionDer+1=%d y indexDer=%d-----------------------------985-------------****\n",thisIndex,phase,posicion+1,posicionDer+1,indexDer);
    startCompare(indexDer,thisIndex,false,posicion,primero);
  }
}

void Merge::solicitarPosicionDer(int indexN, int indexDer, int phaseN){
  //CkPrintf("Entrando a solicitarPosicionDer::: [%d] phase=%d,    [%d] phaseN=%d     estoyAc=%d                9867 Ingreso\n",thisIndex,phase,indexN,phaseN,estoyActualizando);
  if(phaseN == phase){
    //CkPrintf("[%d] %d < %d ? ++++++++++++++++p%d numElements%d indexN%d p%d +++++++++++++++ 986A Env\n",thisIndex,posicion,(numElements-indexN)/(phase-1),posicion,numElements,indexN,phase-1);
    thisProxy[indexN].cambiarPosicionDer(posicion,indexDer);
  }
  else
      thisProxy[indexN].cambiarPosicionDer(posicionDer,indexDer);
  // imprimir(str);
}

void Merge::cambiarPosicionDer(int posicionDerN, int indexDer){
  posicionDer = posicionDerN;
  // CkPrintf("[%d] Cambia posicionDer=%d      9867\n",thisIndex,posicionDer);
  startCompare(indexDer,thisIndex,true,posicion,primero);
}

// Deniega y Se mueve el Índice Izquierdo
void Merge::denySwap(int value, int indexDer){
    CkPrintf("[%d] en denySwap con indexSave=%d\n",indexSave);
    if(indexSave == thisIndex){
        CkPrintf("[%d] guarda %d en [%d]\n",thisIndex,value,indexSave);
        saveValue(value,false);
    }else{
        CkPrintf("[%d] guarda %d en [%d]\n",thisIndex,value,indexSave);
        thisProxy[indexSave].saveValue(value,false);
    }
    indexSave++;
    if( (thisIndex+1-primero) <= (posicion-primero)/2){
        thisProxy[thisIndex+1].startCompare(indexDer,indexSave,false,posicion,primero);  //Muevo izquierdo.
    }
    else{
        // if(indexSave < posicion){
        //    // CkPrintf("[%d]: El indice de guardado no llegó al final. Copiar valores restantes.\n",thisIndex);
        //     int indexIzq = indexDer;
        //     while(indexSave <= posicion){
        //         if(indexIzq==indexDer){
        //             indexIzq++;
        //             saveTempo(indexSave++);
        //         }
        //         else
        //             thisProxy[indexIzq++].saveTempo(indexSave++);  //Copia valores restantes
        //     }
        // }
        // else
        //     thisProxy[indexSave].saveValue(tempo,true);  //Copia valor restante
        //
        indexDer = posicion+1;
        thisProxy[primero].check(indexDer);
    }
}

void Merge::imprimir(char* prefix) {
  if (thisIndex < numElements - 1) {
    CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
    thisProxy[thisIndex + 1].imprimir(prefix);
  } else {
    CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
  }
}

// Display the value
void Merge::displayValue(int prefixLen, char* prefix) {
  prefix[prefixLen] = '\0';
  if (thisIndex < numElements - 1) {
    CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
    thisProxy[thisIndex + 1].displayValue(prefixLen, prefix);
  } else {
    CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase);
    mainProxy.arrayDisplayFinished();
  }
}

#include "merge.def.h"
