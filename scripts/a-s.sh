#!/usr/bin/env bash

#arguments: <file to assemble> <assembled file> <simulator output>

lc2k8_assembler $1 $2; 
lc2k8_simulator $2 > $3;
