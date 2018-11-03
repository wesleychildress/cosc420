#!/usr/bin/env python3.7

# randomArray.py
# mpirun -np 56 ./randomArray.py

from mpi4py import MPI
import sys

size = MPI.COMM_WORLD.Get_size()
rank = MPI.COMM_WORLD.Get_rank()
name = MPI.Get_processor_name()

# seed the pseudorandom number generator
from random import seed
from random import random
from random import randrange

# generate some random numbers
# print(randrange(1,1000))

sys.stdout.write(
    "process %d of %d with random # %d on %s.\n"
    % (rank, size, randrange(1,1000), name))