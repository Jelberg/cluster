#!/bin/bash
#
#SBATCH --job-name=cluster_run
#SBATCH --output=cluster_run.%j.out
#SBATCH --ntasks=11
#

echo "----------INICIO PROCESO----------"

time prun ./run2
echo "-----------FIN PROCESO-----------"
