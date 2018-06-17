#ifndef __LIST_H
#define __List_H
#include "Def.h"
HdrNd initHdrNd();
LstNd initLstNd(TreeNode tnInfo);
Boolean isLstEmpty(HdrNd HdrNdLst);
void printLst(HdrNd HdrNdLst);
void insertLstElem(HdrNd HdrNdLst, TreeNode tnInfo);
void insertLstNd(HdrNd HdrNdLst, LstNd LstNdElem);
LstNd deleteLstElem(HdrNd HdrNdLst, TreeNode tnInfo);
LstNd deleteLstFirst(HdrNd HdrNdLst);
LstNd deleteNextNd(HdrNd HdNdLst, LstNd LstNdElem);

#endif
