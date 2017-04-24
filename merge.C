/*
Anda con hasta 21. No anda con 12. Lo que queda corregir es solicitarPosicionDer
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
  tempo = myValue;
  phase = 0;
  activo = false;
  indexSave = thisIndex;
  posicion = -1;
  posicionDer = -1;
  indexLlamoIzq = -1;
  accion = 0;
  primero = -1;
  checkines = 0;
  estoyActualizando = false;
}

// Constructor needed for chare object migration (ignore for now)
// NOTE: This constructor does not need to appear in the ".ci" file
Merge::Merge(CkMigrateMessage *msg) { }

void Merge::initPhase(int pos, int posDer, int phaseN) {
  phase = phaseN;
  phase++;
  activo = true;
  // CkPrintf("Chare [%d] con POS %d, phase = %d\n",thisIndex,pos,phase);
  newNumElements = pos-thisIndex+1;
  newPos = newNumElements/2-1+thisIndex;
  if(newNumElements < 3){
    if(thisIndex != pos)
    phase++;
    // CkPrintf("Chare [%d] comienza con phase = %d, y pos %d\n",thisIndex,phase,pos);
    posicion = pos;
    posicionDer = posDer; // VER SI VA ACA
    if(newNumElements==1)
    posicion = posicionDer;
    if(newNumElements>1){
      thisProxy[thisIndex+1].setPhase(phase);
    }
    primero = thisIndex+1;
    startCompare(primero,indexSave, true);
  }
  else{
    thisProxy[thisIndex].initPhase(newPos,pos,phase);
    thisProxy[newPos+1].initPhase(pos,-1,phase);
  }
}

void Merge::setPhase(int phaseN){
  phase = phaseN;
}

void Merge::startCompare(int indexDer, int indexS, bool seMovioIndexDer){
  CkPrintf("[%d] startCompare con indexDer=%d, phase=%d, posicion=%d, posicionDer=%d\n",thisIndex,indexDer,phase,posicion,posicionDer);
  char str[2];
  sprintf(str,"%d",thisIndex);
  strcat(str," start1");
  //  imprimir(str);
  if(!seMovioIndexDer){
    CkPrintf("[%d] tempo = myValue, %d<---%d\n",thisIndex,tempo,myValue);
    tempo = myValue;
  }
  // CkPrintf("\n>>>Comparando [%d]--->v%d,t%d con [%d]    (en phase=%d, posicion=%d, posicionDer=%d)\n",thisIndex,myValue,tempo,indexDer,phase,posicion,posicionDer);
  // CkPrintf("\n[%d] Comparando con [%d]    (en phase=%d) a%d\n",thisIndex,indexDer,phase,accion++);
  indexSave = indexS;
  thisProxy[indexDer].requestSwap(phase,tempo,thisIndex);
}

void Merge::requestSwap(int phaseN, int valueN, int indexIzq){
  CkPrintf("\t\t\t\t[%d]--->v%d:ph%d requestSwap con [%d]--->v%d:ph%d 983\n",indexIzq,valueN,phaseN,thisIndex,myValue,phase);
  // CkPrintf("Chare [%d] en requestSwap. phase:%d, phaseN:%d\n",thisIndex,phase,phaseN);
  if(phase == phaseN || !activo){
    // if(activo && phase==phaseN || !activo){
    // activo = false;
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
    CkPrintf("[%d]--->v%d,t%d El indice derecho no se pasó del final. Comparar con el próximo derecho: [%d].\n",thisIndex,myValue,tempo,indexDer);
    startCompare(indexDer,indexSave,true); //Se mantiene el Izq y Compara con indexDer (que ya avanzó)
  }
  else {
    CkPrintf("[%d]: El indice derecho llegó al final. (indexDer=%d > posicion=%d)\n",thisIndex,indexDer,posicion);
    if(indexSave < posicion){
      CkPrintf("[%d]: Son %d elementos. El indice de guardado no llegó al final. Copiar valores restantes.\n",thisIndex,posicion+1-thisIndex);
      int indexIzq = thisIndex;
      while(indexSave <= posicion)
      if(indexIzq == thisIndex){
        indexIzq++;
        saveTempo(indexSave++,false);
      }else
        thisProxy[indexIzq++].saveTempo(indexSave++,false);  //Copia valores restantes
    }
    else{
      CkPrintf("[%d]: Son solo %d elementos. El indice de guardado llegó al final. (((indexSave==%d)))\n",thisIndex,posicion+1-thisIndex,indexSave);
      thisProxy[indexSave].saveValue(tempo,true);  //Copia valor restante
      if(indexSave == thisIndex)
      CkPrintf("\n\n\n ============456==================================================================\n[%d] ESTO NO DEBERIA EJECUTARSE. ARREGLAR.\n ==============================================================================\n\n",thisIndex);
    }
    check(indexDer);
  }
}

void Merge::check(int indexDer){
  phase--;
  // thisProxy[indexDer-1].setPhase(phase); // Le cambio la fase
  CkPrintf("[%d]: --------------NUEVA FASE: %d-------------- (posicion=%d, posicionDer=%d)\n",thisIndex,phase,posicion,posicionDer);
  // char str[20];
  // sprintf(str,"%d",thisIndex);
  // strcat(str,"> AFTER ");
  // imprimir("NF");
  // tempo = myValue;
  if(phase > 0){
//    if(activo){
      if(indexLlamoIzq >= 0){
        activo = false;
        CkPrintf("[%d] Se desactivó en check Izq. Avisar a [%d] que me puede llamar. 983Izq*************************\n",thisIndex,indexLlamoIzq);
        thisProxy[indexLlamoIzq].cambiarPosicion(thisIndex,false);
        // thisProxy[indexLlamoIzq].startCompare(thisIndex,indexLlamoIzq,false);
        indexLlamoIzq = -1;
      }
      else if(posicionDer > 0 && activo){
        CkPrintf("[%d] Comienza Siguiente Fase: %d.   983**********************************\n",thisIndex,phase);
        cambiarPosicion(indexDer,true);
      }
      else{
        CkPrintf("[%d] Se desactivó definitivamente.\n",thisIndex);
        activo = false;
      }
  //  }
  }
  else{
    CkPrintf("[%d] Se desactivó en check else 981\n",thisIndex);
    activo = false;
    checkines = (posicion-thisIndex);
    if(thisIndex==0){
      CkPrintf("\n==================================================================\n[%d] ======================  FIN DEL PROGRAMA ======================\n==================================================================\n",thisIndex);
      imprimir("===FIN===");
    }
  }
}

void Merge::saveValue(int value, bool deboCopiarTempo){
  int random =  rand() % 100;
  // Si son solo dos elementos, le guardo el valor del izquierdo
  if(deboCopiarTempo){
    CkPrintf("[%d] tempo = myValue, %d<---%d sV1\n",thisIndex,tempo,value);
    tempo = value;
  }
  else{
    CkPrintf("[%d] tempo = myValue, %d<---%d sV2\n",thisIndex,tempo,myValue);
    tempo = myValue;
  }
  CkPrintf("[%d]>> Cambiando valor de %d a %d. a%d (%d)        988\n",thisIndex,myValue,value,accion++,random);
  myValue = value;
  char str[2];
  sprintf(str,"%d",random);
  strcat(str,")_AFTER ");
  // if(indexSave==numElements-1 && phase==0) //Tiene un error porque se va a cumplir para todos los de la derecha
    imprimir("saveValue");
}
void Merge::saveTempo(int indexSave, bool ok){
  CkPrintf("[%d] Cambiando TEMPO valor de [%d] a v%d,t%d. a%d\n",thisIndex,indexSave,myValue,tempo,accion++);
  // tempo = myValue;
  // if(ok)
    // thisProxy[indexSave].saveValue(tempo,true);
  // else
    thisProxy[indexSave].saveValue(tempo,false); //DEFAULT
}
void Merge::cambiarPosicion(int indexDer, bool meLlamoYo){
  CkPrintf("[%d] cambiarPosicion-------------------------------llamadopor=%d------------------------------98****\n",thisIndex,indexDer);
  // primero = indexDer; // == posicionDer??? +1???
  if(meLlamoYo){
    CkPrintf("[%d] Cambia su posicion %d a %d-------------------------------------------------------------****\n",thisIndex,posicion,posicionDer);
    posicion = posicionDer;
  }

  CkPrintf("[%d] cambiarPosicion-------------------posicion+1 %d------------posicionDer+1=%d y indexDer=%d-----------------------------985-------------****\n",thisIndex,posicion+1,posicionDer+1,indexDer);
  // CkPrintf("[%d] ======= [%d]------------------------------------------------$$$$$$$$$$$$984$$$$$$$$-------------****\n",thisIndex,indexIzq);
  if(posicionDer < numElements-1){
    // if(activo)
    // if((numElements+posicion+1)/(phase+1)!=(numElements-posicion+1) && (posicion+1)<numElements-1)
      estoyActualizando = true;
      thisProxy[posicion+1].solicitarPosicionDer(thisIndex,indexDer,phase);
  }else{
    // posicionDer = -1;
    startCompare(indexDer,thisIndex,false);
  }
}

void Merge::solicitarPosicionDer(int indexN, int indexDer, int phaseN){
  CkPrintf("Entrando a solicitarPosicionDer::: [%d] phase=%d,    [%d] phaseN=%d                     986 Ingreso\n",thisIndex,phase,indexN,phaseN);

  if(phaseN == phase){
  // if(phaseN == phase && !estoyActualizando){
    CkPrintf("[%d] %d < %d ? ++++++++++++++++p%d numElements%d indexN%d p%d +++++++++++++++ 986A Env\n",thisIndex,posicion,(numElements-indexN)/(phase-1),posicion,numElements,indexN,phase-1);
    if(posicion <= (numElements-indexN)/(phase-1)){                                     // if(posicion < (numElements-indexN)/(phase-1) || posicion==numElements-1){
      CkPrintf("[%d] Envía posicionDer=%d a [%d]  986 A1\n",thisIndex,posicion,indexN);
      thisProxy[indexN].cambiarPosicionDer(posicion,indexDer);
      // activo = false;
    }else{
      CkPrintf("[%d] Envía posicionDer=%d a [%d]  986 B1\n",thisIndex,-1,indexN);
      thisProxy[indexN].cambiarPosicionDer(-1,indexDer);
    }
  }
  else{
      if(phaseN > phase){
        CkPrintf("phaseN > phase. [%d] phase=%d (p%d,pd%d)    no está en la misma fase que    [%d] phaseN=%d                     986 C1\n",thisIndex,phase,posicion,posicionDer,indexN,phaseN);
        CkPrintf("[%d] Envía posicionDer=%d a [%d]  986 B2\n",thisIndex,-1,indexN);
        thisProxy[indexN].cambiarPosicionDer(-1,indexDer);
      }else{
        CkPrintf("phaseN < phase. [%d] phase=%d (p%d,pd%d)    no está en la misma fase que    [%d] phaseN=%d                     986 C2\n",thisIndex,phase,posicion,posicionDer,indexN,phaseN);
        CkPrintf("[%d] %d < %d ? ++++++++++++++++p%d numElements%d indexN%d p%d +++++++++++++++ 986A Env\n",thisIndex,posicion,(numElements-indexN)/(phase-1),posicion,numElements,indexN,phase);
        indexLlamoIzq = indexN;
      }
      // thisProxy[indexN].cambiarPosicionDer(-1,indexDer); //CON ESTO ANDA HASTA 21 sin lo de este else
      char str[20];
      sprintf(str,"%d",thisIndex);
      strcat(str,"986   ");
      // imprimir(str);
  }
}

void Merge::cambiarPosicionDer(int posicionDerN, int indexDer){
  estoyActualizando = false;
  // posicion = posicionDer;
  posicionDer = posicionDerN;
  //  if(posicion==posicionDer)
  //     posicionDer=-1;
  CkPrintf("[%d] Cambia posicionDer=%d          98\n",thisIndex,posicionDer);
  startCompare(indexDer,thisIndex,true);
}



















// Deniega y Se mueve el Izq
void Merge::denySwap(int value, int indexDer){
  // myValue = value;
  // Guarda su valor en el indice de guardado
  if(indexSave == thisIndex)
    saveValue(value,false);
  else
    thisProxy[indexSave].saveValue(value,false);
  indexSave++;

  // if(thisIndex < primero) {
  //     CkPrintf("[%d]--->v%d,t%d El indice izquierdo no se pasó del final. Comparar con el próximo derecho: [%d].\n",thisIndex,myValue,tempo,indexDer);
  //     startCompare(indexDer,indexSave,true); //Se mantiene el Izq y Compara con indexDer (que ya avanzó)
  //   }
  //   else {
  //     CkPrintf("[%d]: El indice derecho llegó al final. (indexDer=%d > posicion=%d)\n",thisIndex,indexDer,posicion);
  //     if(indexSave < posicion){
  // // CkPrintf("Chare [%d]. [%d] le DENEGÓ SWAP. nuevo valor = %d indexSave=%d <> posicion=%d\n",thisIndex,indexDer,value,indexSave,posicion);
  // if(indexSave == thisIndex){ // Termina la fase
  //   // CkPrintf("Chare [%d] RESTA fase D\n",thisIndex);
  //   phase--;
  //   // CkPrintf("Chare [%d] tiene nueva phase: (%d)\n",thisIndex,phase);
  //   if(indexLlamoIzq >= 0){
  //     // CkPrintf("Chare [%d] volviendo a llamar a [%d]\n",thisIndex,indexLlamoIzq);
  //     thisProxy[indexLlamoIzq].startCompare(thisIndex,indexLlamoIzq,false);
  //   }
  //   // check();
  // }
  // else{
  //   // CkPrintf("Chare [%d] AVANZA indexSave D. Llama a [%d]\n",thisIndex,thisIndex+1);
  //   indexSave++;
  //   if (thisIndex!=indexDer) {
  //     thisProxy[thisIndex+1].startCompare(indexDer,indexSave,false); //Avanza el Izq con thisIndex+1 y Compara con indexDer (que se mantuvo igual)
  //   }
  //   else{
  //     // thisProxy[indexSave].saveValue(tempo,thisIndex);
  //     // thisProxy[0].displayValue(7,"AFTER ");
  //     // thisProxy[1].displayValue(7,"AFTER ");
  //     // thisProxy[2].displayValue(7,"AFTER ");
  //     // thisProxy[3].displayValue(7,"AFTER ");
  //     // thisProxy[4].displayValue(7,"AFTER ");
  //   }
  // }
}

void Merge::imprimir(char* prefix) {
  // char str[20];
  // sprintf(str,"%d",thisIndex);
  // strcat(str,"> AFTER ");
  // if(checkines==numElements-1)
    for (int i = 0; i < numElements; i++)
      thisProxy[i].displayValue(strlen(prefix),prefix);
}

void Merge::displayValue(int prefixLen, char* prefix) {
  // Display the value
  prefix[prefixLen] = '\0';
  if(thisIndex==0) CkPrintf("\n");
  CkPrintf("%s Merge[%d] =\t\t>>> v(%d) <<<\t\tt %d (p %d --- pd %d --- ph %d )\t\tch=%d                  (999)\n",prefix,thisIndex, myValue, tempo, posicion, posicionDer,phase,checkines);
  // if (thisIndex < numElements - 1) {
  //    thisProxy[thisIndex + 1].displayValue(prefixLen, prefix);
  // } else {
  // }
  //    mainProxy.arrayDisplayFinished();
}

#include "merge.def.h"
