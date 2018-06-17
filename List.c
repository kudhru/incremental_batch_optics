#include "List.h"

HdrNd initHdrNd()
{   HdrNd HdrNdLst = (HdrNd)malloc(sizeof(struct hdrNd));
    if(HdrNdLst == NULL)
		return NULL;	

	HdrNdLst->uiCnt = 0;
	HdrNdLst->ptrFirstNd = NULL;
	HdrNdLst->ptrParentNd = NULL;
	
	return HdrNdLst;
}

LstNd initLstNd(TreeNode tnInfo)
{   LstNd LstNdElem = (LstNd)malloc(sizeof(struct lstNd));
	
	if(LstNdElem == NULL)
		return NULL;	

	LstNdElem->tnInfo = tnInfo;
	LstNdElem->ptrChildLst = NULL;
	LstNdElem->ptrNextNd = NULL;
	
	return LstNdElem;	
}

Boolean isLstEmpty(HdrNd HdrNdLst)
{   return(HdrNdLst->ptrFirstNd == NULL || HdrNdLst->uiCnt == 0) ? TRUE : FALSE;
}

void insertLstElem(HdrNd HdrNdLst, TreeNode tnInfo)
{	insertLstNd(HdrNdLst, initLstNd(tnInfo));
	return;
}

void insertLstNd(HdrNd HdrNdLst, LstNd LstNdElem)
{   
	if(LstNdElem == NULL || HdrNdLst == NULL)
		return;
    LstNdElem->ptrNextNd = HdrNdLst->ptrFirstNd;
	HdrNdLst->ptrFirstNd = LstNdElem;
	HdrNdLst->uiCnt++;
	
	return;
}

LstNd deleteLstElem(HdrNd HdrNdLst, TreeNode tnInfo)
{	if(isLstEmpty(HdrNdLst))
		return NULL;
	TreeNode tnInfoTemp = NULL;
	Boolean bIsFound = FALSE;
	LstNd LstNdTemp = HdrNdLst->ptrFirstNd;
	if(LstNdTemp->tnInfo == tnInfo)
    {	return deleteLstFirst(HdrNdLst);
        /*
		HdrNdLst->ptrFirstNd = LstNdTemp->ptrNextNd;
		LstNdTemp->ptrNextNd = NULL;
		HdrNdLst->uiCnt--;
		tnInfoTemp = LstNdTemp->tnInfo;
		LstNdTemp->tnInfo = NULL;
		free( LstNdTemp );
		return tnInfoTemp;
        */
	}

	while(LstNdTemp->ptrNextNd != NULL)
    {   if(LstNdTemp->ptrNextNd->tnInfo == tnInfo)
        {   bIsFound = TRUE;
			// LstNdTemp = deleteNextNd(HdrNdLst, LstNdTemp);
			// break;			
			return deleteNextNd(HdrNdLst, LstNdTemp);
		}		
		LstNdTemp = LstNdTemp->ptrNextNd;
	}
	
    //tnInfoTemp = LstNdTemp->tnInfo;
    //LstNdTemp->tnInfo = NULL;
    //free( LstNdTemp );
    //return (bIsFound)?LstNdTemp:NULL;
	return NULL;
}

LstNd deleteLstFirst(HdrNd HdrNdLst)
{   if(isLstEmpty(HdrNdLst))
		return NULL;
	TreeNode tnInfoTemp = NULL;
	LstNd lstNdTemp = HdrNdLst->ptrFirstNd;

	HdrNdLst->ptrFirstNd = lstNdTemp->ptrNextNd;
	HdrNdLst->uiCnt--;
	lstNdTemp->ptrNextNd = NULL;
//	tnInfoTemp = lstNdTemp->tnInfo;
//	lstNdTemp->tnInfo = NULL;
//	free( lstNdTemp );
		
	return lstNdTemp;
}

LstNd deleteNextNd(HdrNd HdrNdLst, LstNd LstNdElem)
{   if(isLstEmpty(HdrNdLst) || LstNdElem == NULL || LstNdElem->ptrNextNd == NULL)	
		return;

	LstNd LstNdTemp = LstNdElem->ptrNextNd;
    LstNdElem->ptrNextNd = LstNdTemp->ptrNextNd;
	LstNdTemp->ptrNextNd = NULL;	
	HdrNdLst->uiCnt--;

	return LstNdTemp;
}

void printLst(HdrNd HdrNdLst)
{   if(isLstEmpty(HdrNdLst))
		return;

	int iCnt = 0;
	LstNd LstNdTemp = NULL;
    switch(HdrNdLst->ptrFirstNd->tnInfo->ndType)
    {   case INTNODE:	printf(" INTNODE: %d\n", HdrNdLst->uiCnt);
                        LstNdTemp = HdrNdLst->ptrFirstNd;
		                while(LstNdTemp != NULL)
                        {    printf( "BottomLeft: " );
		                     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				             printf("%lf ", LstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
                           	 printf("TopRight: ");
			                 for(iCnt = 0; iCnt < DIMENSION; iCnt++)
                                  printf("%lf ", LstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
                             LstNdTemp = LstNdTemp->ptrNextNd;
			                 printf("\n");
	                   	}
		                printf("\n");	
		                break;
	    case EXTNODE:   printf("EXTNODE: %d\n", HdrNdLst->uiCnt);
                        LstNdTemp = HdrNdLst->ptrFirstNd;
		                while(LstNdTemp != NULL)
                        {	printf("DataPoint: ");
		                    for(iCnt = 0; iCnt < DIMENSION; iCnt++)
	                             printf("%lf ", LstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
	                        LstNdTemp = LstNdTemp->ptrNextNd;
			                printf("\n");
	                    }
		                printf("\n");	
		                break;
	}
	return;
}
