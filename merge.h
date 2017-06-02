#ifndef __MERGE_H__
#define __MERGE_H__

class Merge : public CBase_Merge {

 private:
  /// Member Variables (Object State) ///
  int newLastChareIzq, cantChares, phase;
  int indexLlamoIzq, valueLlamoIzq, cantFases, elementos;
  bool activo, ordenado;
  int *myValues;
  long tiempo;
  int comparar[30];
  /// Member Functions (private) ///
  void sort(int,int);
  void merging(int,int,int);
  void bubbleSort(int);
  void insertionSort();

 public:
  /// Constructors ///
  Merge();
  Merge(CkMigrateMessage *msg);
  /// Entry Methods ///
  void initPhase(int,int,int,int,int[],int);
  void requestSwap(int,int,int);
  void saveValue(int [],int,bool);
  void check();
};

#endif //__MERGE_H__
