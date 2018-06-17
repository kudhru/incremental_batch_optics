make clean
make
# ./output 8 16 2.5 2.5 5 10000000 0 1 empty.txt output_empty.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_1 IntermediateOutputFile.txt 1
# ./output 8 16 2.5 2.5 5 10000000 0 2 IntermediateInputFile.txt IntermediateOutputFile.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_2 IntermediateOutputFile.txt 1
# ./output 8 16 2.5 2.5 5 10000000 0 3 IntermediateInputFile.txt IntermediateOutputFile.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_3 IntermediateOutputFile.txt 1
# ./output 8 16 2.5 2.5 5 10000000 0 4 IntermediateInputFile.txt IntermediateOutputFile.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_4 IntermediateOutputFile.txt 1
# ./output 8 16 2.5 2.5 5 10000000 0 5 IntermediateInputFile.txt IntermediateOutputFile.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_5 IntermediateOutputFile.txt 1
# ./output 8 16 2.5 2.5 5 10000000 0 6 IntermediateInputFile.txt IntermediateOutputFile.txt deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_6 IntermediateOutputFile.txt 1
# # cp IntermediateOutputFile.txt d4.txt
# # cp IntermediateInputFile.txt d3.txt
# ./output 8 16 2.5 2.5 5 10000000 1 7 IntermediateInputFile.txt IntermediateOutputFile.txt 1 IntermediateOutputFile.txt 1 deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_7
# # ./output 8 16 2.5 2.5 5 10000000 1 7 IntermediateInputFile.txt IntermediateOutputFile.txt 1 IntermediateOutputFile.txt 1 deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_7
# ./output 8 16 2.5 2.5 5 10000000 1 8 IntermediateInputFile.txt IntermediateOutputFile.txt 2 IntermediateOutputFile.txt 1 deluciaD32lac_shuffledData_0.1_sampled.txt_partition_20_8
cp IntermediateOutputFile.txt d1.txt
cp IntermediateInputFile.txt d2.txt
echo "running the last iteration\n"
./output 8 16 2.5 2.5 5 10000000 0 1 empty.txt output_empty.txt d2.txt IntermediateOutputFile.txt 1