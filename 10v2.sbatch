#!/bin/bash
#
#SBATCH --job-name=cluster2_run
#SBATCH --output=cluster2_run.%j.out
#SBATCH --ntasks=6
#

echo "----------INICIO PROCESO----------"

time prun ./run2
echo "-----------FIN PROCESO-----------"
