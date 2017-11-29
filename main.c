#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

int huntsman, cooks, quarry, food;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int throwDice(){
    return rand()%6+1;
}

void cook(){
    pthread_mutex_lock(&mutex);
    if(quarry>0){
        quarry--;
        int gained = throwDice();
        food = food + gained;
        printf("Chef made %d food! Quarry now is %d! Food now is %d!\n", gained, quarry, food);
    }else{
        printf("There is no quarry to make food!\n");
    }
    pthread_mutex_unlock(&mutex);
}

void increaseQuarry(){
    pthread_mutex_lock(&mutex);
    quarry++;
    printf("Hunter gained quarry! Quarry now is %d!\n", quarry);
    pthread_mutex_unlock(&mutex);
}

bool eat(){
    pthread_mutex_lock(&mutex);
    if(food>0){
        food--;
        printf("1 Food eaten! Food now is %d!\n", food);
        pthread_mutex_unlock(&mutex);
        return true;
    }
    pthread_mutex_unlock(&mutex);
    return false;
}

bool huntResult(){
    int huntsmanResult = throwDice();
    int animalResult = throwDice();
    if(huntsmanResult>animalResult){
        return true;
    }
    printf("Hunter gained nothing!\n");
    return false;
}

void *hunter(void *vargp){
    srand(time(NULL));
    if(huntResult()) increaseQuarry();
    if(!eat()){
        huntsman--;
        printf("Hunter left the village\n");
    }
    usleep(500);
    pthread_exit(NULL);
    return NULL;
}

void *chef(void *vargp){
    srand(time(NULL));
    cook();
    if(!eat()){
        cooks--;
        printf("Chef left the village\n");
    }
    usleep(500);
    pthread_exit(NULL);
    return NULL;
}

void *playDay(void *vargp)
{
    pthread_t tid1;
    pthread_t tid2;
    for(int i=1; i<=huntsman; i++){
        pthread_create(&tid1, NULL, hunter, NULL);
    }
    for(int i=1; i<=cooks; i++){
        pthread_create(&tid2, NULL, chef, NULL);
    }
    pthread_join(tid1, NULL);
    printf("Day %d ended!\n", *(int*)vargp);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 5){
        printf("Pass parameters huntsman cooks quarry food");
        return 0;
    }
    huntsman = atoi(argv[1]);
    cooks = atoi(argv[2]);
    quarry = atoi(argv[3]);
    food = atoi(argv[4]);
    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);
    for (int d=1;d<=365;d++){
        if(huntsman==cooks && cooks==0){
            printf("Your village lasted for %d day(s)!\n", d-1);
            break;
        }else{
            pthread_create(&tid, NULL, playDay, &d);
            pthread_join(tid, NULL);
        }
    }
    return 0;
}
