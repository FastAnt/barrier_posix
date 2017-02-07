#include<stdio.h>
#include<string.h>
#include <iostream>

#include <fstream>

#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

#include <vector>
#include <list>

using namespace std;

#define PID_FILE_NAME "/home/box/main.pid"

enum errThreadCreate{};
vector<pthread_t> threadConteiner;
//pthread_t threadConteiner[4];
pthread_mutex_t     lock;
pthread_cond_t      condit          = PTHREAD_COND_INITIALIZER;
bool                doneCondit      = false;
pthread_barrier_t   barrier;

void * threadsWorkWithBarrier(void * arg)
{
    std::cout << "wait" << std::endl;
    pthread_barrier_wait (&barrier);
    std::cout << "done" << std::endl;
    return NULL;
}

void * threadsWorkWithConditionSignal(void * arg)
{
    pthread_mutex_lock(&lock);
    std::cout  << " mutex lock" << std::endl;
    while(!doneCondit)
        pthread_cond_wait(&condit,&lock);
    std::cout << "condition signal" << std::endl;
    pthread_mutex_unlock(&lock);
    return NULL;
}


void savePIDtoFile()
{
    std::ofstream f (PID_FILE_NAME);
    f << getpid() << std::endl;
}

enum neededThreadsElem {berrierThread1, berrierThread2, conditionVariableThread1, conditionVariableThread2 };
std::list<neededThreadsElem> threadList = {berrierThread1, berrierThread2,conditionVariableThread1 , conditionVariableThread2};
int main ()
{
    try
    {
       savePIDtoFile();
       pthread_barrier_init (&barrier, NULL, 2);
       pthread_t firstThread;
       for(auto i : threadList)
            threadConteiner.push_back(i);

       auto err = pthread_create(&(threadConteiner[berrierThread1]), NULL, &threadsWorkWithConditionSignal , NULL);
       if (err !=0)
           throw errThreadCreate();

       err = pthread_create(&(threadConteiner[berrierThread2]), NULL, &threadsWorkWithBarrier, NULL);
       if (err !=0)
           throw errThreadCreate();

       cout << "sleeping" << endl;
       sleep(5);
       pthread_mutex_lock(&lock);
       doneCondit = true;
       pthread_cond_signal(&condit);
       pthread_mutex_unlock(&lock);

       pthread_barrier_wait (&barrier);

       pthread_join(threadConteiner[berrierThread1], NULL);
       pthread_join(threadConteiner[berrierThread2], NULL);

       pthread_mutex_destroy(&lock);
       std::cout << "finished" << std::endl;
       return 0;




    }
    catch(errThreadCreate a)
    {
        cout << "cannot create the thread" << endl;
    }
    catch(...)
    {
        cout << "some shit happen" << endl;
    }
}
