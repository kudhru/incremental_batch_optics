path1=
m=8
M=
Epsilon=1.5
Epsilon1=1.5
MINPOINTS=3
UNDEFINED=10000000

gcc FilePartitionForLandmarkClassicalOPTICS.c -o filePartitionForLandmarkOPTICS

./filePartitionForLandmarkOPTICS $5 $7 $8

inputFileOne=$3
outputFileOne=$4
outputFileTwo=IntermediateOutputFile.txt
iterator=1
iterations=$7
lastTimeStamp=1
while [ $iterator -le $iterations ]
do
lastTimeStamp=$iterator
echo "hello\n"
echo "$lastTimeStamp\n"
inputFileTwo=$5"_partitions_"$7"_window_1_"$lastTimeStamp
echo "....$inputFileTwo\n"
sh profClassicalOptics.sh $m $M $Epsilon $Epsilon1 $MINPOINTS $UNDEFINED 0 1 $inputFileOne $outputFileOne $inputFileTwo $outputFileTwo $6 $5 $7 $8 $9

iterator=`expr $iterator + 1`

done
cp IntermediateOutputFile.txt output_landmark_classical_optics_$5\_partition=$7\_m=$m\_M=$M\_EPS=$Epsilon\_MINPOINTS=$MINPOINTS.txt

rm $5"_partitions_"*
mv Profiling* ProfilingResults/
echo "Unification completed on : `date`"
