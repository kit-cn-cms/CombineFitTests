#!/bin/bash

njobs=$1
paths=${@:2}

for path in $paths
do
	cmd="qsub -q default.q -cwd -terse -t 1-$njobs -S /bin/bash -l h=bird* -hard -l os=sld6 -l h_vmem=2000M -l s_vmem=2000M -o /dev/null -e /dev/null $path"
	echo $cmd
	eval $cmd
done
