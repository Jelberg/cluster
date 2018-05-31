#!/bin/bash
#
#SBATCH --job-name=cluster1_run
#SBATCH --output=cluster1_run.%j.out
#SBATCH --ntasks=6
#

echo "----------INICIO PROCESO----------"
pdsh -w node[1-12] rm /local_home/jelberg.12/*.txt

pdsh -w node1 cp /home/public/txts/libro_medicina.txt /local_home/jelberg.12/
pdsh -w node1 cp /home/public/txts/modificado/palabras_libro_medicina.txt /local_home/jelberg.12/

time prun ./run
echo "-----------FIN PROCESO-----------"
