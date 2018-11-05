// Author: Wes Kendall
// Copyright 2012 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Program that computes the average of an array of elements in parallel using
// MPI_Scatter and MPI_Gather
//
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
  int count;
  int i;
  for(i = 0; i < num_elements; i++) {
    if(array[i] % 2 == 0)
      count++;
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
  // Problem is how to find if the the elements in the sub array are prime
  // and creating the frequency table
  int sub_parity = compute_parity(sub_rand_nums, num_elements_per_proc);

  // Gather all partial averages down to the root process
  int *sub_paritys = NULL;
  if (world_rank == 0) {
    sub_paritys = (int *)malloc(sizeof(int) * world_size);
    assert(sub_paritys != NULL);
  }
  MPI_Gather(&sub_parity, 1, MPI_INT, sub_paritys, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Now that we have all of the partial averages on the root, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
  if (world_rank == 0) {
    //int avg = compute_avg(sub_avgs, world_size);
    //printf("Avg of all elements is %d\n", avg);

    int countParity = compute_parity(sub_paritys, world_size);
    printf("Total count of even parity numbers %d\n", countParity);

  }

  // Clean up
  if (world_rank == 0) {
    free(rand_nums);
    //free(sub_avgs);
    free(sub_paritys);
  }
  free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
