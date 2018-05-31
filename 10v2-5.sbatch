#!/bin/bash
#
#SBATCH --job-name=cluster_run2-5
#SBATCH --output=cluster_run2-5.%j.out
#SBATCH --ntasks=6
#

echo "----------INICIO PROCESO----------"

time prun ./run2-5
echo "-----------FIN PROCESO-----------"