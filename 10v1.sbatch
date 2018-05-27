#!/bin/bash
#
#SBATCH --job-name=cluster_run
#SBATCH --output=cluster_run.%j.out
#SBATCH --ntasks=11
#

echo "----------INICIO PROCESO----------"
pdsh -w node1 cp /home/public/txts/libro_medicina.txt /local_home/jelberg.12/
pdsh -w node1 cp /home/public/txts/modificado/palabras_libro_medicina.txt /local_home/jelberg.12/

time prun ./run
echo "-----------FIN PROCESO-----------"
