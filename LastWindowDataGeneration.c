#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
int main( int argc, char** argv)
{
	char partitionedFilename[1000];
	double tempRead;
	FILE *fp= fopen(argv[1], "r"),*fpartition;
	assert(fp!=NULL);
	int noOfPoints,noOfDim,i,noOfPartitions=atoi(argv[2]),sizeOfPartition,j,k,windowSize=atoi(argv[3]),sizeOfDataFile;
	fscanf(fp,"%d\n%d\n",&noOfPoints,&noOfDim);
	sizeOfPartition = noOfPoints / noOfPartitions ;
	for(i=0;i < noOfPartitions - windowSize ;i++)
	{
		// sprintf(partitionedFilename,"%s_partition_%d",argv[1],i+1);
		// fpartition=fopen(partitionedFilename,"w");
		// assert(fpartition!=NULL);
		// fprintf(fpartition, "%d\n%d",sizeOfPartition,noOfDim );
		for(j = 0;j < sizeOfPartition;j++ )
		{
			//fprintf(fpartition, "\n" );
			for(k=0;k<noOfDim;k++)
			{
				fscanf(fp,"%lf",&tempRead);
				//fprintf(fpartition, "%lf ",tempRead );
			}
		}
		//fclose(fpartition);
	}
	sprintf(partitionedFilename,"%s_lastWindowData_partitions=%d_windowSize=%d.txt",argv[1],noOfPartitions,windowSize);
	fpartition=fopen(partitionedFilename,"w");
	assert(fpartition!=NULL);
	sizeOfDataFile= noOfPoints - sizeOfPartition*(noOfPartitions- windowSize);
	fprintf(fpartition,"%d\n%d",sizeOfDataFile,noOfDim);
	for(i=0;i < sizeOfDataFile ;i++)
	{
			fprintf(fpartition, "\n" );
			for(k=0;k<noOfDim;k++)
			{
				fscanf(fp,"%lf",&tempRead);
				fprintf(fpartition, "%lf ",tempRead );
			}
		
		//fclose(fpartition);
	}
	fclose(fpartition);
	// for(j = (noOfPartitions-1) * sizeOfPartition ;j < noOfPoints;j++ )
	// {
	// 	fprintf(fpartition, "\n" );
	// 	for(k=0;k<noOfDim;k++)
	// 	{
	// 		fscanf(fp,"%lf",&tempRead);
	// 		fprintf(fpartition, "%lf ",tempRead );
	// 	}
	// }
	// fclose(fpartition);
	fclose(fp);
}