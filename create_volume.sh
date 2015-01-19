#!/bin/bash


export CURRENT_VOLUME=0
./frmt.bin ; ./mkvol.bin -fs 1 -fc 0 -s 10 ; ./mknfs.bin "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
