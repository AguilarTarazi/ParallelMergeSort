#ifndef __MERGE_H__
#define __MERGE_H__

class Merge : public CBase_Merge {

 private:
  /// Member Variables (Object State) ///
  int myValue, newPos, newNumElements, phase, tempo, indexSave, posicion, posicionDer, indexLlamoIzq, accion, primero, checkines;
  bool activo, estoyActualizando;
  /// Member Functions (private) ///

 public:
  /// Constructors ///
  Merge();
  Merge(CkMigrateMessage *msg);

  /// Entry Methods ///
  void initPhase(int,int,int);
  void setPhase(int);
  void startCompare(int,int,bool);
  void requestSwap(int,int,int);
  void acceptSwap(int,int);
  void cambiarPosicion(int,bool);
  void solicitarPosicionDer(int,int,int);
  void cambiarPosicionDer(int,int);
  void denySwap(int,int);
  void saveValue(int,bool);
  void saveTempo(int,bool);
  void check(int);
  void imprimir(char prefix[10]);
  void displayValue(int prefixLen, char* prefix);
};


#endif //__MERGE_H__
