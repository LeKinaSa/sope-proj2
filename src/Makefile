CC=gcc

# Compiler flags
CFLAGS=-Wall -Wextra -pedantic -pthread

# Source files
SRCSU1=$(wildcard src-u1/*.c)
SRCSQ1=$(wildcard src-q1/*.c)

# Targets
PROGU1=U1
PROGQ1=Q1

all:
	u1 q1

u1:
	$(CC) $(CFLAGS) -o $(PROGU1) $(SRCSU1)

q1:
	$(CC) $(CFLAGS) -o $(PROGQ1) $(SRCSQ1)

clean:
	rm $(PROGU1) $(PROGQ1)