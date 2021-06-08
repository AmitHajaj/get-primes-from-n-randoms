#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
/**
 * this file generate random numbers based on a user input.
 * it return the number of primes it generated and the sum of them.
 * The primality test i used is Rabin Miller primality test.
*/

pthread_mutex_t mutex;
long sum = 0;
long primeCounter = 0;
long *randomNumbers;
long size;

long witness(long a, long b, long mod)
{
    long x = 0,y = a % mod;

    while (b > 0)
    {
        if (b % 2 == 1)
        {    
            x = (x + y) % mod;
        }
        y = (y * 2) % mod;
        b /= 2;
    }
    return x % mod;
}

/* 
 * modular exponentiatiol
 * Compute the base^expo % mod
 */

long long modpow(long base, long exponent, long mod)
{
    long x = 1;
    long y = base;

    while (exponent > 0)
    {
        if (exponent % 2 == 1){
            x = (x * y) % mod;
        }
        y = (y * y) % mod;

        exponent = exponent / 2;
    }
    return x % mod;
}
/*
 * Rabin-Miller Primality test. 
 */

void* isprime(long numToCheck,int iteration)
{
    int i;
    long s;
    if (numToCheck < 2)
    {
        return 0;
    }

    if (numToCheck != 2 && numToCheck % 2==0)
    {
        return 0;
    }
    s = numToCheck - 1;
    while (s % 2 == 0)
    {
        s /= 2;
    }

    for (i = 0; i < iteration; i++)
    {
        long a = rand() % (numToCheck - 1) + 1, temp = s;
        long mod = modpow(a, temp, numToCheck);
        while (temp != numToCheck - 1 && mod != 1 && mod != numToCheck - 1)
        {
            mod = witness(mod, mod, numToCheck);
            temp *= 2;
        }

        if (mod != numToCheck - 1 && temp % 2 == 0)
        {
            return 0;
        }
    }
    pthread_mutex_lock(&mutex);
    sum += numToCheck;
    primeCounter++;
    pthread_mutex_unlock(&mutex);
    return 0;
}
//function that checks if a given set of numbers are primes.
void* target(void* args){
    long* arr = (long*)args;

    //check the given array section for primes.
    int i;
    for(i=0; i<size || arr[i] == -1; i++){
        isprime(arr[i], 5);
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
    int random;
    srand(randomPivot);

    //generate random numbers and put them in array.

    int cnt =0;
    for (int i=0;i<numOfRandomNumbers;i++){
        random = rand();
        randomNumbers[i] = random;
        cnt++;
    }

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

    //keep the out format as this!!
    printf("%ld,%ld\n",sum,primeCounter);

    exit(0);
}
