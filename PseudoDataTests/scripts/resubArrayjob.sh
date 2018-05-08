#!/bin/bash

njobs=$1
queue=$2
paths=${@:3}

for path in $paths
do
	cmd="condor_qsub -cwd -terse -t 1-$njobs -S /bin/bash -o /dev/null -e /dev/null $path"
	echo $cmd
	eval $cmd
done
