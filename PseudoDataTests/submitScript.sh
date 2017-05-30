#!/bin/bash

echo "calling script..."

qsub -q default.q -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -V -o "$1"/log.out -e "$1"/log.err ./submitPlotResults.sh "$1" "$2"
