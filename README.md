# SOPE - Project 2, Group T2G04
First part of the second project of the Operating Systems curricular unit.

* Clara Martins (up201806528)
* Daniel Monteiro (up201806185)
* Gonçalo Pascoal (up201806332)

## Implemented features
* Command line argument parsing;
* Thread creation in order to generate and deal with bathroom requests;
* Logging of `IWANT`, `RECVD`, `ENTER`, `IAMIN`, `TIMUP`, `2LATE`, `CLOSD` and `GAVUP` and `FAILD`.

## Features that don't work
* Some unexpected behaviour may occur when using very high values for the `nsecs` argument.
* If the server can’t open the client FIFO, the client thread may get stuck in the read function call.

## Relevant details
* The threads run detached from the main thread (using `pthread_detach`) so that the number of threads is limited only by the operating system;
* Alarms were used to control the runtime of both processes;
* We also installed a handler for SIGPIPE, which, on some occasions, when both programs were being run in the same console, would terminate the client process early.
