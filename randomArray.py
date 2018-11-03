#!/usr/bin/env python3.7

"""
mpirun -np 56 ./randomArray.py
"""

from mpi4py import MPI
import sys



# seed the pseudorandom number generator
from random import seed
from random import random
from random import randrange
# seed random number generator
# generate some random numbers
print(random(), random(), random())
# reset the seed
# generate some random numbers
print(random(), random(), random())
1
2
3
4
5
6
7
8
9
10
11
# seed the pseudorandom number generator
from random import seed
from random import random
# seed random number generator
# generate some random numbers
print(random(), random(), random())
# reset the seedx
# generate some random numbers
print(random(), random(), random())

print(randrange(1,1000))