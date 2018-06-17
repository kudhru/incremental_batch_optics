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
	int windowSize=atoi(argv[3]);
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

	int sum=0,*temp=(int*)malloc(sizeof(int)*noOfPartitions),m;
	FILE **fTemp=(FILE**)malloc(sizeof(FILE *)*noOfPartitions);
	FILE *foptics;
	for(i=1;i<=noOfPartitions;i++)
	{
		sum=0;
		sprintf(partitionedFilename,"%s_partitions_%d_window_1_%d",argv[1],noOfPartitions,i);
		fpartition=fopen(partitionedFilename,"w");
		for(j=1;j<=i;j++)
		{
			sprintf(partitionedFilename,"%s_partition_%d_%d",argv[1],noOfPartitions,j);
			fTemp[j-1]=fopen(partitionedFilename,"r");
			fscanf(fTemp[j-1],"%d\n%d\n",temp+j-1,&noOfDim);
			sum+=temp[j-1];
		}
		fprintf(fpartition, "%d\n%d",sum,noOfDim );
		for(j=1;j<=i;j++)
		{
			for(k=0;k<temp[j-1];k++)
			{
				fprintf(fpartition, "\n" );
				for(m=0;m<noOfDim;m++)
				{
					fscanf(fTemp[j-1],"%lf",&tempRead);
					fprintf(fpartition, "%lf ",tempRead );
				}
			}
		}
		fclose(fpartition);
		for(j=1;j<=i;j++)
			fclose(fTemp[j-1]);
	}


}