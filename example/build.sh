#!/bin/bash

gcc -I.. -I../queue ../queue/queue.c ../ts.c *.c -o test -ggdb
