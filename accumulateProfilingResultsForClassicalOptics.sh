dataFile=$1
noOfPartitions=$2
windowSize=$3
startPartition=1
endPartition=windowSize
echo "$dataFile \n"
echo "$noOfPartitions \n"
echo "$windowSize \n"
gcc   compilingProfResults.c  -o mainTime
mkdir AccumulatedProfResults

fileName=ProfilingResults/Profiling_result\[classical_$dataFile\_partitions_$noOfPartitions\_windowSize_$windowSize\_window_1_$startPartition\].txt
outputFileName=AccumulatedProfResults/ProfilingResults_classical_$dataFile\_$noOfPartitions\_$windowSize.txt
echo "$fileName"
./mainTime $fileName $outputFileName 0
 startPartition=`expr $startPartition + 1`

while [ $startPartition -le $windowSize ]
do
fileName=ProfilingResults/Profiling_result\[classical_$dataFile\_partitions_$noOfPartitions\_windowSize_$windowSize\_window_1_$startPartition\].txt
echo "$fileName"
./mainTime $fileName $outputFileName 1
 startPartition=`expr $startPartition + 1`
done

# fileName=ProfilingResults/Profiling_result\[classical_$dataFile\_partitions_$noOfPartitions\_windowSize_$windowSize\_window_1_$startPartition\].txt
# echo "$fileName"
# ./mainTime $fileName $outputFileName 1

# fileName=ProfilingResults/Profiling_result\[batchwise_$dataFile\_$noOfPartitions\_$windowSize\_1_$startPartition\].txt
# outputFileName=AccumulatedProfResults/ProfilingResults_batchwise_Deletion_$dataFile\_$noOfPartitions\_$windowSize.txt
# echo "$fileName"
# ./mainTime $fileName $outputFileName 0
#  startPartition=`expr $startPartition + 1`

while [ $startPartition -le $noOfPartitions ]
do
# fileName=ProfilingResults/Profiling_result\[batchwise_$dataFile\_$noOfPartitions\_$windowSize\_0_$startPartition\].txt
# outputFileName=AccumulatedProfResults/ProfilingResults_batchwise_Addition_$dataFile\_$noOfPartitions\_$windowSize.txt
# echo "$fileName"
# ./mainTime $fileName $outputFileName 1
temp=`expr $startPartition - $windowSize + 1`
fileName=ProfilingResults/Profiling_result\[classical_$dataFile\_partitions_$noOfPartitions\_windowSize_$windowSize\_window\_$temp\_$startPartition\].txt
# outputFileName=AccumulatedProfResults/ProfilingResults_batchwise_Deletion_$dataFile\_$noOfPartitions\_$windowSize.txt
echo "$fileName"
./mainTime $fileName $outputFileName 1
 startPartition=`expr $startPartition + 1`
done