#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/time.h>


pthread_mutex_t mutex;
long sum = 0;
long primeCounter = 0;
long *randomNumbers;
long size;

/**
 * From number theory we can say that number is a prime if it is equivilent
 * to 1 or 5 in mod 6.
 * Now, in our isPrime function we will check first if number is equiv to 1 or 5 in mod 6,
 * if so we will keep checking, but only untill sqrt(n) because it is knowm from sieve of Erastosenes.
 * Else we return no.
 *
 */
void* isPrime(int num){
    if(num == 2 || num == 3){
        pthread_mutex_lock(&mutex);
        sum += num;
        primeCounter++;
        pthread_mutex_unlock(&mutex);
    }
    if(num%6 == 1 || num%6 == 5){
        //run from 2 to sqrt(num)
        int i;
        int root = (int)sqrt((double)num);
        for(i=2; i<=root; i++){
            //Check fif divisible.
            if(num%i == 0){
                return 0;
            }
        }
        pthread_mutex_lock(&mutex);
        sum += num;
        primeCounter++;
//        printf("Prime %ld: %d", primeCounter, num);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    else{
        return 0;
    }
}

//function that checks if a given set of numbers are primes.
void* target(void* args){
//    struct threadArgsBuffer* buff = (struct threadArgsBuffer*)args;
    long* arr = (long*)args;
//    int size = buff->limit;

    //check the given array section for primes.
    int i;
    for(i=0; i<size || arr[i] == -1; i++){
        isPrime(arr[i]);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    struct timeval t1, t2;
    double elapsedTime;

    if(argc != 3) {
        printf("Too few arguments ");
        printf("USAGE: ./primeCalc <prime pivot> <num of random numbers>");
        exit(0);
    }

    int randomPivot = atoi(argv[1]);
    long numOfRandomNumbers = atol(argv[2]);
    randomNumbers = (long*) malloc(sizeof(long)*numOfRandomNumbers);

    //Check how many cores available.
    long numOfThread = get_nprocs_conf();

    //init random generator
    int random = rand();
    srand(randomPivot);

    // start timer
    gettimeofday(&t1, NULL);
    //generate random numbers and put them in array.

    int cnt =0;
    for (int i=0;i<numOfRandomNumbers;i++){
        random = rand();
        randomNumbers[i] = random;
        cnt++;
    }
    printf("Created numbers: %d\n", cnt);

    //Split the arr evenly between threads.
    size = (long)(numOfRandomNumbers/numOfThread);

    long *stops = (long*) malloc(sizeof(long)*numOfThread);
    for(long i=0; i<numOfThread; i++){
        stops[i] = i*size;
    }
    //create thread array of size numOfThread.
    pthread_t th[numOfThread];

    //Starting the thread.
    for(int i=0; i<numOfThread; i++){
        if(pthread_create(th + i, NULL, &target, &randomNumbers[stops[i]]) != 0){
            perror("Failed to create thread.");
        }
    }

    //Joining the thread in a different loop to make parallelism.
    for(int i=0; i<numOfThread; i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("Failed to join thread.");
        }
    }

    gettimeofday(&t2, NULL);
    double time_spent = (double)(t2.tv_sec -t1.tv_sec);
    //keep the out format as this!!
    printf("%ld,%ld\n",sum,primeCounter);
    printf("time spent: %lf\n", time_spent);

    exit(0);
}
