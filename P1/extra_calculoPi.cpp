#include <iostream>
#include <pthread.h>
#include <cstdlib>

using namespace std;

// numero de hebras
long nthreads = 4;
// numero de partes del intervalo
long partes;
// vector para almacenar las sumas parciales de cada hebra
double *sumas;

// la funcion a integrar
double f (double x) {
   return 4.0/(1+x*x);
}

// la funcion para las sumas parciales
void* suma_parcial(void* indice) {
   long nhebra = (long) indice;
   // comienzo y fin del bucle
   long ini = nhebra * partes/nthreads;
   long fin = ini + partes/nthreads;

   // sumar cada parte
   double sumap = 0;
   for (long i = ini; i < fin; i++) {
      sumap += f(i*1.0/partes)/partes;
   }

   // almacenar la suma parcial en el vector
   sumas[nhebra] = sumap;
}

int main(int argc, char* argv[]) {
   // leer argumentos de entrada (numero de partes y hebras)
   switch (argc) {
      case 3:
         nthreads = atoi(argv[2]);
      case 2:
         partes = atoi(argv[1]);
         break;
      default:
         partes = 1;
   }

   sumas = new double[nthreads];

   // redondeo del numero de puntos hacia arriba
   partes = ((partes+nthreads-1)/nthreads)*nthreads;

   // lanzar las hebras
   pthread_t hebras[nthreads];
   for (long i = 0; i < nthreads; i++) {
      pthread_create(hebras+i,NULL,suma_parcial,(void*)i);
   }

   // juntar todas las sumas
   double suma = 0;
   for (long i = 0; i < nthreads; i++) {
      pthread_join(hebras[i],NULL);
      suma += sumas[i];
   }

   // modificar la precision al imprimir el float
   cout.precision(20);
   // imprimir resultado
   cout << "La suma es " << suma << endl;
}
