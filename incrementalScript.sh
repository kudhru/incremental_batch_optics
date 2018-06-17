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
m=8
M=16
Epsilon=2.5
Epsilon1=2.5
MINPOINTS=5
UNDEFINED=10000000
echo "=========================================="
echo "Following code was profiled on : `date`"
echo "Script for Profiling serial Code using Vampir"
echo "\nFiles given->\n\t$2\n\t$3"
echo "=========================================="
echo
echo "\tCompiling Code..."
make clean
make -f $6
if [ $? -eq 1 ] ;	then
	echo "\tError occured during compilation\nTerminating script\n"
	exit 1
else
	echo "\n\tCompilation done successfully.\n"
fi
#executable file name
exename=output
temp=./temp
rm -rf $temp
mkdir $temp

gcc FilePartition.c -o filePartition
./filePartition $5 $7

gcc LastWindowDataGeneration.c -o lastgeneration
./lastgeneration $5 $7 $8	


#export VT_ON=no
export VT_UNIFY=no
#export VT_MAX_FLUSHES=10
#export VT_BUFFER_SIZE=256M
export VT_MODE=STAT
export VT_PFORM_GDIR=$temp
export VT_PFORM_LDIR=$temp
export VT_FILE_PREFIX=$2_$3
nm output>output.nm
export VT_GNU_NMFILE=output.nm
echo "\t\tRunning Code...\n"
echo "--" $path1$1"--\n"
echo "--" $2"--\n"
echo "--" $3"--\n"
echo "--" $4"--\n"
echo "--" $7"--\n"
#sonal di, execute your program here
inputFileOne=$3
outputFileOne=$4
inputFileTwo=$5"_partition_"$7"_1"
outputFileTwo=IntermediateOutputFile.txt
./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $9
iterator=2
inputFileOne=IntermediateInputFile.txt
outputFileOne=$outputFileTwo
while [ $iterator -le $8 ]
do

inputFileTwo=$5"_partition_"$7"_"$iterator

./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $9
 iterator=`expr $iterator + 1`
done
while [ $iterator -le $7 ]
do
inputFileTwo=`expr $iterator - $8`

./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 1 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $9

inputFileTwo=$5"_partition_"$7"_"$iterator

./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $9
 iterator=`expr $iterator + 1`
done
#while loop ends
if [ $9 -eq 0 ]; then
	cp IntermediateOutputFile.txt output_pointwise_incremetal_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
else
	cp IntermediateOutputFile.txt output_batchwise_incremetal_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
#gprof -z output > gprof_dataset-$2\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt 
fi
if [ $? -ne 0 ] ;	then
	echo "\tError occured. Terminating script...\n"
	exit 1
fi

echo "Exeuction Done on : `date`"
#cd $temp
vtunify --stats $temp/$2_$3
#cd ..
cp $temp/$2_$3.prof.txt Profiling_result[dataset-$5\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS].txt


#rm -rf $temp

echo "Unification completed on : `date`"

./filePartition $5_lastWindowData_partitions=$7_windowSize=$8.txt 1
lastWindowFile=$5_lastWindowData_partitions=$7_windowSize=$8.txt_partition_1_1
if [ $9 -eq 0 ]; then
./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $3 $4 $lastWindowFile output_pointwise_incremetal_optics_$lastWindowFile\_partition=$1\_windowSize=$1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $9
else
./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $3 $4 $lastWindowFile output_batchwise_incremetal_optics_$lastWindowFile\_partition=$1\_windowSize=$1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $9	
fi
rm $5"_partition_"$7*
rm $lastWindowFile

echo "End of Script\n"
