#define _CRT_SECURE_NO_WARNINGS
#define _CRT_RAND_S
#include <pthread.h>
#include <string>
#include <semaphore.h>
#include <ctime>
#include <iostream>
#include <conio.h>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
    #define Sleep(x) Sleep(x)
    #define wait() _getch()
#else
#include <unistd.h>
    #define Sleep(x) sleep(x)
    #define wait() scanf("1")
#endif
using namespace std;
sem_t semaphore1;
sem_t semaphore2;
sem_t semaphore3;
pthread_cond_t not_full;
int counter = 0;
int readers = 0;
int writers = 0;

/**
 * Метод писателя
 * @param args
 * @return
 */
void* writer(void* args) {
    //ждем семафор, который оставляет двругой писатель в конце
    sem_wait(&semaphore1);
    // ждем, пока все читатели выйдут
    while (counter != 0 ) {
        pthread_cond_wait(&not_full, &semaphore2);
    }
    int num = writers++;
    string mes1 = "writer ";
    mes1.append(to_string(num));
    mes1.append(" started working at ");
    mes1.append(to_string(clock()));
    mes1.append("\n\n");
    cout<<mes1;
    Sleep(1500);
    string mes2 = "writer ";
    mes2.append(to_string(num));
    mes2.append(" stopped working at ");
    mes2.append(to_string(clock()));
    mes2.append("\n\n");
    cout<<mes2;
    sem_post(&semaphore1);
}

void* reader(void* args) {
    //Я решил чуть усложнить условие, теперь на входе каждый читатель и писатель получает талончик(с номером)
    //Писатели одновременно не смогут получить доступ к талончику, а вот читатели смогут, плэтому я добавляю
    // здесь еще один семафор для получения толончика входа(можно было и мьютекс)
    sem_wait(&semaphore3);
    int num = readers++;
    sem_post(&semaphore3);
    counter++;
    string mes1 = "reader ";
    mes1.append(to_string(num));
    mes1.append(" started reading at ");
    mes1.append(to_string(clock()));
    mes1.append("\n\n");
    cout<<mes1;
    Sleep(1500);
    string mes2 = "reader ";
    mes2.append(to_string(num));
    mes2.append(" stopped reading at ");
    mes2.append(to_string(clock()));
    mes2.append("\n\n");
    cout<<mes2;
    counter--;
}



int main() {
    sem_init(&semaphore1, 0, 1);
    sem_init(&semaphore2, 0, 0);
    sem_init(&semaphore3, 0, 1);
    int wrNum;
    printf("Writers (>=1)=");
    cin >> wrNum;
    if (wrNum < 1){
        wrNum = 1;
    }
    int rNum;
    printf("Readers (>=1)=");
    cin >> rNum;
    if (rNum < 1){
        rNum = 1;
    }
    vector<pthread_t> writers;
    for (int i = 0; i < wrNum; i++){
        writers.push_back(pthread_t());
    }
    vector<pthread_t> readers;
    for (int i = 0; i < rNum; i++){
        readers.push_back(pthread_t());
    }
    int len = (rNum > wrNum) ? rNum : wrNum;
    for (int i = 0; i < len; i++){
        //заупскаем читателей и писателей вперемешку
        if (i < wrNum){
            pthread_create(&writers[i], NULL, writer, NULL);
        }
        if (i < rNum){
            pthread_create(&readers[i], NULL, reader, NULL);
        }

    }
    //ожидаем завершения всех потоков
    for(auto t: writers){
        pthread_join(t, NULL);
    }
    for(auto t: readers){
        pthread_join(t, NULL);
    }


    sem_destroy(&semaphore1);
    sem_destroy(&semaphore2);
    sem_destroy(&semaphore3);
    return 0;
}