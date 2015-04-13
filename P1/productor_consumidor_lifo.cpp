#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>

using namespace std;

const long num_items = 100; // elementos a producir
const long tam_vector = 10; // tamanio del buffer
int buffer[tam_vector]; // buffer de elementos
long pos = -1; // puntero de posicion
sem_t mutex; // semaforo de exclusion mutua

// funcion para producir datos
int producir_dato() {
   static int contador = 1;
   return contador++;
}

// funcion para consumir datos
void consumir_dato(int dato) {
   cout << "dato recibido: " << dato << endl;
}

// hebra productora
void * productor(void *) {
   for(unsigned i = 0; i < num_items; i++ ) {
      int dato = producir_dato();

      // comprobar que hay espacio para escribir en el buffer
      sem_wait(&mutex);
      while (pos == tam_vector-1) {
	 sem_post(&mutex);

	 sem_wait(&mutex);
      }
      // escribir en el buffer
      pos++;
      buffer[pos] = dato;
      sem_post(&mutex);
   }

   return NULL ;
}

void * consumidor(void *) {
   for(unsigned i = 0; i < num_items; i++ ) {
      // comprobar que hay elementos para leer
      sem_wait(&mutex);
      while (pos == -1) {
	 sem_post(&mutex);

	 sem_wait(&mutex);
      }
      // leer del buffer
      int dato = buffer[pos];
      pos--;
      sem_post(&mutex);

      consumir_dato(dato);
   }

   return NULL ;
}

int main() {
   pthread_t hebra_prod;
   pthread_t hebra_cons;

   // iniciar semaforo de exclusion mutua
   sem_init(&mutex, 0, 1);

   // crear hebras
   pthread_create(&hebra_prod, NULL, productor, NULL);
   pthread_create(&hebra_cons, NULL, consumidor, NULL);

   // esperar a finalizar
   pthread_join(hebra_prod, NULL);
   pthread_join(hebra_cons, NULL);

   cout << "fin\n";
}
