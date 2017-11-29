#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

int huntsmen, cooks, quarry, food;

pthread_mutex_t mutex_resources = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_huntsmen = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cooks = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_day = PTHREAD_MUTEX_INITIALIZER;

int throwDice(){
    return rand()%6+1;
}

void cook(){
    pthread_mutex_lock(&mutex_resources);
    if(quarry>0){
        quarry--;
        int gained = throwDice();
        food = food + gained;
        printf("Chef made %d food! Quarry now is %d! Food now is %d!\n", gained, quarry, food);
    }else{
        printf("There is no quarry to make food!\n");
    }
    pthread_mutex_unlock(&mutex_resources);
}

void increaseQuarry(){
    pthread_mutex_lock(&mutex_resources);
    quarry++;
    printf("Hunter gained quarry! Quarry now is %d!\n", quarry);
    pthread_mutex_unlock(&mutex_resources);
}

void decreaseHuntsmenNumber(){
    pthread_mutex_lock(&mutex_huntsmen);
    if(huntsmen>0){
        huntsmen--;
        printf("Hunter left the village\n");
    }
    pthread_mutex_unlock(&mutex_huntsmen);
}

void decreaseCooksNumber(){
    pthread_mutex_lock(&mutex_cooks);
    if(cooks>0){
        cooks--;
        printf("Chef left the village\n");
    }
    pthread_mutex_unlock(&mutex_cooks);
}

int getHuntsmenNumber(){
    pthread_mutex_lock(&mutex_huntsmen);
    int num = huntsmen;
    pthread_mutex_unlock(&mutex_huntsmen);
    return num;
}

int getCooksNumber(){
    pthread_mutex_lock(&mutex_cooks);
    int num = cooks;
    pthread_mutex_unlock(&mutex_cooks);
    return num;
}

bool eat(){
    pthread_mutex_lock(&mutex_resources);
    if(food>0){
        food--;
        printf("1 Food eaten! Food now is %d!\n", food);
        pthread_mutex_unlock(&mutex_resources);
        return true;
    }
    pthread_mutex_unlock(&mutex_resources);
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
        decreaseHuntsmenNumber();
    }
    usleep(500);
    pthread_exit(NULL);
    return NULL;
}

void *chef(void *vargp){
    srand(time(NULL));
    cook();
    if(!eat()){
        decreaseCooksNumber();
    }
    usleep(500);
    pthread_exit(NULL);
    return NULL;
}

void playDay(int d)
{
    pthread_t tid1;
    pthread_t tid2;
    for(int i=1; i<=getHuntsmenNumber(); i++){
        pthread_create(&tid1, NULL, hunter, NULL);
    }
    for(int i=1; i<=getCooksNumber(); i++){
        pthread_create(&tid2, NULL, chef, NULL);
    }
    pthread_join(tid1, NULL);
    printf("Day %d ended!\n", d);
}

int main(int argc, char *argv[])
{
    if (argc < 5){
        printf("Pass parameters huntsmen cooks quarry food");
        return 0;
    }
    huntsmen = atoi(argv[1]);
    cooks = atoi(argv[2]);
    quarry = atoi(argv[3]);
    food = atoi(argv[4]);
    pthread_mutex_init(&mutex_resources, NULL);
    pthread_mutex_init(&mutex_huntsmen, NULL);
    pthread_mutex_init(&mutex_cooks, NULL);
    pthread_mutex_init(&mutex_day, NULL);
    for (int d=1;d<=365;d++){
        pthread_mutex_lock(&mutex_day);
        if(getHuntsmenNumber()==0 && getCooksNumber()==0){
            printf("Your village lasted %d days!\n", d-1);
            break;
        }
        playDay(d);
        pthread_mutex_unlock(&mutex_day);
    }
    return 0;
}
