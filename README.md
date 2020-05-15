# SOPE - Project 2, Group T2G04
First part of the second project of the Operating Systems curricular unit.

* Clara Martins (up201806528)
* Daniel Monteiro (up201806185)
* Gonçalo Pascoal (up201806332)

## Implemented features
* Command line argument parsing;
* Thread creation in order to generate and deal with bathroom requests;
* Logging of `IWANT`, `RECVD`, `ENTER`, `IAMIN`, `TIMUP`, `2LATE`, `CLOSD` and `GAVUP` and `FAILD`;
* Support for a limited amount of threads and places.

## Features that don't work
* Some unexpected behaviour may occur when using very high values for the `nsecs` argument.

## Relevant details
* The threads run detached from the main thread (using `pthread_detach`) so that the number of threads is limited only by the operating system;
* Alarms were used to control the runtime of both processes;
* We also installed a handler for `SIGPIPE`, which, on some occasions, when both programs were being run in the same console, would terminate the client process early;
* To control the amount of threads executing simultaneously we used a semaphore initialized to `nthreads`. Before creating each thread we call `sem_wait`. Each thread calls `sem_post` immediately before returning. This means that if there are already `nthreads` executing, the main thread will wait until one of them returns;
* To control the amount of places in the bathroom, we used a stack that stores the place numbers. Initially, we place the numbers from 0 to `nplaces` - 1 on the stack, so that the stack has size `nplaces`. Each thread removes the next place number from the stack and, when the time for the request is up, places it back in the stack (these operations are performed inside a critical section, to prevent synchronization errors). If the stack is empty, the thread waits for a condition variable that is notified each time a place number is put back into the stack.
