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
originalFile=${14}
noofpartitions=${15}
windowsize=${16}
tempString=${14}_${15}_${16}
echo "...........$tempString........."
# AffectedPointsCountFile=AffectedPointsCount_$14_$15_$16.txt
# IntersectionPointsCountFile=IntersectionPointsCount_$14_$15_$16.txt
# neighborhoodQueryCountInTree1File=neighborhoodQueryCountInTree1_$14_$15_$16.txt
# neighborhoodQueryCountInTree2File=neighborhoodQueryCountInTree2_$14_$15_$16.txt
# rdUndefPointsFile=rdUndefPoints_$14_$15_$16.txt
# rdUndefClusterPointsFile=rdUndefClusterPoints_$14_$15_$16.txt
echo "=========================================="
echo "Following code was profiled on : `date`"
echo "Script for Profiling serial Code using Vampir"
echo "\nFiles given->\n\t$2\n\t$3"
echo "=========================================="
echo
echo "\tCompiling Code..."
#make clean
#make -f $13
if [ $? -eq 1 ] ;	then
	echo "\tError occured during compilation\nTerminating script\n"
	exit 1
else
	echo "\n\tCompilation done successfully.\n"
fi
#executable file name
exename=output
temp=./temp_$tempString
rm -rf $temp
mkdir $temp

#export VT_ON=no
export VT_UNIFY=no
#export VT_MAX_FLUSHES=10
#export VT_BUFFER_SIZE=256M
export VT_MODE=STAT
export VT_PFORM_GDIR=$temp
export VT_PFORM_LDIR=$temp
export VT_FILE_PREFIX=${14}_$3
nm output>output_$tempString.nm
export VT_GNU_NMFILE=output_$tempString.nm
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
vtunify --stats $temp/${14}_$3
echo "###################3Profiling_result[pointwise_$tempString_$7_$8].txt########"
#cd ..
if [ ${17} -eq 0 ] ; then
	cp $temp/${14}_$3.prof.txt Profiling_result[pointwise_$tempString\_$7_$8].txt
elif [ ${17} -eq 1 ] ; then
	cp $temp/${14}_$3.prof.txt Profiling_result[batchwise_$tempString\_$7_$8].txt
else
	cp $temp/${14}_$3.prof.txt Profiling_result[classical_$tempString\_$7_$8].txt
fi

#rm -rf $temp

echo "Unification completed on : `date`"
