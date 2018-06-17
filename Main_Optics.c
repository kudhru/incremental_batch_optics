#include "Optics.h"
#include "Data.h"
#include "List.h"
#include "RTree.h"




int main(int argc, char **argv)
{
    int myrank,i, j,k,chunkSize,numprocs,numObjs,*startIndex,*endIndex,obj_len,noofThreads;
    double** objects;      /* [numObjs][numCoords] data objects */
    char *filename1;
    MINENTRIES = atoi(argv[1]);  //Minimum entries for node in RTree 8
    MAXENTRIES = atoi(argv[2]);        //Maximum entries for node in RTree 16
    EPS=atof(argv[3]);
    EPS1=atof(argv[4]);
    MINPOINTS=atoi(argv[5]);
    UNDEFINED=atoi(argv[6]);
    originalDataFile= argv[16];
    noOfPartitions= atoi(argv[17]);
    windowSize= atoi(argv[18]);
    // filename1=argv[7];//input file name
    // int noOfPartitions=atoi(argv[8]),windowSize=atoi(argv[9]);
    //incrementalOptics(filename1,noOfPartitions,windowSize);
    //printf("end 4 %d %d\n",atoi(argv[7]),atoi(argv[8]));
    // for addition, we need
    //argv[8]- current timestamp file
    // argv[9] - initial data list file
    // argv[10] -  initial order list file
    // argv[11] -  new data list file
    // argv[12] -  final order list file

    // for deletion, we need
    //argv[8]- current timestamp file
    //argv[9] - initial data list file
    //argv[10] -  initial order list file
    //argv[11]- time stamp to be deleted (integer)
    //argv[12] -  final order list file
     // printf("argv[7]=%s\n argv[8]=%s\n argv[9]=%s\nargv[10]=%s \n argv[11]=%s\n argv[12]=%s\n argv[13]=%s\n argv[14]=%s\n",argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14] );
    // printf("argv[15]=%s\n",argv[15] );

    if(atoi(argv[13])==0)//pointwiseinsertion
    {
        if(atoi(argv[7])==0)
            pointWiseInsertion(atoi(argv[8]),argv[9],argv[10],argv[11],argv[12],argv[15]);
         else
            pointWiseDeletion(argv[9],argv[10],atoi(argv[11]),argv[12],argv[15]);
    }
    else if(atoi(argv[13])==1)// batchwise insertion
    {
        if(atoi(argv[7])==0)
            addPointsToOrderList(atoi(argv[8]),argv[9],argv[10],argv[11],argv[12],argv[15]);
        else
            insertionAndDeletionCombined(argv[9],argv[10],argv[14],argv[12] ,atoi(argv[11]), atoi(argv[8]),argv[15]);
    }
    else if(atoi(argv[13])==2)// batchwise insertion
    {
        if(atoi(argv[7])==0)
            runClassicalOpticsForInsertion(atoi(argv[8]),argv[9],argv[10],argv[11],argv[12],argv[15]);
        else
            runClassicalOpticsForDeletion(argv[9],argv[10],argv[14],argv[12] ,atoi(argv[11]), atoi(argv[8]),argv[15]);
    }
        
    //IncrementalOptics();
    //OpticsMultiNodeApproach(startIndex,endIndex,numprocs,objects,numObjs,DIMENSION,myrank,argv[2],procStack,filename1,noofThreads);
    //MPI_Finalize();
    //getchar();
    return 0;
}
