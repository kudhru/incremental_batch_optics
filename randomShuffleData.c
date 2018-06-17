#include "Optics.h"
#include "Data.h"
#include "List.h"
#include "RTree.h"
int main(int argc, char **argv)
{
	DataHdr dataList=readData(argv[1]);
	char shuffledFile[100];
	sprintf(shuffledFile,"%s_shuffledData_%s_sampled.txt",argv[1],argv[2]);
	FILE *fp=fopen(shuffledFile,"w");
	
	int i=0,count=0,index,k,sampleAmount=floor(atof(argv[2])*dataList->uiCnt);
	fprintf(fp, "%d\n%d",sampleAmount,DIMENSION );
	while(i< 50*sampleAmount && count <sampleAmount)
	{	
		index = random() % dataList->uiCnt;
		if(dataList->dataClstElem[index].fixed==0)
		{
			dataList->dataClstElem[index].fixed=1;
			fprintf(fp, "\n" );
			for(k=0;k<DIMENSION-1;k++)
				fprintf(fp, "%lf ",dataList->dataClstElem[index].iData[k] );
			fprintf(fp, "%lf",dataList->dataClstElem[index].iData[k] );
			count++;
		}
		i++;
	}
	printf("count=%d i=%d\n",count,i );
	for(index=0;index<dataList->uiCnt && count <sampleAmount;index++)
	{
		if(dataList->dataClstElem[index].fixed==0)
		{
			dataList->dataClstElem[index].fixed=1;
			fprintf(fp, "\n" );
			for(k=0;k<DIMENSION-1;k++)
				fprintf(fp, "%lf ",dataList->dataClstElem[index].iData[k] );
			fprintf(fp, "%lf",dataList->dataClstElem[index].iData[k] );
			count++;
		}
	}
	fclose(fp);
}