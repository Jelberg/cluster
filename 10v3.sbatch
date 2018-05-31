#!/bin/bash
#
#SBATCH --job-name=cluster3_run
#SBATCH --output=cluster3_run.%j.out
#SBATCH --ntasks=6
#

pdsh -w node[2-12] rm /local_home/jelberg.12/libro_medicina.txt

echo "----------INICIO PROCESO----------"


time prun ./run3


echo "-----------FIN PROCESO-----------"

pdsh -w node[2-12] rm /local_home/jelberg.12/*.txt