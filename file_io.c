/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*   File:         file_io.c                                                 */
/*   Description:  This program reads point data from a file                 */
/*                 and write cluster output to files                         */
/*   Input file format:                                                      */
/*            First number represents Number of Objects (numObjs)            */
/*            Second number represents Number of Dimensions (numCoords)	     */
/*            After that each numCoords numbers represent a data point       */
/*                 coordinates) of each object                               */
/*   Author:  Wei-keng Liao                                                  */
/*            ECE Department Northwestern University                         */
/*            email: wkliao@ece.northwestern.edu                             */
/*   Copyright, 2005, Wei-keng Liao                                          */
/*   See COPYRIGHT notice in top-level directory.                            */
/*                                                     			     */
/*   Modified by: Saiyedul Islam					     */
/*		  CSIS Department BITS Pilani				     */
/*		  saiyedul.islam@gmail.com		                     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "file_io.h"

#define MAX_CHAR_PER_LINE 128


/*---< file_read() >---------------------------------------------------------*/
void  file_read(  char *filename,      /* input file name */
                  int  *numObjs,       /* no. data objects (local) */
                  int  *numCoords,double*** objects)     /* no. coordinates */
{
			
		FILE *infile;

		if ((infile = fopen(filename, "r")) == NULL)
		{
			fprintf(stderr, "Error: no such file (%s)\n", filename);
			return ;
		}
		
		fscanf(infile, "%d", numObjs);
		fscanf(infile, "%d", numCoords);

		/* allocate space for objects[][] and read all objects */
		int i,j,len = (*numObjs) * (*numCoords);
		*objects    = (double**)malloc((*numObjs) * sizeof(double*));
		assert(*objects != NULL);
		(*objects)[0] = (double*) malloc(len * sizeof(double));
		assert(objects[0] != NULL);
		for (i=1; i<(*numObjs); i++)
		    (*objects)[i] = (*objects)[i-1] + (*numCoords);

		i = 0;
		while(feof(infile)!= 1)
		{
      for (j=0; j<(*numCoords); j++)
      {
        fscanf(infile, "%lf", &((*objects)[i][j]));
			}
			i++;
		}
		fclose(infile);      
}
DataHdr readData(char *strFileName)
{   // read data from the file
	int iCnt = 0;
	int iDim = 0;
	int noOfPoints;

	if(strFileName == NULL)
		return NULL;

	FILE *filP = fopen(strFileName, "r");

	if(filP == NULL)
		return NULL;
    fscanf(filP,"%d\n%d\n",&noOfPoints,&DIMENSION);
    //DataPoint *ptrData = (DataPoint *) malloc( iNumber * sizeof(DataPoint) );
	DataPoint dataPointTemp = NULL;
	DataHdr dataList = initDataHdr(noOfPoints);

	if(dataList == NULL)
		return NULL;
	while(!feof(filP))
    {   
    	dataPointTemp = (DataPoint)malloc(DIMENSION *sizeof(dataPoint));

		for(iDim = 0; iDim < DIMENSION - 1; iDim++)
			fscanf(filP, "%lf ", &dataPointTemp[iDim]);
        fscanf(filP, "%lf\n", &dataPointTemp[iDim]);
        insertDataLstElem(dataList, dataPointTemp);
	}

	return dataList;
}
DataHdr readDataModified(char *strFileName,int dataList2_Size)
{   
	// read data from the file
	int iCnt = 0;
	int iDim = 0;
	int noOfPoints;

	if(strFileName == NULL)
		return NULL;

	FILE *filP = fopen(strFileName, "r");

	if(filP == NULL)
		return NULL;
    fscanf(filP,"%d\n%d\n",&noOfPoints,&DIMENSION);
    //printf("DIMENSION=%d\n",DIMENSION );
    //DataPoint *ptrData = (DataPoint *) malloc( iNumber * sizeof(DataPoint) );
	DataPoint dataPointTemp = NULL;
	DataHdr dataList = initDataHdr(noOfPoints+dataList2_Size);

	if(dataList == NULL)
		return NULL;
	while(!feof(filP))
    {   
    	dataPointTemp = (DataPoint)malloc(DIMENSION *sizeof(dataPoint));

		for(iDim = 0; iDim < DIMENSION - 1; iDim++)
			fscanf(filP, "%lf ", &dataPointTemp[iDim]);
        fscanf(filP, "%lf\n", &dataPointTemp[iDim]);
        insertDataLstElem(dataList, dataPointTemp);
	}
	return dataList;
}
