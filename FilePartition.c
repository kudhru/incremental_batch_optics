#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
int main( int argc, char** argv)
{
	char partitionedFilename[1000];
	double tempRead;
	FILE *fp= fopen(argv[1], "r"),*fpartition;
	assert(fp!=NULL);
	int noOfPoints,noOfDim,i,noOfPartitions=atoi(argv[2]),sizeOfPartition,j,k;
	fscanf(fp,"%d\n%d\n",&noOfPoints,&noOfDim);
	sizeOfPartition = noOfPoints / noOfPartitions ;
	for(i=0;i < noOfPartitions-1 ;i++)
	{
		sprintf(partitionedFilename,"%s_partition_%d_%d",argv[1],noOfPartitions,i+1);
		fpartition=fopen(partitionedFilename,"w");
		assert(fpartition!=NULL);
		fprintf(fpartition, "%d\n%d",sizeOfPartition,noOfDim );
		for(j = 0;j < sizeOfPartition;j++ )
		{
			fprintf(fpartition, "\n" );
			for(k=0;k<noOfDim;k++)
			{
				fscanf(fp,"%lf",&tempRead);
				fprintf(fpartition, "%lf ",tempRead );
			}
		}
		fclose(fpartition);
	}
	sprintf(partitionedFilename,"%s_partition_%d_%d",argv[1],noOfPartitions,i+1);
	fpartition=fopen(partitionedFilename,"w");
	assert(fpartition!=NULL);
	fprintf(fpartition, "%d\n%d",noOfPoints-(noOfPartitions-1) * sizeOfPartition,noOfDim );
	for(j = (noOfPartitions-1) * sizeOfPartition ;j < noOfPoints;j++ )
	{
		fprintf(fpartition, "\n" );
		for(k=0;k<noOfDim;k++)
		{
			fscanf(fp,"%lf",&tempRead);
			fprintf(fpartition, "%lf ",tempRead );
		}
	}
	fclose(fpartition);
	fclose(fp);
}