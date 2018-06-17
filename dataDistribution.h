#ifndef __DATA__DIST_H
#define __DATA_DIST_H
#include "Def.h"
#include "Data.h"
int partitions(DataPoint *objects, int low, int high, int dim, int numCoords, dataPoint *temp);
int quickSelect(DataPoint *objects, int left, int right, int kth, int dim, int numCoords);
int findMaxRange(DataPoint *objects, int numObjs, int numCoords);
int* buildKDtree(DataPoint *objects, int numObjs, int numCoords, int nproc);
void buildKDtreeGeneric(DataPoint *objects,int numCoords, int nproc,int startIndex,int endIndex,int procId,int * StartArray,int * EndArray,Stack procStack);
#endif