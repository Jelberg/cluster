#!/bin/bash
#
#SBATCH --job-name=ordena_run
#SBATCH --output=orena_run.%j.out
#SBATCH --ntasks=6
#

echo "----------INICIO PROCESO----------"

time prun ./ordena
echo "-----------FIN PROCESO-----------"