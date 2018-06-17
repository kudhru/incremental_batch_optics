#ifndef __DATA_H
#define __DATA_H
#include "Def.h"
DataHdr initDataHdr(int size);
SuccessorListHeader initSuccessorList();
Data initData(DataPoint iData);
PointHdr initPointHdr() ;//Sonal made
void freePointHdr(PointHdr orderList);
Boolean isDataLstEmpty(DataHdr dataHdrInfo);
void insertDataLstElem(DataHdr dataHdrInfo, DataPoint iData);
Data insertDataLstNd(DataHdr dataHdrInfo, Data dataClstElem);
void printDataLst(DataHdr dataHdrInfo);
NbHdr initNbHdr();
Boolean isNbLstEmpty(NbHdr nbHdrInfo);
void insertNeighbors(Data dataTemp, Data dataClstElem, double dist);
void insertNeighbors_1(Data dataTemp, Data dataClstElem, double dist);
void printNbhLst(NbHdr nbHdrInfo,DataHdr);
int compare_pq(Data data1,Data data2);
PQueue *pqueue_new(int (*cmp)(Data, Data), size_t capacity) ;
void pqueue_delete(PQueue *q) ;
void pqueue_enqueue(PQueue *q, const void *data) ;
void *pqueue_dequeue(PQueue *q);
void *pqueue_top(PQueue *q);
void pqueue_heapify(PQueue *q, size_t idx);
Stack newStack();
Boolean pushStack (Stack st, int pid1,int pid2);
Boolean popStack (Stack st, int* pid1,int* pid2);
Boolean topOfStack (Stack st, int* pid1,int* pid2);
int sizeOfStack(Stack st);
void displayStack (Stack st);
#endif