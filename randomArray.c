/******************************************************************************
* random_array.c
* DESCRIPTION: 
*   -  Master generates arrary of N randoms 
*   -  Distributes to cluster N/sizeOfWorld
*   -  Checks for even parity
*   -  Checks for primes
*   -  Checks frequncy of randoms
*   @@@@@@@@@  sizeOfWorld must be even  @@@@@@@@@
* AUTHORS: Alieu, Andre, Wezley
* LAST REVISED: 11/06/18
****************************************************************************/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#define  ARRAYSIZE	1000
#define  MASTER		0

int data[ARRAYSIZE];
int freq[ARRAYSIZE];
int primes[ARRAYSIZE];
int primed;
int parity;

int findParity(int, int, int);
int findPrime(int, int, int);
void frequency(int, int, int);

int main (int argc, char *argv[])
{
    int numtasks, taskid, rc, dest, offset, i, j, tag1, tag2, source, chunksize; 
    int prime, parity, myprime, myparity;
    MPI_Status status;

    /***** Initializations *****/
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    if (ARRAYSIZE % numtasks != 0) 
    {
        printf("Quitting. Array size of 100 tasks must be divisible by world size.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(0);
    }
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    printf ("MPI task %d has started...\n", taskid);    
    chunksize = (ARRAYSIZE / numtasks);
    tag2 = 1;
    tag1 = 2;

    /***** Master task only ******/
    if (taskid == MASTER) //Initialize the array
    {
        
        for(i=0; i<ARRAYSIZE; i++)
        {
            data[i] =  arc4random_uniform(10000);
            // printf("Initialized array position %d = %d\n", i, data[i]);
        }
        /* Send each task its portion of the array - master keeps 1st part */
        offset = chunksize;
        for (dest=1; dest<numtasks; dest++)
        {
            
            MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD); // offset 
            MPI_Send(&data[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD); // array of randoms
            MPI_Send(&parity, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD); // count of even parity
            MPI_Send(&prime, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);  // count of primes
            MPI_Send(&primes[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD);
            MPI_Send(&freq[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD); // array for frequency
            
            printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
            offset = offset + chunksize;
        }
        
        /* Master does its part of the work */
        offset = 0;
        myparity = findParity(offset, chunksize, taskid); /* find even parity function */
        myprime  = findPrime(offset, chunksize, taskid); /* find primes function */
        frequency(offset, chunksize, taskid); /* parity function */
        
        /* Wait to receive results from each task */
        for (i=1; i<numtasks; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status); // offset
            MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status); // data array
            MPI_Recv(&parity, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status); // count of even parity
            MPI_Recv(&prime, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);  // count of primes
            MPI_Recv(&primes[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);
            MPI_Recv(&freq[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status); // array for frequency
        }
        
        /* Get final sum and print sample results */  
        MPI_Reduce(&myparity, &parity, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
        MPI_Reduce(&myprime, &prime, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
        
        printf("\nFrequency of all elements of array : \n");
        for(i=0; i<ARRAYSIZE; i++)
        {
            if(freq[i] != 0)
            {
                printf("%-4d occurs %-d times\n", data[i], freq[i]);
            }
        }
        
        printf("Randoms generated of even parity : %d\n", parity);
        printf("Randoms generated that are prime : %d\n", prime);
        if(prime<1)
        {
            printf("List of Primes : NONE\n");
        }
        else
        {
            printf("List of Primes : \n");
            for(i=0; i<prime; i++)
            {
                printf("%-4d\n", primes[i]);
            }
        }

  }  /* end of master section */



/***** Non-master tasks only *****/

if (taskid > MASTER) {

    /* Receive my portion of array from the master task */
    source = MASTER;
    MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
    MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);
    MPI_Recv(&parity, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);// count of even parity
    MPI_Recv(&prime, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status); // count of primes
    MPI_Recv(&primes[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);
    MPI_Recv(&freq[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status); // array for frequency

    /* find even parity function */
    myparity = findParity(offset, chunksize, taskid);
    /* find primes function */
    myprime  = findPrime(offset, chunksize, taskid);
    /* parity function */
    frequency(offset, chunksize, taskid);
    
    /* Send my results back to the master task */
    dest = MASTER;
    MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
    MPI_Send(&data[offset], chunksize, MPI_FLOAT, MASTER, tag2, MPI_COMM_WORLD);
    MPI_Send(&parity, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
    MPI_Send(&prime, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
    MPI_Send(&primes[offset], chunksize, MPI_FLOAT, MASTER, tag2, MPI_COMM_WORLD);
    MPI_Send(&freq[offset], chunksize, MPI_FLOAT, MASTER, tag2, MPI_COMM_WORLD); // array for frequency
    MPI_Reduce(&myparity, &parity, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    MPI_Reduce(&myprime, &prime, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    

  } /* end of non-master */


MPI_Finalize();

}   /* end of main */


// Finds even parity numbers
int findParity(int myoffset, int chunk, int myid)
{
  int count = 0;
  int i;
  for(i = myoffset; i < myoffset + chunk; i++) {
    if(data[i] % 2 == 0)
      count++;
  }
  return count;
}
    
// Finds prime numbers or not
int findPrime(int myoffset, int chunk, int myid)
{
  int count = 0;
  int i, k, n;
  int flag = 0;

  for(i = myoffset; i < myoffset + chunk; i++)
  {
    n = data[i];
    for(k = 2; k <= n/2; ++k)
    {
      //Condition for non primes
      if(n%k == 0) {
        flag = 1;
        break;
      }
    }
    if (n == 1) {

    }
    else 
    {
        if(flag == 0)
        {
            primes[count]=data[i];
            count++;
        }
    }
  }

  return count;
}
    
// Finds frequency of numbers or
void frequency(int myoffset, int chunk, int myid)
{
    int i, j, count;

    for(i = myoffset; i < myoffset + chunk; i++) // Initially initialize frequencies to -1
    {
        freq[i] = -1;
    }
    for(i = myoffset; i < myoffset + chunk; i++)
    {
        count = 1;
        for(j=i+1; j < myoffset + chunk; j++)
        {
            /* If duplicate element is found */
            if(data[i]==data[j])
            {
                count++;

                /* Make sure not to count frequency of same element again */
                freq[j] = 0;
            }
        }

        /* If frequency of current element is not counted */
        if(freq[i] != 0)
        {
            freq[i] = count;
        }
    }
}

