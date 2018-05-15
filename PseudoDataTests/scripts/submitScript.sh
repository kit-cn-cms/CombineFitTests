#!/bin/bash

echo "calling script..."

condor_qsub -cwd -S /bin/bash -V -o "$1"/log.out -e "$1"/log.err ./runPlotResults.sh "$1" "$2" "$3"
