#ifndef __MERGE_H__
#define __MERGE_H__

class Merge : public CBase_Merge {

 private:
  /// Member Variables (Object State) ///
  int nuevaPos, nuevaCantChares, phase, posicionAnterior, posicion, posicionDer, indexLlamoIzq, cantFases, elementos;
  bool activo, yaActualizoPosicion;
  int *myValues;
  /// Member Functions (private) ///
  void sort(int,int,int[]);
  void merging(int,int,int,int[]);
  void bubbleSort(int[],int);


 public:
  /// Constructors ///
  Merge();
  Merge(CkMigrateMessage *msg);

  /// Entry Methods ///
  void initPhase(int,int,int,int[],int);
  void setPhase(int,int[],int);
  void startCompare(int,int);
  void requestSwap(int,int,int);
  void saveValue(int [],int,bool);
  void check();
  void reiniciar(bool);
  void solicitarPosicion(int,int);
  void cambiarPosicionDer(int);
  void imprimir(char prefix[10]);
  void displayValue(int prefixLen, char* prefix);
};


#endif //__MERGE_H__
