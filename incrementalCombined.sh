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
m=10
M=20
Epsilon=${10}
Epsilon1=${10}
MINPOINTS=${11}
UNDEFINED=10000000

#gcc FilePartition.c -o filePartition
./filePartition $5 $7

#gcc LastWindowDataGeneration.c -o lastgeneration
./lastgeneration $5 $7 $8	

./filePartition $5_lastWindowData_partitions=$7_windowSize=$8.txt 1
lastWindowFile=$5_lastWindowData_partitions=$7_windowSize=$8.txt_partition_1_1

inputFileOne=$3
outputFileOne=$4
inputFileTwo=$5"_partition_"$7"_1"
outputFileTwo=IntermediateOutputFile_$5_$7_$8.txt
finalInputFile=IntermediateInputFile_$5_$7_$8.txt
AffectedPointsCountFile=AffectedPointsCount_$5_$7_$8.txt
IntersectionPointsCountFile=IntersectionPointsCount_$5_$7_$8.txt
neighborhoodQueryCountInTree1File=neighborhoodQueryCountInTree1_$5_$7_$8.txt
neighborhoodQueryCountInTree2File=neighborhoodQueryCountInTree2_$5_$7_$8.txt
rdUndefPointsFile=rdUndefPoints_$5_$7_$8.txt
rdUndefClusterPointsFile=rdUndefClusterPoints_$5_$7_$8.txt
nodesVisitedFile=nodesVisitedInRtree_$5_$7_$8.txt
sh incrementalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9 $inputFileTwo $finalInputFile
#./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo
iterator=2
inputFileOne=$finalInputFile
outputFileOne=$outputFileTwo
while [ $iterator -le $8 ]
do

inputFileTwo=$5"_partition_"$7"_"$iterator
sh incrementalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9 $inputFileTwo $finalInputFile
# ./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo
 iterator=`expr $iterator + 1`
done
while [ $iterator -le $7 ]
do
timestampToBeDeleted=`expr $iterator - $8`
inputFileTwo=$5"_partition_"$7"_"$iterator
sh incrementalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 1 $iterator $inputFileOne $outputFileOne $timestampToBeDeleted $outputFileTwo $6 $5 $7 $8 $9 $inputFileTwo $finalInputFile
# ./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 1 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo



# sh incrementalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9

#./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo
 iterator=`expr $iterator + 1`
done
#while loop ends
# cp IntermediateOutputFile.txt output_incremetal_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
if [ $9 -eq 0 ]; then
	cp $outputFileTwo OutputResults/output_pointwise_incremetal_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
	cp $AffectedPointsCountFile AffectedPoints/AffectedPointsCount_pointwise_$5_$7_$8.txt
	#cp IntersectionPointsCount.txt IntersectionPoints/IntersectionPointsCount_pointwise_$5_$7_$8.txt
	cp $neighborhoodQueryCountInTree1File NeighborhoodQueryCount/NeighborhoodQueryCountInTree1_pointwise_$5_$7_$8.txt
	cp $nodesVisitedFile NodesVisited/nodesVisitedInRtree_pointwise_$5_$7_$8.txt
	#cp neighborhoodQueryCountInTree2.txt NeighborhoodQueryCount/NeighborhoodQueryCountInTree2_pointwise_$5_$7_$8.txt
elif [ $9 -eq 1 ]; then
	cp $outputFileTwo OutputResults/output_batchwise_incremetal_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
	cp $AffectedPointsCountFile AffectedPoints/AffectedPointsCount_batchwise_$5_$7_$8.txt
	cp $IntersectionPointsCountFile IntersectionPoints/IntersectionPointsCount_batchwise_$5_$7_$8.txt
	cp $neighborhoodQueryCountInTree1File NeighborhoodQueryCount/NeighborhoodQueryCountInTree1_batchwise_$5_$7_$8.txt
	cp $neighborhoodQueryCountInTree2File NeighborhoodQueryCount/NeighborhoodQueryCountInTree2_batchwise_$5_$7_$8.txt
	if [ `expr $7 - $8` -gt 0  ]; then
		cp $rdUndefPointsFile rdUndefCount/rdUndefPoints_batchwise_$5_$7_$8.txt
		cp $rdUndefClusterPointsFile rdUndefCount/rdUndefClusterPoints_batchwise_$5_$7_$8.txt
	fi
	cp $nodesVisitedFile NodesVisited/nodesVisitedInRtree_batchwise_$5_$7_$8.txt
