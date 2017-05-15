#ifndef __MERGE_H__
#define __MERGE_H__

class Merge : public CBase_Merge {

 private:
  /// Member Variables (Object State) ///
  int newPos, newcantChares, phase;
  int indexLlamoIzq, cantFases, elementos;
  bool activo;
  int *myValues;
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
  void startCompare(int);
  void requestSwap(int,int,int);
  void saveValue(int [],int,bool);
  void check();
  void listo();
};

#endif //__MERGE_H__
