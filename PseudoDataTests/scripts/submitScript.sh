#!/bin/bash

echo "calling script..."

qsub -q short.q -l h=bird* -hard -l os=sld6 -l h_vmem=5000M -l s_vmem=5000M -cwd -S /bin/bash -V -o "$1"/log.out -e "$1"/log.err ./runPlotResults.sh "$1" "$2"