else
	cp $outputFileTwo OutputResults/output_classical_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
	#cp $AffectedPointsCountFile AffectedPoints/AffectedPointsCount_batchwise_$5_$7_$8.txt
	#cp $IntersectionPointsCountFile IntersectionPoints/IntersectionPointsCount_batchwise_$5_$7_$8.txt
	#cp $neighborhoodQueryCountInTree1File NeighborhoodQueryCount/NeighborhoodQueryCountInTree1_batchwise_$5_$7_$8.txt
	#cp $neighborhoodQueryCountInTree2File NeighborhoodQueryCount/NeighborhoodQueryCountInTree2_batchwise_$5_$7_$8.txt
	#if [ `expr $7 - $8` -gt 0  ]; then
	#	cp $rdUndefPointsFile rdUndefCount/rdUndefPoints_batchwise_$5_$7_$8.txt
	#	cp $rdUndefClusterPointsFile rdUndefCount/rdUndefClusterPoints_batchwise_$5_$7_$8.txt
	#fi
	cp $nodesVisitedFile NodesVisited/nodesVisitedInRtree_classicalOptics_$5_$7_$8.txt

fi
#gprof -z output > gprof_dataset-$2\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt 

# if [ $? -ne 0 ] ;	then
# 	echo "\tError occured. Terminating script...\n"
# 	exit 1
# fi

echo "Exeuction Done on : `date`"
#cd $temp
#vtunify --stats $temp/$2_$3
#cd ..
#cp $temp/$2_$3.prof.txt Profiling_result[dataset-$5\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS].txt

#rm -rf $temp
# # make -f $6
# if [ $? -eq 1 ] ;	then
# 	echo "\tError occured during compilation\nTerminating script\n"
# 	exit 1
# else
# 	echo "\n\tCompilation done successfully.\n"
# fi

# exename=output
 temp=./temp_$5_$7_$8
 rm -rf $temp
 mkdir $temp

#export VT_ON=no
export VT_UNIFY=no
#export VT_MAX_FLUSHES=10
#export VT_BUFFER_SIZE=256M
export VT_MODE=STAT
export VT_PFORM_GDIR=$temp
export VT_PFORM_LDIR=$temp
export VT_FILE_PREFIX=$2_$3
nm output > output_$5_$7_$8.nm
export VT_GNU_NMFILE=output_$5_$7_$8.nm
echo "\t\tRunning Code...\n"

# if [ $9 -eq 0 ]; then
# ./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $3 $4 $lastWindowFile OutputResults/output_pointwise_incremetal_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $9 random.txt $finalInputFile
# else
./output $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $3 $4 $lastWindowFile OutputResults/output_incremetal_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $9 random.txt $finalInputFile
# fi
rm $5"_partition_"$7*
rm $lastWindowFile
#rm rdUndef*
mv Profiling* ProfilingResults/
rm $5_lastWindowData_partitions=$7_windowSize=$8.txt
#rm neighborhoodQueryCountInTree*
rm output_$5_$7_$8.nm
rm -rf $temp
rm $IntersectionPointsCountFile
rm $AffectedPointsCountFile
rm $neighborhoodQueryCountInTree1File
rm $neighborhoodQueryCountInTree2File
rm $nodesVisitedFile
rm $rdUndefPointsFile
rm $rdUndefClusterPointsFile
rm $finalInputFile

#gcc incremental_extract.c -o incrementalWithNonCore
if [ $9 -eq 0 ]; then
	./incrementalWithNonCore $outputFileTwo $Epsilon > ClusterResults/clusterResults_pointwise_incremental_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
	# ./incrementalWithNonCore OutputResults/output_pointwise_incremetal_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $Epsilon > ClusterResults/clusterResults_pointwise_incremental_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
elif [ $9 -eq 1 ]; then
	./incrementalWithNonCore $outputFileTwo $Epsilon > ClusterResults/clusterResults_batchwise_incremental_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
	# ./incrementalWithNonCore OutputResults/output_batchwise_incremetal_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $Epsilon > ClusterResults/clusterResults_batchwise_incremental_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
else
	./incrementalWithNonCore $outputFileTwo $Epsilon > ClusterResults/clusterResults_classical_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
fi
 ./incrementalWithNonCore OutputResults/output_incremetal_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt $Epsilon > ClusterResults/clusterResults_batchwise_incremental_optics_$lastWindowFile\_partition=1\_windowSize=1\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
rm $outputFileTwo
if [ $9 -eq 0 ]; then
	sh accumulateProfilingResultsForPointwiseIncOptics.sh $5 $7 $8
elif [ $9 -eq 1 ]; then	
	sh accumulateProfilingResultsForBatchwiseIncOptics.sh $5 $7 $8
else
	sh accumulateProfilingResultsForClassicalOpticsNew.sh $5 $7 $8
fi	
echo "Unification completed on : `date`"
