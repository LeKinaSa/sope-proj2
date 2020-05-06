CC=gcc

# Compiler flags
CFLAGS=-Wall -Wextra -Wno-unused-parameter -pedantic -pthread -Ishared/

# Source files
SRCSU1=$(wildcard src-u1/*.c shared/*.c)
SRCSQ1=$(wildcard src-q1/*.c shared/*.c)

# Targets
PROGU1=U1
PROGQ1=Q1
PROGU2=U2
PROGQ2=Q2

all: u1 q1 u2 q2

u1:
	$(CC) $(CFLAGS) -o $(PROGU1) $(SRCSU1)

q1:
	$(CC) $(CFLAGS) -o $(PROGQ1) $(SRCSQ1)

u2:
	$(CC) $(CFLAGS) -o $(PROGU2) $(SRCSU1)

q2:
	$(CC) $(CFLAGS) -o $(PROGQ2) $(SRCSQ1)

clean:
	rm $(PROGU1) $(PROGQ1) $(PROGU2) $(PROGQ2)