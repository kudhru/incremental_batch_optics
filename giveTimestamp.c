#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include "Def.h"
#include "Optics.h"
#include "file_io.h"
int main(int argc, char * argv[])
{
	char * inputFileName= argv[1];
	char * outputFileName= argv[2];
	int noOfPartitions= atoi(argv[3]);
	DataHdr dataList= readData(inputFileName);
	PointHdr orderList= initPointHdr();
	buildClusterOrdering(outputFileName,orderList,dataList);
	int i,offset,j,sizeOfOnePartition= dataList->uiCnt / noOfPartitions;
	for(i=0;i < noOfPartitions;i++)
	{
		offset= i*sizeOfOnePartition;
		for(j=0;j < sizeOfOnePartition ; j++)
			dataList->dataClstElem[offset + j].globalTimestamp = i+1;
	}
	PrintOrderedListInToFileForAddtion(orderList,dataList,NULL,outputFileName,0 );
	// getchar();
	// getchar();
}