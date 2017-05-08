#ifndef __MERGE_H__
#define __MERGE_H__

class Merge : public CBase_Merge {

 private:
  /// Member Variables (Object State) ///
  int myValue, nuevaPos, nuevaCantChares, phase, tempo, indexSave, posicion, posicionDer;
  int indexLlamoIzq, accion, primero, cantFases, elementos;
  bool activo, llameYo;
  int *myValues;
  int *tempos;
  int numElementsLocal;
  int comparar[15];
  /// Member Functions (private) ///
  void sort(int,int,int[]);
  void merging(int,int,int,int[]);
  void bubbleSort(int[],int);


 public:
  /// Constructors ///
  Merge();
  Merge(CkMigrateMessage *msg);

  /// Entry Methods ///
  void initPhase(int,int,int,int[],int,int);
  void setPhase(int,int[],int);
  void setValues(int[],int);
  void startCompare(int,int,bool,int,int);
  void startComparePhase(int,int,bool,int,int,int);
  void requestSwap(int,int,int,int);
  void acceptSwap(int,int,int [],int);
  void cambiarPosicion(int,bool);
  void solicitarPosicionDer(int,int,int);
  void cambiarPosicionDer(int,int);
  void denySwap(int,int,int [],int);
  void saveValue(int [],int,bool);
  // void saveTempo(int,int []);
  void check(int);
  void imprimir(char prefix[10]);
  void displayValue(int prefixLen, char* prefix);

  //void compareData(int [],int);

};


#endif //__MERGE_H__
