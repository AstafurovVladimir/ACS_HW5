#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const int dbSize = 25;
int dataBase[dbSize];
pthread_mutex_t mutexWrite;
sem_t  taken; 
unsigned int seed = 42;

void SortDB() {
    for (int i = 0; i < dbSize; i++) {
        for (int j = 1; j < dbSize; j++) {
            if (dataBase[j - 1] > dataBase[j]) {
                int tmp = dataBase[j];
                dataBase[j] = dataBase[j - 1];
                dataBase[j - 1] = tmp;
            }
        }
    }
}

void *Writer(void *param) {
    int number = *((int*)param);
    while (true) {
        int data = rand() % 100;
        int sleepTime = rand() % 20;
        int workSlot = rand() % dbSize;
        pthread_mutex_lock(&mutexWrite); //Лок записи от других писателей
        sem_init(&taken, 0, 0); //Установка семафора на 0 запрещающая вход читателям
        int old = dataBase[workSlot]; 
        dataBase[workSlot] = data;
        SortDB();
        printf("Producer %d: Replacing value %d with value %d in cell [%d]\nDatabase now is:\n", number, old, data, workSlot) ; 
        for (int i = 0; i < dbSize; i++) {
            printf("%d ", dataBase[i]);
        }
        printf("\n");
        sem_init(&taken, 0, 100); //Установка семафора на число значительно большее чем число читателей
        pthread_mutex_unlock(&mutexWrite); //Анлок записи
        sleep(sleepTime);
    }
    return nullptr;
}

void *Reader(void *param) {
     int number = *((int*)param);
    while (true) {
        int sleepTime = rand() % 20;
        int workSlot = rand() % dbSize;
        sem_wait(&taken); //Проверка семафора
        int data = dataBase[workSlot]; 
        printf("Reader %d: Reading value %d in cell [%d]\n", number, data, workSlot) ;
        sem_post(&taken);
        sleep(sleepTime);
    }
    return nullptr;
}

int main() {
    srand(seed);
    for (int i = 0; i < dbSize; i++) {
        dataBase[i] = rand() % 100;
    }
    SortDB();
    pthread_mutex_init(&mutexWrite, nullptr) ;
    sem_init(&taken, 0, 0); 
    pthread_t threadWriters[3] ;
    int writers[3];
    for (int i=0 ; i<3 ; i++) {
        writers[i] = i + 1;
        pthread_create(&threadWriters[i],nullptr,Writer, (void*)(writers+i));
    }
    pthread_t threadReaders[6] ;
    int readers[6];
    for (int i=0 ; i < 6 ; i++) {
        readers[i] = i + 6;
        pthread_create(&threadReaders[i],nullptr,Reader, (void*)(readers+i));
    }
    sleep(30); // Приостановка главного потока чтобы читатели и писатели работали
    return 0;
}
