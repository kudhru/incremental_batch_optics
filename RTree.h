#ifndef __RTREE_H
#define __RTREE_H
#include "Def.h"
#include "Data.h"
#include "List.h"

Region initRgnRect(Dimension iBottomLeft, Dimension iTopRight);
void setRect(LstNd lstNd, TreeNode tnInfo);

TreeNode initExtNd(Data dataClstElem);
TreeNode initIntNd(Dimension iBottomLeft, Dimension iTopRight);

Boolean expansionArea(Region rgnRect, TreeNode tnInfo, Double ptrDMinExp, Region rgnNewRect);
double area(Region rgnRect);

LstNd pickChild(HdrNd ptrChildLst, TreeNode tnInfo);
void pickSeeds(HdrNd ptrChildList, LstNd *lstNdChildOne, LstNd *lstNdChildTwo);
void splitNode(LstNd ptrChild);

Boolean insertTree(HdrNd hdrNdTree, TreeNode tnInfo);
HdrNd buildRTree(DataHdr dataHdrLst);
HdrNd createRoot(HdrNd hdrNdTree);
void printTree(HdrNd hdrNdTree);

unsigned int getNeighborHood(HdrNd hdrNdTree, Data dataNdTemp);
unsigned int getNeighborHood2(HdrNd hdrNdTree, Data dataNdTemp);
unsigned int getNeighborHoodUsingBruteForce(DataHdr dataList, Data dataTemp);
double findDist(DataPoint iDataOne, DataPoint iDataTwo);
unsigned int findRecords(HdrNd hdrNdTree, Region rgnRect, Data dataNdTemp);
unsigned int findRecordsUsingBruteForce(DataHdr dataList, Region rgnRect, Data dataTemp);
Boolean isContains(Region rgnRect, DataPoint iData);
Boolean isOverLap(Region rgnRectOne, Region rgnRectTwo);

void appendRTree(HdrNd hdrNdTree, DataHdr dataHdrLst);
Region findOverLap(Region rgnRectOne, Region rgnRectTwo);
void reDistributeEnt(LstNd parent,HdrNd childHdr,LstNd removed);
Boolean findLeaf(HdrNd curLstHd,LstNd prev,LstNd cur,Data record);
Boolean matchRecord(DataPoint rec1,DataPoint rec2);
void adjustRect(LstNd l);
void adjustLeafRect(LstNd l);
void printTree(HdrNd hdrNdTree);
#endif
