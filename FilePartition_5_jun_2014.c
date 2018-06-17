#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include "Def.h"
#include "file_io.h"
int main( int argc, char** argv)
{
	FILE * fpartition;
	char partitionedFilename[1000];
	double tempRead;
	int i,j,k,offset;
	char * fileName= argv[1];
	int noOfPartitions= atoi(argv[2]);
	int windowSize= atoi(argv[3]);
	int firstPartitionLength=atoi(argv[4]);

	DataHdr dataList= readData(fileName);
	int firstPartitionSize = (dataList->uiCnt / noOfPartitions) * firstPartitionLength ;
	int sizeOfSmallerPartitions=(dataList->uiCnt - firstPartitionSize)/(noOfPartitions-firstPartitionLength);
	int finalPartitionSize = dataList->uiCnt - firstPartitionSize - sizeOfSmallerPartitions*(noOfPartitions-firstPartitionLength-1);
	sprintf(partitionedFilename,"%s_partition_%d_%d",fileName,noOfPartitions,1);
	fpartition=fopen(partitionedFilename,"w");
	assert(fpartition!=NULL);
	fprintf(fpartition, "%d\n%d",firstPartitionSize,DIMENSION );
	for(i=0;i<firstPartitionSize;i++)
	{
		fprintf(fpartition, "\n" );
		for(k=0;k<DIMENSION;k++)
			fprintf(fpartition, "%lf ",dataList->dataClstElem[i].iData[k] );
	}
	fclose(fpartition);
	for(i=firstPartitionLength+1;i<noOfPartitions;i++)
	{
		sprintf(partitionedFilename,"%s_partition_%d_%d",fileName,noOfPartitions,i);
		fpartition=fopen(partitionedFilename,"w");
		assert(fpartition!=NULL);
		fprintf(fpartition, "%d\n%d",sizeOfSmallerPartitions,DIMENSION );
		offset= firstPartitionSize + (i- firstPartitionLength-1 )*sizeOfSmallerPartitions;
		for(j=0;j<sizeOfSmallerPartitions;j++)
		{
			fprintf(fpartition, "\n" );
			for(k=0;k<DIMENSION;k++)
				fprintf(fpartition, "%lf ",dataList->dataClstElem[offset+j].iData[k] );
		}
		fclose(fpartition);
	}
	sprintf(partitionedFilename,"%s_partition_%d_%d",fileName,noOfPartitions,i);
	fpartition=fopen(partitionedFilename,"w");
	assert(fpartition!=NULL);
	fprintf(fpartition, "%d\n%d",finalPartitionSize,DIMENSION );
	offset= dataList->uiCnt - finalPartitionSize;
	for(j=0;j<finalPartitionSize;j++)
	{
		fprintf(fpartition, "\n" );
		for(k=0;k<DIMENSION;k++)
			fprintf(fpartition, "%lf ",dataList->dataClstElem[offset+j].iData[k] );
	}
	fclose(fpartition);
}