#include "dataDistribution.h"

int partitions(DataPoint *objects, int low, int high, int dim, int numCoords, dataPoint *temp)
{
    int p = low, r = high, i = low-1, j, k;
    dataPoint x = objects[r-1][dim], t;
    for(j = p; j < r-1; j++)
    {
        if (objects[j][dim] <= x)
        {
            i=i+1;
            //swapObjects(objects[i], objects[j], numCoords, temp);
            for(k = 0; k < numCoords; k++)
			{
				t = objects[j][k];
				objects[j][k] = objects[i][k];
				objects[i][k] = t;
			}
        }
    }
    //swapObjects(objects[i+1], objects[r-1], numCoords, temp);
    for(k = 0; k < numCoords; k++)
	{
		t = objects[r-1][k];
		objects[r-1][k] = objects[i+1][k];
		objects[i+1][k] = t;
	}
    return i+1;
}

int quickSelect(DataPoint *objects, int left, int right, int kth, int dim, int numCoords)
{
    #ifdef PRINT
   // printf("l=%d r=%d k=%d dim=%d\n ",left,right,kth,dim);
    #endif
    int pivotIndex, len;
    dataPoint *temp = (dataPoint*) calloc(numCoords, sizeof(dataPoint));
    assert(temp != NULL);
    
    while(1)                        //run till pivot element becomes 'kth' element in [left, right]
    {
        pivotIndex = partitions(objects, left, right, dim, numCoords, temp);          //Select the Pivot Between Left and Right
	//	printf("pivotIndex=%d\n",pivotIndex);       
 len = pivotIndex - left;// + 1;

        if(kth == len)
            return pivotIndex;

        else if(kth < len)
            right = pivotIndex;// - 1;

        else
        {
            kth = kth - len;
            left = pivotIndex;// + 1;
        }
    }
    free(temp);
}

int findMaxRange(DataPoint *objects, int numObjs, int numCoords)
{
    int i, j, maxR = 0;

    dataPoint *maxD = (dataPoint*) calloc(numCoords, sizeof(dataPoint));
    dataPoint *minD = (dataPoint*) calloc(numCoords, sizeof(dataPoint));
    assert(maxD != NULL && minD != NULL);

    for(j=0; j < numCoords; j++)
    {
        minD[j] = maxD[j] = objects[0][j];
    }

    //calculate min & max of each dimension
    for(i=1; i < numObjs; i++)
    {
        for(j=0; j < numCoords; j++)
        {
            if(objects[i][j] < minD[j])
                minD[j] = objects[i][j];

            else if(objects[i][j] > maxD[j])
                maxD[j] = objects[i][j];
        }
    }

    //calculate range of each dimension and find max range
    for(j=0; j < numCoords; j++)
    {
        minD[j] = maxD[j] - minD[j];
        if(minD[j] > minD[maxR])
            maxR = j;
    }

    free(maxD);
    free(minD);

    return maxR;
}

//Distributes the data pointed by 'objects' into 'nproc' number of processes using KD Tree (median based)
int* buildKDtree(DataPoint *objects, int numObjs, int numCoords, int nproc)
{
    int i, j, maxR = 0, firstSplit, secondSplit;
    int *dSize = (int*) calloc(nproc, sizeof(int));
    assert(dSize != NULL);

    maxR = findMaxRange(objects, numObjs, numCoords);
    firstSplit = quickSelect(objects, 0, numObjs, numObjs/2, maxR, numCoords);              //3rd argument specifies the size of spilt (N/2 in case of median)

    //printf("\nFirst split is at %d in dimension %d", firstSplit, maxR);
    maxR = findMaxRange(objects, firstSplit, numCoords);                                    //processing 1st half of firstsplit
    secondSplit = quickSelect(objects, 0, firstSplit, firstSplit/2, maxR, numCoords);

    //printf("\nSecond split is at %d in dimension %d", secondSplit, maxR);
    dSize[0] = secondSplit;
    dSize[1] = firstSplit - dSize[0];

    maxR = findMaxRange(objects+firstSplit, numObjs - firstSplit, numCoords);               //processing 2nd half of firstsplit
    secondSplit = quickSelect(objects, firstSplit , numObjs, firstSplit/2, maxR, numCoords);

    //rintf("\nThird split is at %d in dimension %d", secondSplit, maxR);
    dSize[2] = secondSplit - (dSize[0] + dSize[1]);
    dSize[3] = numObjs - (dSize[0] + dSize[1] + dSize[2]);

	/*for(i=0; i < nproc; i++)
		printf("\n%d", dSize[i]);*/
    return dSize;                                                                             //send back the number of elements in each of the partition
}
void buildKDtreeGeneric(DataPoint *objects,int numCoords, int nproc,int startIndex,int endIndex,int procId,int * StartArray,int * EndArray,Stack procStack)
{
    if(nproc==1)
        return;
    int i, j, maxR = 0, firstSplit, secondSplit;
    #ifdef PRINT
    printf("nproc=%d procId=%d\n",nproc,procId);
    #endif	
    maxR = findMaxRange(objects, endIndex-startIndex+1, numCoords);    
    #ifdef PRINT
    printf("maxR=%d\n",maxR);
    #endif
    firstSplit = quickSelect(objects,startIndex, endIndex+1, (endIndex-startIndex+1)/2, maxR, numCoords);
    #ifdef PRINT
     printf("firstSplit=%d\n",firstSplit); 
    #endif
    StartArray[procId+nproc/2]=firstSplit;
    EndArray[procId+nproc/2]=endIndex;
    pushStack(procStack, procId,procId+nproc/2);
    //dSize[procId+nproc/2]=endIndex-firstSplit+1;
    buildKDtreeGeneric(objects,numCoords,nproc/2,startIndex,firstSplit-1, procId,StartArray,EndArray,procStack);
    if(nproc%2==0)
        buildKDtreeGeneric(objects,numCoords,nproc/2,firstSplit,endIndex, procId+nproc/2,StartArray,EndArray,procStack);
    else
        buildKDtreeGeneric(objects,numCoords,nproc/2 +1,firstSplit,endIndex, procId+nproc/2,StartArray,EndArray,procStack);     
    //int *dSize = (int*) calloc(nproc, sizeof(int));
    //assert(dSize != NULL);
    // maxR = findMaxRange(objects, numObjs, numCoords);
    // firstSplit = quickSelect(objects, 0, numObjs, numObjs/2, maxR, numCoords);              //3rd argument specifies the size of spilt (N/2 in case of median)

    // //printf("\nFirst split is at %d in dimension %d", firstSplit, maxR);
    // maxR = findMaxRange(objects, firstSplit, numCoords);                                    //processing 1st half of firstsplit
    // secondSplit = quickSelect(objects, 0, firstSplit, firstSplit/2, maxR, numCoords);

    // //printf("\nSecond split is at %d in dimension %d", secondSplit, maxR);
    // dSize[0] = secondSplit;
    // dSize[1] = firstSplit - dSize[0];

    // maxR = findMaxRange(objects+firstSplit, numObjs - firstSplit, numCoords);               //processing 2nd half of firstsplit
    // secondSplit = quickSelect(objects, firstSplit , numObjs, firstSplit/2, maxR, numCoords);

    // //rintf("\nThird split is at %d in dimension %d", secondSplit, maxR);
    // dSize[2] = secondSplit - (dSize[0] + dSize[1]);
    // dSize[3] = numObjs - (dSize[0] + dSize[1] + dSize[2]);


    /*for(i=0; i < nproc; i++)
        printf("\n%d", dSize[i]);*/
    //return dSize;                                                                             //send back the number of elements in each of the partition
}
