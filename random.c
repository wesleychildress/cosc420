/* random.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
int *create_rand_nums(int num_elements, int range_limit) {
  int *rand_nums = (int *)malloc(sizeof(int) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = rand() % range_limit + 1;
    printf("%d\n", rand_nums[i]);
  }
  return rand_nums;
}

// Computes the average of an array of numbers
int compute_avg(int *array, int num_elements) {
  int sum = 0;
  int i;
  for (i = 0; i < num_elements; i++) {
    sum += array[i];
  }
  return sum / num_elements;
}

// Finds if element has an even sub_parity
int compute_parity(int *array, int num_elements) {
  int count = 0;
  int i;
  for(i = 0; i < num_elements; i++) {
    if(array[i] % 2 == 0)
      count++;
  }
  return count;
}

// Finds if an element is a prime number or not
int find_prime(int *array, int num_elements) {
  int count = 0;
  int i, k, n;
  int flag = 0;

  for(i = 0; i < num_elements; i++) {
    n = array[i];
    for(k = 2; k <= n/2; ++k) {
      //Condition for non primes
      if(n%k == 0) {
        flag = 1;
        break;
      }
    }
    if (n == 1) {

    }
    else {
      if(flag == 0){
        count++;
      }
    }
  }

  return count;
}

int main(int argc, char** argv) {

  if (argc != 3) {
    fprintf(stderr, "Usage: avg num_elements_per_proc\n");
    exit(1);
  }

  int num_elements_per_proc = atoi(argv[1]);
  int range_limit = atoi(argv[2]);
  int freq[range_limit];
  int i;

  // Seed the random number generator to get different results each time
  srand(time(NULL));

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Create a random array of elements on the root process. Its total
  // size will be the number of elements per process times the number
  // of processes
  int *rand_nums = NULL;
  if (world_rank == 0) {
    rand_nums = create_rand_nums(num_elements_per_proc * world_size, range_limit);
  }



  // For each process, create a buffer that will hold a subset of the entire
  // array
  int *sub_rand_nums = (int *)malloc(sizeof(int) * num_elements_per_proc);
  assert(sub_rand_nums != NULL);


  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
              num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  // Finds the parity of each sub-array
  int sub_parity = compute_parity(sub_rand_nums, num_elements_per_proc);

  // Gather all partial averages down to the root process
  int *sub_paritys = NULL;
  if (world_rank == 0) {
    sub_paritys = (int *)malloc(sizeof(int) * world_size);
    assert(sub_paritys != NULL);
  }
  MPI_Gather(&sub_parity, 1, MPI_INT, sub_paritys, 1, MPI_INT, 0, MPI_COMM_WORLD);


  //Checks to see if the sub arrays are prime numbers or not.
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
              num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  // Finds if an element is a prime number
  int sub_prime = find_prime(sub_rand_nums, num_elements_per_proc);

  // Gather all partial averages down to the root process
  int *sub_primes = NULL;
  if (world_rank == 0) {
    sub_primes = (int *)malloc(sizeof(int) * world_size);
    assert(sub_primes != NULL);
  }
  MPI_Gather(&sub_prime, 1, MPI_INT, sub_primes, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
              num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  for(i = 0; i < num_elements_per_proc; i++) {
    freq[sub_rand_nums[i]]++;
  }

  //MPI_Gather(&sub_parity, 1, MPI_INT, sub_paritys, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    int i, k;
    int countParity = 0;
    int countPrimes = 0;
    for(i = 0; i < world_size; i++) {
      countParity = countParity + sub_paritys[i];
    }
    printf("Total count of even parity numbers %d\n", countParity);

    for(i = 0; i < world_size; i++) {
      countPrimes = countPrimes + sub_primes[i];
    }
    printf("Total count of prime numbers %d\n", countPrimes);

    for(i = 0; i <= range_limit; i++) {
      printf("%d: %d\n", i, freq[i]);
    }
  }

  // Clean up
  if (world_rank == 0) {
    free(rand_nums);
    //free(sub_avgs);
    free(sub_paritys);
    free(sub_primes);
  }
  free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
