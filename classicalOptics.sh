path1=
m=10
M=20
Epsilon=${10}
Epsilon1=${10}
MINPOINTS=${11}
UNDEFINED=10000000

gcc FilePartitionForSlidingWindowClassicalOPTICS.c -o filePartitionForOPTICS

./filePartitionForOPTICS $5 $7 $8
inputFileOne=$3
outputFileOne=$4
outputFileTwo=IntermediateOutputFile.txt
finalInputFile=IntermediateInputFile.txt
nodesVisitedFile=nodesVisitedInRtree_$5_$7_$8.txt
iterator=1
iterations=$8
lastTimeStamp=1
while [ $iterator -le $iterations ]
do
lastTimeStamp=$iterator
echo "hello\n"
echo "$lastTimeStamp\n"
inputFileTwo=$5"_partitions_"$7"_windowSize_"$8"_window_1_"$lastTimeStamp
echo "....$inputFileTwo\n"
sh profClassicalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9 $finalInputFile $finalInputFile

iterator=`expr $iterator + 1`

done
iterator=2
iterations=`expr $7 - $8 + 1`
lastTimeStamp=1
while [ $iterator -le $iterations ]
do
lastTimeStamp=`expr $iterator + $8 - 1`
echo "hello\n"
echo "$lastTimeStamp\n"
inputFileTwo=$5"_partitions_"$7"_windowSize_"$8"_window_"$iterator"_"$lastTimeStamp
echo "....$inputFileTwo\n"
sh profClassicalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 $iterator $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9 $finalInputFile $finalInputFile

iterator=`expr $iterator + 1`

done
cp IntermediateOutputFile.txt output_classical_optics_$5\_partition=$7\_windowSize=$8\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt
cp $nodesVisitedFile NodesVisited/nodesVisitedInRtree_classical_$5_$7_$8.txt
rm $5"_partitions_"*
rm $nodesVisitedFile
mv Profiling* ProfilingResults/
sh accumulateProfilingResultsForClassicalOptics.sh $5 $7 $8
echo "Unification completed on : `date`"
