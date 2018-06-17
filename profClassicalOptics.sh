#
#	This script profiles MPI code using VampirTrace
#
#	File dependence on 	:	makefile	(Instrumented according to VampirTrace)
#	Input		c		:	Dimesion of Input data,1 input file and 1 output file passed as command line arguments
#	Output 				:	Profiling_result[dataset-$2_m=$m_M=$M_EPS=$Epsilon_MINPOINTS=$MINPOINTS].txt
#
#	Author 				:	Saiyedul Islam
#		 					saiyedul.islam@gmail.com
#
#parameters for running code
path1=
m=$1
M=$2
Epsilon=$3
Epsilon1=$4
MINPOINTS=$5
UNDEFINED=$6
echo "=========================================="
echo "Following code was profiled on : `date`"
echo "Script for Profiling serial Code using Vampir"
echo "\nFiles given->\n\t$2\n\t$3"
echo "=========================================="
echo
echo "\tCompiling Code..."
make clean
make -f ${13}
# if [ $? -eq 1 ] ;	then
# 	echo "\tError occured during compilation\nTerminating script\n"
# 	exit 1
# else
# 	echo "\n\tCompilation done successfully.\n"
# fi
#executable file name
exename=output
temp=./temp
rm -rf $temp
mkdir $temp

#export VT_ON=no
export VT_UNIFY=no
#export VT_MAX_FLUSHES=10
#export VT_BUFFER_SIZE=256M
export VT_MODE=STAT
export VT_PFORM_GDIR=$temp
export VT_PFORM_LDIR=$temp
export VT_FILE_PREFIX=$9_$3
nm output>output.nm
export VT_GNU_NMFILE=output.nm
echo "\t\tRunning Code...\n"
#sonal di, execute your program here
# inputFileOne=$3
# outputFileOne=$4
# inputFileTwo=$5"_partition_"$7"_1"
# outputFileTwo=IntermediateOutputFile.txt
echo "running executive\n"
./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED $7 $8 $9 ${10} ${11} ${12} ${17} ${18} ${19} ${14} ${15} ${16}
echo "running executive completed\n"

if [ $? -ne 0 ] ;	then
	echo "\tError occured. Terminating script...\n"
	exit 1
fi

echo "Exeuction Done on : `date`"
#cd $temp
vtunify --stats $temp/$9_$3
#cd ..
	cp $temp/$9_$3.prof.txt Profiling_result[classical_${11}].txt


#rm -rf $temp

echo "Unification completed on : `date`"
