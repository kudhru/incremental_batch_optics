#ifndef __OPTICS_H
#define __OPTICS_H
#include "Def.h"
#include "Data.h"
#include "RTree.h"
#include "file_io.h"
void Optics(DataHdr dataList, HdrNd hdrNdTree, PointHdr OrderList);
void expandCluster(DataHdr dataList, HdrNd hdrNdTree, Data dataTemp, PointHdr OrderList,PQueue *pq);
void Update(DataHdr dataList, Data dataTemp,PQueue *pq);
double find_core_dist(Data dataTemp);
void assign_nbData(NB temp1, NB temp2);
void decrease(Data searchNd,PQueue *pq);
void extractClusters(Cluster_order ord);
Cluster_order getClusterOrder(char *strFileName);
void viewNoise(Noise noiseHd);
void viewClusters(Cluster clust);
void freeSuccessorList(Data data);
void allocateDataSizeToProcesses(int * dataSize,int numprocs);
Stack buildStack(int *stackArray,int stackSize);
int buildClusterOrdering( char * strFileName,PointHdr orderList, DataHdr initialDataList);
void PrintOrderedListInToFileForAddtion(PointHdr UpdatedList,DataHdr dataList1,DataHdr dataList2, char * strFileName,int noOfObjects );
//double**distributeData(int *startIndex,int*endIndex,int numprocs,double **objects,int *numObjs,int DIMENSION,MPI_Comm comm,int myrank);
void AddToSuccessorList(DataHdr dataList,Data dataTemp);
void GetAffectedList(HdrNd RTree1, DataHdr dataList1, DataHdr dataList2);
void reorganizingClusterOrdering(DataHdr dataList1,DataHdr dataList2,HdrNd dataTree1,HdrNd dataTree2,PointHdr OrderList1,PointHdr OrderList2,PointHdr ReorganizedOrderList);
void ProcessOtherFixedPointsOfReorganizedList(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PointHdr OrderList1,PointHdr OrderList2, PQueue* pq);
void processSuccessors(Data dataTemp,DataHdr dataList1,PQueue * pq);
void processAffectedPoint(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PQueue* pq);
void UpdateForReorganizingCO(DataHdr dataList1, DataHdr dataList2, Data dataTemp1, Data dataTemp2, PQueue *pq);
void decrease1(Data searchNd,PQueue *pq);
void AddToOrderList(PointHdr UpdatedList, int iNum,int timestamp);
void PrintOrderedListInToFile(PointHdr UpdatedList,DataHdr dataList1,DataHdr dataList2, char * strFileName,int noOfObjects );
int nextIterationRequired(Stack procStack,int myrank);
void OpticsMultiNodeApproach(int *startIndex,int *endIndex,int numprocs,double **objects,int numObjs,int DIMENSION,int myrank,char * outputFileName,Stack procStack,char *inputFile,int noofThreads);
void findIntersectionPoints(HdrNd hdrOldRtree, HdrNd hdrNewRtree, DataHdr dataList1, DataHdr dataList2, LstNd nodeOld, LstNd nodeNew,  Region intRegion);
void processPenultimate(HdrNd hdrOldRtree, HdrNd hdrNewRtree, LstNd nodeOld, LstNd nodeNew, DataHdr dataList1, DataHdr dataList2, Region intRegion) ;

#endif