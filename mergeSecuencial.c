#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merging(int low, int mid, int high, int a[], int b[]) {
   int l1, l2, i;

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

   for(i = low; i <= high; i++)
      a[i] = b[i];
}

void sort(int low, int high,int a[], int b[]) {
   int mid;

   if(low < high) {
      mid = (low + high) / 2;
      sort(low, mid,a,b);
      sort(mid+1, high,a,b);
      merging(low, mid, high,a,b);
   } else {
      return;
   }
}

int main(int argc, char* argv[]) {
   int i,j;
   int cant;
   if(argc > 1){
       cant=atoi(argv[1]);
   }
   int a[cant];
   int b[cant-1];
   for(j=0;j<cant;j++){
       a[j]=rand()%10000;
   }

   // int a[11] = { 10, 14, 19, 26, 27, 31, 33, 35, 42, 44, 0 };
   // int b[10];
   // printf("List before sorting\n");
   //
   // for(i = 0; i < cant; i++){
   //    printf("%d, ", a[i]);}
   // printf("\n");
   double secs;
   clock_t t_ini = clock();
   sort(0, cant-1, a, b);
   clock_t t_fin = clock();
   //
   // printf("\nList after sorting\n");
   //
   // for(i = 0; i <= cant-1; i++){
   //    printf("%d, ", a[i]);}
   //  printf("\n");

    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    printf("%.16g milisegundos\n", secs * 1000.0);
}
