#include "RTree.h"

Region initRgnRect(Dimension iBottomLeft, Dimension iTopRight)
{   //initializes the rectangle with the given bottom left and top right corners
    //if the values for the corners are specified NULL, initializes a rectangle with origin as co-ordinates for both corners.
	Region rgnRect = (Region)malloc(sizeof(*rgnRect));
    if(rgnRect == NULL)
		return NULL;
		
    if(iBottomLeft != NULL)
		rgnRect->iBottomLeft = iBottomLeft;
	else
    //rgnRect->iBottomLeft = (Dimension) calloc( DIMENSION, sizeof( dimension ) );
		rgnRect->iBottomLeft = (Dimension)malloc(DIMENSION*sizeof(double));

	if(rgnRect->iBottomLeft == NULL)
    {   free(rgnRect);
		return NULL;
    }

	if(iTopRight != NULL)
		rgnRect->iTopRight = iTopRight;
	else
		rgnRect->iTopRight = (Dimension)malloc(DIMENSION*sizeof(double));

	if(rgnRect->iTopRight == NULL)
    {   
    	if(rgnRect->iBottomLeft == NULL);
		free(rgnRect);
		return NULL;
	}

	return rgnRect;
}

void setRect(LstNd lstNd, TreeNode tnInfo)
{   // copies the data in the tree node tnInfo to lstNd
    int iCnt = 0;
    switch(tnInfo->ndType)
    {   case INTNODE:
		//incase of internal node copy the bottom left and top right corners
		for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   lstNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
			lstNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
     	}
		break;
        case EXTNODE:
		// in case of external node copy the data element
		for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   lstNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = tnInfo->tdInfo->dataClstElem->iData[iCnt];
			lstNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = tnInfo->tdInfo->dataClstElem->iData[iCnt];
		}
		break;
	}

	return;
}

TreeNode initIntNd(Dimension iBottomLeft, Dimension iTopRight)
{   //intializes internal node of a Tree with rectangle whose bottom left and topright corners are given

	Region rgnRect = initRgnRect(iBottomLeft, iTopRight);
	// initializes a rectangle with the given coordonates for the bottom left and top right corners

	TreeData tdInfo = (TreeData)malloc(sizeof(*tdInfo));

	if(tdInfo == NULL)
		return NULL;

	tdInfo->rgnRect = rgnRect;
    TreeNode tnInfo = (TreeNode)malloc(sizeof(*tnInfo));

	if(tnInfo == NULL)
		return NULL;

	tnInfo->ndType = INTNODE;
	tnInfo->tdInfo = tdInfo;

	return tnInfo;
}

TreeNode initExtNd(Data dataClstElem)
{  // initializes the external node of a tree with Data
   if(dataClstElem == NULL)
		return NULL;

	TreeNode tnInfo = (TreeNode)malloc(sizeof(*tnInfo));

	if(tnInfo == NULL)
		return NULL;

	TreeData tdInfo = (TreeData)malloc(sizeof(*tdInfo));
    if(tdInfo == NULL)
    {   free(tnInfo);
		return NULL;
	}

	tdInfo->dataClstElem = dataClstElem;	// Data
    tnInfo->ndType = EXTNODE;	// external node
	tnInfo->tdInfo = tdInfo;

	return tnInfo;
}

HdrNd createRoot(HdrNd hdrNdTree)
{   //in case of root split this is called to create a new root
    HdrNd hdrNdRoot = initHdrNd();
    Dimension iBottomLeft = (Dimension)calloc(DIMENSION, sizeof(dimension));
	Dimension iTopRight = (Dimension)calloc(DIMENSION,sizeof(dimension));

	LstNd lstNdTemp = hdrNdTree->ptrFirstNd;
	int iCnt = 0;
	Boolean bIsFirst = TRUE;

   //set the bottom left and top right corners for the new root
	while(lstNdTemp != NULL)
    {	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
        {   if(bIsFirst)
            {   iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
         	}
			else
            {   if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < iBottomLeft[iCnt])
					iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > iTopRight[iCnt])
					iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
    	    }
		}
		lstNdTemp = lstNdTemp->ptrNextNd;
		bIsFirst = FALSE;
	}

//initialize a node with the bottomleft and top right corners obtained and insert into the list
	hdrNdRoot->ptrFirstNd = initLstNd(initIntNd(iBottomLeft, iTopRight));
    hdrNdRoot->ptrFirstNd->ptrChildLst = hdrNdTree;
	hdrNdRoot->uiCnt = 1;

	return hdrNdRoot;
}

Boolean expansionArea(Region rgnRect, TreeNode tnInfo, Double ptrDMinExp, Region rgnNewRect)
{   // calculates if the area by which the rgnRect should be enlarged so as to include the tnInfo is less than the value pointed by ptrDMinExp and return TRUE and assigns rgnNewRect with the new enlarged rectangle.
  //   if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// printf("start of expansionArea\n");
    int iCnt = 0;
    Region rgnRectTemp = initRgnRect(NULL, NULL);
    for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   switch(tnInfo->ndType)
        {   case INTNODE:
            //assign least of bottom left corner along each dimension to rgnRectTemp
			rgnRectTemp->iTopRight[iCnt] = (tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > rgnRect->iTopRight[iCnt]) ? tnInfo->tdInfo->rgnRect->iTopRight[iCnt] : rgnRect->iTopRight[iCnt];
			rgnRectTemp->iBottomLeft[iCnt] = (tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < rgnRect->iBottomLeft[iCnt]) ? tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] : rgnRect->iBottomLeft[iCnt];
            break;

		    case EXTNODE:
           //assign maximum of top right corner along each dimension to rgnRectTemp
			rgnRectTemp->iTopRight[iCnt] = (tnInfo->tdInfo->dataClstElem->iData[iCnt] > rgnRect->iTopRight[iCnt]) ? tnInfo->tdInfo->dataClstElem->iData[iCnt] : rgnRect->iTopRight[iCnt];
			rgnRectTemp->iBottomLeft[iCnt] = (tnInfo->tdInfo->dataClstElem->iData[iCnt] < rgnRect->iBottomLeft[iCnt]) ? tnInfo->tdInfo->dataClstElem->iData[iCnt] : rgnRect->iBottomLeft[iCnt];
			break;
		}
	}
    //calculate the difference in area for new rectangle and old rectangle
	double dExp = area(rgnRectTemp) - area(rgnRect);
    //in case there no value to compare ( -1 ) or incase the value is less than the value to be comparedcopy the rgnRectTemp to rgnRectNew to Return it.
	if(*ptrDMinExp == -1 || dExp <= *ptrDMinExp)
    {   if(dExp == *ptrDMinExp)
			*ptrDMinExp = 0 - dExp;
		else
			*ptrDMinExp = dExp;
        for(iCnt =0; iCnt< DIMENSION; iCnt++)
        {	rgnNewRect->iBottomLeft[iCnt] = rgnRectTemp->iBottomLeft[iCnt];
			rgnNewRect->iTopRight[iCnt] = rgnRectTemp->iTopRight[iCnt];
		}
		free(rgnRectTemp->iBottomLeft);
		free(rgnRectTemp->iTopRight);
		free(rgnRectTemp);
    //area to be enlarged is less than the previous value
		// if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// printf("end of expansionArea\n");
		return TRUE;
	}

	free(rgnRectTemp->iBottomLeft);
	free(rgnRectTemp->iTopRight);
	free(rgnRectTemp);
// if(tnInfo->tdInfo->dataClstElem->iNum==620)
// 		printf("end of expansionArea\n");
    //area to be enlarged is not less than the previous value
	return FALSE;
}

double area(Region rgnRect)
{   //calcluates the area of rectangle
    if(rgnRect == NULL)
		return 0;
    double dArea = 1;
	int iCnt = 0;
    //multiply values along each dimension
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
		dArea = dArea * (rgnRect->iTopRight[iCnt] - rgnRect->iBottomLeft[iCnt]);
	return dArea;
}

LstNd pickChild(HdrNd ptrChildLst, TreeNode tnInfo)
{  
	// decides which node among the child nodes to be picked for insertion and returns a pointer to that node
    if(ptrChildLst == NULL)
		return NULL;

	LstNd lstNdTemp = ptrChildLst->ptrFirstNd;
	LstNd lstNdChild = NULL;
	double dMinExp = -1;
    Region rgnNewRect = initRgnRect(NULL, NULL);
    Region rgnFinalRect = initRgnRect(NULL, NULL);
    int iCnt;
    // for each child child in the list of child nodes do the following
  //   if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// printf("start of pickchild\n");
	while(lstNdTemp != NULL)
    {   //call the expansionArea function to determine the are by which the child node has to enlarged to accomodate the new point or region.
		if(expansionArea(lstNdTemp->tnInfo->tdInfo->rgnRect, tnInfo, &dMinExp, rgnNewRect))
        {//if the expansionArea return true mark the node to be the one that might be picked. if the expansion is same as one of the previous nodes then compare the ares of the current noe and the previous node and pick the one with least area.
               if(dMinExp < 0)
               {     dMinExp = 0 - dMinExp;
                     if(area(lstNdChild->tnInfo->tdInfo->rgnRect) > area(lstNdTemp->tnInfo->tdInfo->rgnRect))
                     {
                     	     lstNdChild = lstNdTemp;
                     	     for(iCnt =0; iCnt< DIMENSION; iCnt++)
					         {	rgnFinalRect->iBottomLeft[iCnt] = rgnNewRect->iBottomLeft[iCnt];
								rgnFinalRect->iTopRight[iCnt] = rgnNewRect->iTopRight[iCnt];
							 }
                     }
               }
			   else
				{
				   lstNdChild = lstNdTemp;
				   for(iCnt =0; iCnt< DIMENSION; iCnt++)
		         	{	rgnFinalRect->iBottomLeft[iCnt] = rgnNewRect->iBottomLeft[iCnt];
						rgnFinalRect->iTopRight[iCnt] = rgnNewRect->iTopRight[iCnt];
				 	}
				}
		}
		// if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// {
		// 	printf("before changing lstNdTemp\n");
		// 	printNode(lstNdTemp);
		// }
    	lstNdTemp = lstNdTemp->ptrNextNd;
  //   	if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// {
		// 	printf("after changing lstNdTemp\n");
		// 	printNode(lstNdTemp);
		// 	getchar();
		// }

	}
	// if(tnInfo->tdInfo->dataClstElem->iNum==620)
	// 	printf("end of pickchild\n");
    //for the node that is picked assign the region pointed by new rectangle region and return the node
    Region rgnRectTemp = lstNdChild->tnInfo->tdInfo->rgnRect;
	lstNdChild->tnInfo->tdInfo->rgnRect =  rgnFinalRect;
	//rgnNewRect = NULL;

	free(rgnRectTemp->iBottomLeft);
	free(rgnRectTemp->iTopRight);
	free(rgnRectTemp);

	free(rgnNewRect->iBottomLeft);
	free(rgnNewRect->iTopRight);
	free(rgnNewRect);
	//checkForCycle(ptrChildLst);
	return lstNdChild;
}

// void pickSeeds(HdrNd ptrChildLst, LstNd *lstNdChildOne, LstNd *lstNdChildTwo)
// {    //in case a node has to be split pick the two child nodes that are used to create new child lists
// 	if(ptrChildLst == NULL)
// 		return;
//     //used to store pointers to nodes of maximum bottom left and minimum top right corners along each dimension
// 	TreeNode *tnInfoMin = (TreeNode *)malloc(DIMENSION * sizeof(TreeNode));
// 	TreeNode *tnInfoMax = (TreeNode *)malloc(DIMENSION * sizeof(TreeNode));

// 	LstNd lstNdTemp = NULL;
//     int iCnt = 0;
// 	Boolean bIsFirst = TRUE;

// 	switch(ptrChildLst->ptrFirstNd->tnInfo->ndType)
//     {	case INTNODE:   lstNdTemp = ptrChildLst->ptrFirstNd;
//                         //for storing maximum bottomleft and minimum top right corners that are found till now
// 	                    Region rgnRectTemp = initRgnRect(NULL, NULL);
//                         //for storing the minim bottom left and maximum topr right corner
// 	                    Region rgnRectOut = initRgnRect(NULL, NULL);
//                         while(lstNdTemp != NULL)
//                         {     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
//                               {     
//                               	if(bIsFirst)
//                                     {     //if this is first node set its corners to max bottom left and min top right corners
// 				                          rgnRectTemp->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
// 				                          rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
// 				                          rgnRectOut->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
// 				                          rgnRectOut->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
//                                           //set the marker to the at the iCnt to dimesnion to current node if it has the max bottom left and min top right corners.
// 				                          tnInfoMin[iCnt] = lstNdTemp->tnInfo;
// 				                          tnInfoMax[iCnt] = lstNdTemp->tnInfo;
// 				                          continue;
//                                     }
// 			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] > rgnRectTemp->iBottomLeft[iCnt])
//                                     {     //if the current node has greater bottom left corner than the node pointed by the tnInfoMin[ iCnt ] along iCnt Dimension assign it to tnInfoMin[ iCnt ]
// 				                          rgnRectTemp->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
// 				                          tnInfoMin[iCnt] = lstNdTemp->tnInfo;
//                                     }
// 			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] < rgnRectTemp->iTopRight[iCnt])
//                                     {     //if the current node has lower top right corner than the node pointed by the tnInfoMax[ iCnt ] along iCnt Dimension assign it to tnInfoMax[ iCnt ]
// 				                          rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
// 				                          tnInfoMax[iCnt] = lstNdTemp->tnInfo;
//                                     }
// 			                        else
//                                     {     //make sure that the max bottom left and min top right are not for the same rectangle
// 				                          if(tnInfoMin[iCnt] == tnInfoMax[iCnt])
//                                                 tnInfoMax[ iCnt ] = lstNdTemp->tnInfo;
//                                     }
//                                     //sotre the minimum bottom left corner along each dimension
// 			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < rgnRectOut->iBottomLeft[iCnt])
//                                                 rgnRectOut->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
//                                     //sotre the maximum top right corner along each dimension
// 			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > rgnRectOut->iTopRight[iCnt])
//                                                 rgnRectOut->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
//                                }
//                                lstNdTemp = lstNdTemp->ptrNextNd;
// 		                       if(bIsFirst)
//                                     bIsFirst = FALSE;
//                          }	//while
// 	                     double dNormSep = 0;
// 	                     double dMaxNormSep = 0;
// 	                     dimension dimMaxNormSep = 0;
// 	                     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
//                          {     //calculate normal seperation along each dimension
//                                dNormSep = fabs(rgnRectTemp->iBottomLeft[iCnt] - rgnRectTemp->iTopRight[iCnt]) / fabs(rgnRectOut->iTopRight[iCnt] - rgnRectOut->iBottomLeft[iCnt]);
//                                if(dNormSep > dMaxNormSep)
//                                {   dMaxNormSep = dNormSep;
// 			                       dimMaxNormSep = iCnt;
//                                }
//                          }
//                          // remove the node pointed by tnInfoMin at tnInfoMax at dMaxNormSep and assign them to be the two nodes that are picked for split
//                          // printf("inside pickSeeds\n");
//                          // printAllChildrenInList(ptrChildLst);
//                          //  printf("printing first node to be deleted\n");
//                          // printTnInfo(tnInfoMin[(int)dimMaxNormSep]);
// 	                     *lstNdChildOne = deleteLstElem(ptrChildLst, tnInfoMin[(int)dimMaxNormSep]);
// 	                     // printf("printing first node deleted\n");
// 	                     // printNode(*lstNdChildOne);
// 	                     // printf("intermediate pickSeeds\n");
//                       //    printAllChildrenInList(ptrChildLst);
//                       //    printf("printing second node to be deleted\n");
//                       //    printTnInfo(tnInfoMax[(int)dimMaxNormSep]);
// 	                     // if(tnInfoMin[(int)dimMaxNormSep]==tnInfoMax[(int)dimMaxNormSep])
// 	                     // 		printAllChildrenInList(ptrChildLst);
// 	                     *lstNdChildTwo = deleteLstElem(ptrChildLst, tnInfoMax[(int)dimMaxNormSep]);
// 	                      // if(tnInfoMin[(int)dimMaxNormSep]==tnInfoMax[(int)dimMaxNormSep])
// 	                     	// 	printAllChildrenInList(ptrChildLst);
// 	     //                 printf("printing second node deleted\n");
// 	     //                 printNode(*lstNdChildTwo);
// 						// printf("after both deletion\n");
//       //                    printAllChildrenInList(ptrChildLst);
// 	                     if(*lstNdChildTwo==NULL)//in case both seeds pick up same child
// 	                     {
// 	                     	// if(ptrChildLst->uiCnt==0)
// 	                     	// 	printf("list isLstEmpty\n");
// 	                     	// printf("lstNdTemp NULL %d\n",ptrChildLst->uiCnt);
// 	                     	// printAllChildrenInList(ptrChildLst);
// 	                     	// getchar();
// 	                     	*lstNdChildTwo = deleteLstFirst(ptrChildLst);
// 	                     	// printAllChildrenInList(ptrChildLst);
// 	                     	// getchar();
// 	                     }
// 	                     // if(*lstNdChildTwo==NULL)
// 	                     // 	printf("lstNdChildTwo nul\n");
// 	                     free(rgnRectTemp->iBottomLeft);
// 	                     free(rgnRectTemp->iTopRight);
// 	                     free(rgnRectTemp);
// 	                     free(rgnRectOut->iBottomLeft);
// 	                     free(rgnRectOut->iTopRight);
// 	                     free(rgnRectOut);
//                          break;

// 	case EXTNODE:        //same as in case of INTNODE but only with the difference that max seperation instead of normal seperation is picked.. so just find two nodes which are at the farthest distance along each dimension
// 	                     lstNdTemp = ptrChildLst->ptrFirstNd;
// 	                     DataPoint iDataMin = (DataPoint)calloc(DIMENSION, sizeof(dataPoint));
// 	                     DataPoint iDataMax = (DataPoint)calloc(DIMENSION, sizeof(dataPoint));
//                          bIsFirst = TRUE;
// 	                     while(lstNdTemp != NULL)
//                          {     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
//                                {   if(bIsFirst)
//                                    {    iDataMin[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
// 				                        iDataMax[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
//                                        	tnInfoMin[iCnt] = lstNdTemp->tnInfo;
// 				                        tnInfoMax[iCnt] = lstNdTemp->tnInfo;
// 			                         	continue;
//                                    }
// 			                       if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt] <= iDataMin[iCnt])
//                                    {    iDataMin[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
// 				                        tnInfoMin[iCnt] = lstNdTemp->tnInfo;
//                                	   }
// 		                           if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt] > iDataMax[iCnt])
//                                    {    iDataMax[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
//                                         tnInfoMax[iCnt]  = lstNdTemp->tnInfo;
//                                    }
//                            	    }
//                                 lstNdTemp = lstNdTemp->ptrNextNd;
//                                	if(bIsFirst)
//                                     bIsFirst = FALSE;
//                          } //while
// 	                     double dSep = 0;
// 	                     double dMaxSep = 0;
// 	                     dimension dimMaxSep = 0;
// 	                     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
//                          {      dSep = fabs(iDataMax[iCnt] - iDataMin[iCnt]);
// 		                        if(dSep > dMaxSep)
//                                 {   dMaxSep = dSep;
// 			                        dimMaxSep = iCnt;
// 	                            }
//                          }
// 	                     *lstNdChildOne = deleteLstElem(ptrChildLst, tnInfoMin[(int)dimMaxSep]);
// 	                     *lstNdChildTwo = deleteLstElem(ptrChildLst, tnInfoMax[(int)dimMaxSep]);

// 	                     free(iDataMin);
// 	                     free(iDataMax);
//                          break;
// 	}//switch

// 	free(tnInfoMin);
// 	free(tnInfoMax);

// 	return;
// }
void pickSeeds(HdrNd ptrChildLst, LstNd *lstNdChildOne, LstNd *lstNdChildTwo)
{    //in case a node has to be split pick the two child nodes that are used to create new child lists
	if(ptrChildLst == NULL)
		return;
    //used to store pointers to nodes of maximum bottom left and minimum top right corners along each dimension
	TreeNode *tnInfoMin = (TreeNode *)malloc(DIMENSION * sizeof(TreeNode));
	TreeNode *tnInfoMax = (TreeNode *)malloc(DIMENSION * sizeof(TreeNode));
	TreeNode temp;
	LstNd lstNdTemp = NULL;
    int iCnt = 0;
	Boolean bIsFirst = TRUE;

	switch(ptrChildLst->ptrFirstNd->tnInfo->ndType)
    {	case INTNODE:   lstNdTemp = ptrChildLst->ptrFirstNd;
                        //for storing maximum bottomleft and minimum top right corners that are found till now
	                    Region rgnRectTemp = initRgnRect(NULL, NULL);
	                    // Region rgnRectTemp2= initRgnRect(NULL, NULL);
	           //          for(iCnt = 0; iCnt < DIMENSION; iCnt++)
            //             {
            //             	if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] > lstNdTemp->ptrNextNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt])
            //             	{
            //             		rgnRectTemp->iBottomLeft[iCnt] =   lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
            //             		rgnRectTemp->iTopRight[iCnt] = lstNdTemp->ptrNextNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];

            //             	}
            //             	else
            //             	{
            //             		rgnRectTemp->iBottomLeft[iCnt] =   lstNdTemp->ptrNextNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
            //             		rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];

            //             	}
                        	 
				        // rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
            //             }
	                    
                        //for storing the minim bottom left and maximum topr right corner
	                    Region rgnRectOut = initRgnRect(NULL, NULL);
                        while(lstNdTemp != NULL)
                        {     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
                              {     
                              	if(bIsFirst)
                                    {     //if this is first node set its corners to max bottom left and min top right corners
				                          rgnRectTemp->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				                          rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
				                          rgnRectOut->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				                          rgnRectOut->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
                                          //set the marker to the at the iCnt to dimesnion to current node if it has the max bottom left and min top right corners.
				                          tnInfoMin[iCnt] = lstNdTemp->tnInfo;
				                          tnInfoMax[iCnt] = lstNdTemp->tnInfo;
				                          continue;
                                    }
			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] > rgnRectTemp->iBottomLeft[iCnt])
                                    {     //if the current node has greater bottom left corner than the node pointed by the tnInfoMin[ iCnt ] along iCnt Dimension assign it to tnInfoMin[ iCnt ]
				                          rgnRectTemp->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
				                          tnInfoMin[iCnt] = lstNdTemp->tnInfo;
                                    }
			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] < rgnRectTemp->iTopRight[iCnt])
                                    {     //if the current node has lower top right corner than the node pointed by the tnInfoMax[ iCnt ] along iCnt Dimension assign it to tnInfoMax[ iCnt ]
				                          rgnRectTemp->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
				                          tnInfoMax[iCnt] = lstNdTemp->tnInfo;
                                    }
			                        // else
                           //          {     //make sure that the max bottom left and min top right are not for the same rectangle
				                       //    if(tnInfoMin[iCnt] == tnInfoMax[iCnt])
                           //                      tnInfoMax[ iCnt ] = lstNdTemp->tnInfo;
                           //          }
                                    //sotre the minimum bottom left corner along each dimension
			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] < rgnRectOut->iBottomLeft[iCnt])
                                                rgnRectOut->iBottomLeft[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt];
                                    //sotre the maximum top right corner along each dimension
			                        if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] > rgnRectOut->iTopRight[iCnt])
                                                rgnRectOut->iTopRight[iCnt] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt];
                               }
                               lstNdTemp = lstNdTemp->ptrNextNd;
		                       if(bIsFirst)
                                    bIsFirst = FALSE;
                         }	//while
	                     double dNormSep = 0;
	                     double dMaxNormSep = 0;
	                     dimension dimMaxNormSep = 0;
	                     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
                         {     //calculate normal seperation along each dimension
                               dNormSep = fabs(rgnRectTemp->iBottomLeft[iCnt] - rgnRectTemp->iTopRight[iCnt]) / fabs(rgnRectOut->iTopRight[iCnt] - rgnRectOut->iBottomLeft[iCnt]);
                               if(dNormSep > dMaxNormSep)
                               {   dMaxNormSep = dNormSep;
			                       dimMaxNormSep = iCnt;
                               }
                         }
                         if(tnInfoMin[(int)dimMaxNormSep] == tnInfoMax[(int)dimMaxNormSep])
                         {
                         	lstNdTemp = ptrChildLst->ptrFirstNd;
                         	temp=tnInfoMax[(int)dimMaxNormSep];
                         	if(temp != lstNdTemp->tnInfo)
                         	{
                         		rgnRectTemp->iTopRight[(int)dimMaxNormSep] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[(int)dimMaxNormSep];
                         		 tnInfoMax[(int)dimMaxNormSep] = lstNdTemp->tnInfo;
                         		lstNdTemp = lstNdTemp->ptrNextNd;
                         	}
                         	else
                         	{
                         		rgnRectTemp->iTopRight[(int)dimMaxNormSep] = lstNdTemp->ptrNextNd->tnInfo->tdInfo->rgnRect->iTopRight[(int)dimMaxNormSep];
                         		tnInfoMax[(int)dimMaxNormSep] = lstNdTemp->ptrNextNd->tnInfo;
                         		lstNdTemp = lstNdTemp->ptrNextNd->ptrNextNd;
                         	}
                         	while(lstNdTemp != NULL)
                         	{
                         		if(lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[(int)dimMaxNormSep] < rgnRectTemp->iTopRight[(int)dimMaxNormSep] && temp != lstNdTemp->tnInfo)
                         		{
                         			rgnRectTemp->iTopRight[(int)dimMaxNormSep] = lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[(int)dimMaxNormSep];
				                          tnInfoMax[(int)dimMaxNormSep] = lstNdTemp->tnInfo;
                         		}
                         			lstNdTemp = lstNdTemp->ptrNextNd;
                         	} 
                         }
                         if(tnInfoMin[(int)dimMaxNormSep]==tnInfoMax[(int)dimMaxNormSep])
                         {
                         	printf("error in the code\n");
                         	exit(-1);
                         }
                         // remove the node pointed by tnInfoMin at tnInfoMax at dMaxNormSep and assign them to be the two nodes that are picked for split
                         // printf("inside pickSeeds\n");
                         // printAllChildrenInList(ptrChildLst);
                         //  printf("printing first node to be deleted\n");
                         // printTnInfo(tnInfoMin[(int)dimMaxNormSep]);
	                     *lstNdChildOne = deleteLstElem(ptrChildLst, tnInfoMin[(int)dimMaxNormSep]);
	                     // printf("printing first node deleted\n");
	                     // printNode(*lstNdChildOne);
	                     // printf("intermediate pickSeeds\n");
                      //    printAllChildrenInList(ptrChildLst);
                      //    printf("printing second node to be deleted\n");
                      //    printTnInfo(tnInfoMax[(int)dimMaxNormSep]);
	                     // if(tnInfoMin[(int)dimMaxNormSep]==tnInfoMax[(int)dimMaxNormSep])
	                     // 		printAllChildrenInList(ptrChildLst);
	                     *lstNdChildTwo = deleteLstElem(ptrChildLst, tnInfoMax[(int)dimMaxNormSep]);
	                      // if(tnInfoMin[(int)dimMaxNormSep]==tnInfoMax[(int)dimMaxNormSep])
	                     	// 	printAllChildrenInList(ptrChildLst);
	     //                 printf("printing second node deleted\n");
	     //                 printNode(*lstNdChildTwo);
						// printf("after both deletion\n");
      //                    printAllChildrenInList(ptrChildLst);
	                     // if(*lstNdChildTwo==NULL)//in case both seeds pick up same child
	                     // {
	                     // 	// if(ptrChildLst->uiCnt==0)
	                     // 	// 	printf("list isLstEmpty\n");
	                     // 	// printf("lstNdTemp NULL %d\n",ptrChildLst->uiCnt);
	                     // 	// printAllChildrenInList(ptrChildLst);
	                     // 	// getchar();
	                     // 	*lstNdChildTwo = deleteLstFirst(ptrChildLst);
	                     // 	// printAllChildrenInList(ptrChildLst);
	                     // 	// getchar();
	                     // }
	                     // if(*lstNdChildTwo==NULL)
	                     // 	printf("lstNdChildTwo nul\n");
	                     free(rgnRectTemp->iBottomLeft);
	                     free(rgnRectTemp->iTopRight);
	                     free(rgnRectTemp);
	                     free(rgnRectOut->iBottomLeft);
	                     free(rgnRectOut->iTopRight);
	                     free(rgnRectOut);
                         break;

	case EXTNODE:        //same as in case of INTNODE but only with the difference that max seperation instead of normal seperation is picked.. so just find two nodes which are at the farthest distance along each dimension
	                     lstNdTemp = ptrChildLst->ptrFirstNd;
	                     DataPoint iDataMin = (DataPoint)calloc(DIMENSION, sizeof(dataPoint));
	                     DataPoint iDataMax = (DataPoint)calloc(DIMENSION, sizeof(dataPoint));
                         bIsFirst = TRUE;
	                     while(lstNdTemp != NULL)
                         {     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
                               {   if(bIsFirst)
                                   {    iDataMin[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
				                        iDataMax[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
                                       	tnInfoMin[iCnt] = lstNdTemp->tnInfo;
				                        tnInfoMax[iCnt] = lstNdTemp->tnInfo;
			                         	continue;
                                   }
			                       if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt] <= iDataMin[iCnt])
                                   {    iDataMin[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
				                        tnInfoMin[iCnt] = lstNdTemp->tnInfo;
                               	   }
		                           if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt] > iDataMax[iCnt])
                                   {    iDataMax[iCnt] = lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt];
                                        tnInfoMax[iCnt]  = lstNdTemp->tnInfo;
                                   }
                           	    }
                                lstNdTemp = lstNdTemp->ptrNextNd;
                               	if(bIsFirst)
                                    bIsFirst = FALSE;
                         } //while
	                     double dSep = 0;
	                     double dMaxSep = 0;
	                     dimension dimMaxSep = 0;
	                     for(iCnt = 0; iCnt < DIMENSION; iCnt++)
                         {      dSep = fabs(iDataMax[iCnt] - iDataMin[iCnt]);
		                        if(dSep > dMaxSep)
                                {   dMaxSep = dSep;
			                        dimMaxSep = iCnt;
	                            }
                         }
	                     *lstNdChildOne = deleteLstElem(ptrChildLst, tnInfoMin[(int)dimMaxSep]);
	                     *lstNdChildTwo = deleteLstElem(ptrChildLst, tnInfoMax[(int)dimMaxSep]);

	                     free(iDataMin);
	                     free(iDataMax);
                         break;
	}//switch

	free(tnInfoMin);
	free(tnInfoMax);

	return;
}
void splitNode(LstNd ptrChild)
{    
	// splits the node into two nodes
     //printf("\nBHAIYYA SPLIT\n");
     //printf("%d\t%d\n",ptrChild->tnInfo->tdInfo->dataClstElem->iData[0],ptrChild->tnInfo->tdInfo->dataClstElem->iData[0]);
	if(ptrChild == NULL || isLstEmpty(ptrChild->ptrChildLst))
		return;

	LstNd lstNdOne = NULL;
	LstNd lstNdTwo = NULL;
	TreeNode tnInfoTemp = NULL;

	LstNd lstNdTemp = NULL;

	double dExpOne = -1;
	double dExpTwo = -1;

    //pick two nodes that are farthest along any dimension
 //    printf("before pickseeds\n");
	// printAllChildrenOfNode(ptrChild);

	pickSeeds(ptrChild->ptrChildLst, &lstNdOne, &lstNdTwo);
	// printf("after pickseeds\n");
	// printAllChildrenOfNode(ptrChild);
	// checkForCycle(ptrChild->ptrChildLst);
	if(lstNdOne == NULL || lstNdTwo == NULL)
		return;
    //printf("BHAIYYA SPLIT\t");
    //printf("%d\t%d\n",ptrChild->tnInfo->tdInfo->dataClstElem->iData[0],ptrChild->tnInfo->tdInfo->dataClstElem->iData[1]);

    //create two child lists
	LstNd ptrChildTemp = initLstNd(initIntNd(NULL, NULL));
	LstNd ptrChildSib = initLstNd(initIntNd(NULL, NULL));
	TreeNode tn = initIntNd(NULL, NULL);

    //link the two child lists so that one follows the other
	ptrChildTemp->ptrChildLst = initHdrNd();
	ptrChildSib->ptrChildLst = initHdrNd();
	ptrChildSib->ptrNextNd = ptrChild->ptrNextNd;

    //insert the picked children one into each of the list
	insertLstNd(ptrChildTemp->ptrChildLst, lstNdOne);
	setRect(ptrChildTemp, lstNdOne->tnInfo);
	// printf("first node in ptrChildTemp\n");
	// printNode(lstNdOne);
	// checkForCycle(ptrChildTemp->ptrChildLst);

	insertLstNd(ptrChildSib->ptrChildLst, lstNdTwo);
	setRect(ptrChildSib, lstNdTwo->tnInfo);
	// printf("first node in ptrChildSib\n");
	// printNode(lstNdTwo);
	// checkForCycle(ptrChildSib->ptrChildLst);

	Region rgnNewRectOne = initRgnRect(NULL, NULL);
	Region rgnNewRectTwo = initRgnRect(NULL, NULL);

	Boolean bIsOne = FALSE;
	Boolean bIsNdOneInComp = FALSE;
	Boolean bIsNdTwoInComp = FALSE;

	int iCnt = 0;

	lstNdTemp = deleteLstFirst(ptrChild->ptrChildLst);

    //pick one element from the list of children of the node to be split
	while(lstNdTemp != NULL)
    {   //if one of the nodes has so few entires that all the remaining children are to be assigned set that node to be incomplete
		if(ptrChildTemp->ptrChildLst->uiCnt + ptrChild->ptrChildLst->uiCnt == MINENTRIES - 1)
			bIsNdOneInComp = TRUE;

		if(ptrChildSib->ptrChildLst->uiCnt + ptrChild->ptrChildLst->uiCnt == MINENTRIES - 1)
			bIsNdTwoInComp = TRUE;
        //if both nodes are not potentiall incomplete i.e. all the remaining children need not be assigned to it for the node not to underflow
		if(!bIsNdOneInComp && !bIsNdTwoInComp)
        {   dExpOne = -1;
		    dExpTwo = -1;
            //find the area by which the rectangle in each node should be expanded to accomodat the given rectangle
		    expansionArea(ptrChildTemp->tnInfo->tdInfo->rgnRect, lstNdTemp->tnInfo, &dExpOne, rgnNewRectOne);
		    expansionArea(ptrChildSib->tnInfo->tdInfo->rgnRect, lstNdTemp->tnInfo, &dExpTwo, rgnNewRectTwo);
            //printf("kahan:%d\t%d\t%d\t%d\n",rgnNewRectOne->iBottomLeft[0], rgnNewRectOne->iBottomLeft[1], rgnNewRectOne->iTopRight[0], rgnNewRectOne->iTopRight[1]);

            //ark the node that requires least expansion to be the list into which the child can be added
		    if(dExpOne < dExpTwo)
			     bIsOne = TRUE;
	        if(dExpOne > dExpTwo)
		         bIsOne = FALSE;
	        if(dExpOne == dExpTwo)
            {    //incase both reequired to be enlarged by same amount pick the one with lower area currently
		         double dAreaOne = area(ptrChildTemp->tnInfo->tdInfo->rgnRect);
			     double dAreaTwo = area(ptrChildSib->tnInfo->tdInfo->rgnRect);
			     if(dAreaOne < dAreaTwo)
                      bIsOne = TRUE;
                 if(dAreaOne > dAreaTwo)
                      bIsOne = FALSE;
                 if(dAreaOne == dAreaTwo)
                 {    //incase the area are same too pick the node which has lesser number of children
                      if(ptrChildTemp->ptrChildLst->uiCnt < ptrChildSib->ptrChildLst->uiCnt)
                           bIsOne = TRUE;
                      else
					       bIsOne = FALSE;
                 }
           }
		} //if
		
		else
        {   //if one of the nodes is potentially incomplete mark it to be the node to which the child has to be assigned
		    if(bIsNdOneInComp)
                  bIsOne = TRUE;
		    if(bIsNdTwoInComp)
			      bIsOne = FALSE;
		}

		if(bIsOne)
        {   //insert the new child
			insertLstNd(ptrChildTemp->ptrChildLst, lstNdTemp);
            if(bIsNdOneInComp)
            {   dExpOne = -1;
				expansionArea(ptrChildTemp->tnInfo->tdInfo->rgnRect, lstNdTemp->tnInfo, &dExpOne, rgnNewRectOne);
			}
            //expand the rectangle to accomodate new child
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
            {	ptrChildTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = rgnNewRectOne->iBottomLeft[iCnt];
				ptrChildTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = rgnNewRectOne->iTopRight[iCnt];
			}
			// printf("cycle check in ptrChildTemp inside while\n");
			// checkForCycle(ptrChildTemp->ptrChildLst);
			// printf("end of while checking\n");
		}
		else
        {   //insert the new child
   //      	printf("cycle check in ptrChildSib inside while before insertion\n\n");
   //      	printNode(lstNdTemp);
   //      	printf("\n");
			// checkForCycle(ptrChildSib->ptrChildLst);
			// printf("end of while checking\n");
			insertLstNd(ptrChildSib->ptrChildLst, lstNdTemp);
            if(bIsNdTwoInComp)
            {   dExpTwo = -1;
				expansionArea(ptrChildSib->tnInfo->tdInfo->rgnRect, lstNdTemp->tnInfo, &dExpTwo, rgnNewRectTwo);
			}
            //expand the rectangle to accomodate the new child
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
            {	ptrChildSib->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = rgnNewRectTwo->iBottomLeft[iCnt];
				ptrChildSib->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = rgnNewRectTwo->iTopRight[iCnt];
			}
			// printf("cycle check in ptrChildSib inside while\n");
			// checkForCycle(ptrChildSib->ptrChildLst);
			// printf("end of while checking\n");
		}
        //picke next node
		lstNdTemp = deleteLstFirst(ptrChild->ptrChildLst);
		
		
	}
    //set the node that is passed too first of the two nodes and set the next pointer of the second to the next pointer of the node that is passed..so that now in place of the node that is passed we have to nodes instead
	ptrChildTemp->ptrChildLst->ptrParentNd = ptrChildTemp;
	ptrChildSib->ptrChildLst->ptrParentNd = ptrChildSib;
	ptrChildTemp->ptrNextNd = ptrChildSib;
	ptrChild->ptrChildLst->ptrParentNd = NULL;
	free(ptrChild->ptrChildLst);

	free(ptrChild->tnInfo->tdInfo->rgnRect->iBottomLeft);
	free(ptrChild->tnInfo->tdInfo->rgnRect->iTopRight);
	free(ptrChild->tnInfo->tdInfo->rgnRect);

	free(ptrChild->tnInfo->tdInfo);
	free(ptrChild->tnInfo);

	ptrChild->tnInfo = ptrChildTemp->tnInfo;
	ptrChild->ptrChildLst = ptrChildTemp->ptrChildLst;
	ptrChild->ptrNextNd = ptrChildTemp->ptrNextNd;

	ptrChildTemp->ptrNextNd = NULL;
	ptrChildTemp->ptrChildLst = NULL;
	ptrChildTemp->tnInfo = NULL;

	free(rgnNewRectOne->iBottomLeft);
	free(rgnNewRectOne->iTopRight);
	free(rgnNewRectOne);
	free(rgnNewRectTwo->iBottomLeft);
	free(rgnNewRectTwo->iTopRight);
	free(rgnNewRectTwo);
	// printf("cycle check in ptrChild\n");
	// checkForCycle(ptrChild->ptrChildLst);
	// printf("cycle check in ptrChildSib\n");
	// checkForCycle(ptrChildSib->ptrChildLst);
	// printf("end of cycle check in ptrChildSib\n");
	return;
}


Boolean insertTree(HdrNd hdrNdTree, TreeNode tnInfo)
{   //insert a node into tree
	int iCnt = 0;
	// if(tnInfo->tdInfo->dataClstElem->iNum==620)
	// 	printf("insertTree entering\n");

	if(hdrNdTree->ptrFirstNd == NULL || hdrNdTree->ptrFirstNd->tnInfo->ndType == EXTNODE)
    {    //incase of the external node insert the node into the child list and if the node has more tha max entries return true to indicat that
		// if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// 	printf("external node reached\n");

		insertLstElem(hdrNdTree, tnInfo);

		// if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// 	printf("external node reached end\n");

		//should be there: printf(".... ...entries nw after insertion is : %d and limit is %d\n",hdrNdTree->uiCnt,MAXENTRIES);
		return(hdrNdTree->uiCnt > MAXENTRIES) ? TRUE : FALSE;
    }

	if(hdrNdTree->ptrFirstNd->ptrChildLst->uiCnt == 0)
		setRect(hdrNdTree->ptrFirstNd, tnInfo);

	// if(tnInfo->tdInfo->dataClstElem->iNum==620)
	// 	printf("before picking child\n");
    //pick the child into which the node can be inserted
    // checkForCycle(hdrNdTree);
    // printf("no cylce till now\n");
	
	LstNd lstNdTemp = pickChild(hdrNdTree, tnInfo);
	
	// checkForCycle(hdrNdTree);
	//expnandRect( lstNdTemp, tnInfo );
// if(tnInfo->tdInfo->dataClstElem->iNum==620)
// 		printf("after picking child\n");
   //call insert tree on the child that is picked
	if(insertTree(lstNdTemp->ptrChildLst, tnInfo))
    {   //incase the resultant insert has caused the node to over flow invoke split node
        //should be there: printf("\nBHAIYYA SPLIT\n");
        //should be there: printf("%d\t%d\t%d\n",lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[0],lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[0],isLstEmpty(lstNdTemp->ptrChildLst));
  //       checkForCycle(lstNdTemp->ptrChildLst);
  //       if(tnInfo->tdInfo->dataClstElem->iNum==620)
		// printf("before splitting node\n");
        splitNode(lstNdTemp);
 //        printf("after split node\n");
 //        checkForCycle(lstNdTemp->ptrChildLst);
 //        printf("checking for lstNdTemp->ptrChildLst complete\n");
 //        checkForCycle(hdrNdTree);
 //        printf("chcekinf for hdrnd complete\n");
 //        if(tnInfo->tdInfo->dataClstElem->iNum==620)
	// 	printf("after splitting node\n");
	// checkForCycle(lstNdTemp->ptrChildLst);
 //        checkForCycle(hdrNdTree);
	    hdrNdTree->uiCnt++;
        //if after split node is invoked the node is overflowing return treu to to its parent to let it know that node is over flowing
	    return (hdrNdTree->uiCnt > MAXENTRIES) ? TRUE : FALSE;
	}
	// if(tnInfo->tdInfo->dataClstElem->iNum==620)
	// 	printf("at the end of insert tree\n");
	return FALSE;
}

HdrNd buildRTree(DataHdr dataHdrLst)
{   // build RTree with the list of elements passed
	HdrNd hdrNdTree = initHdrNd();
	hdrNdTree->ptrFirstNd = initLstNd(initIntNd(NULL, NULL));
	hdrNdTree->uiCnt++;
	hdrNdTree->ptrFirstNd->ptrChildLst = initHdrNd();

	//DataNd dataTemp = dataHdrLst->dataFirstNd;
    ////should be there: printf("MIn %d max %d\n",MINENTRIES,MAXENTRIES);
	int cnt = 0,i;
    //retirve element one by one and insert them into tree invoking create root incase of root split
	for(i=0;i<dataHdrLst->uiCnt;i++)
	{
		//printf("i=%d\n",i );
		insertTree(hdrNdTree, initExtNd((dataHdrLst->dataClstElem)+i));
		// printf("after insertion i=%d\n",i );
		if(hdrNdTree->uiCnt > 1)
			hdrNdTree = createRoot(hdrNdTree);
		// checkForCycle(hdrNdTree);
	}
	/*
	while(dataTemp != NULL)
    {   //should be there: printf("\niNum : %d", dataTemp->dataClstElem->iNum);
		//should be there: printf("im also inside buildrtree\t%d\t%d\n",dataTemp->dataClstElem->iData[0],dataTemp->dataClstElem->iData[1]);
		insertTree(hdrNdTree, initExtNd(dataTemp->dataClstElem));
		if(hdrNdTree->uiCnt > 1)
			hdrNdTree = createRoot(hdrNdTree);

		//should be there: printf("********************************\n");
		//printTree(hdrNdTree);   
    	//should be there: printf("********************************\n");
		dataTemp = dataTemp->dataNextNd;
    }*/

	return	hdrNdTree;
}


double findDist(DataPoint iDataOne, DataPoint iDataTwo)
{   //calculate euclidean distance between two points
	double dDist = 0;
	int iCnt = 0;
	int k=0;
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
	{    dDist = dDist + pow(iDataOne[iCnt] - iDataTwo[iCnt], 2);
	}
	//printf("data1 %d,%d data2 %d,%d Distance %d\n", iDataOne[0],iDataOne[1],iDataTwo[0],iDataTwo[1],sqrt(dDist));
	//return dDist;

	return sqrt(dDist);
}

unsigned int getNeighborHood(HdrNd hdrNdTree, Data dataTemp)
{   
	//printf("Region query %d\n",++rquery);
   //retriens the number of records in the eps neighborhood.
	// if(dataTemp->iNum==42368)
	// 	printf("inside getNeighborHood %d %d %d\n",dataTemp->iNum, dataTemp->timestamp, dataTemp->iNumNew );
	if(hdrNdTree == NULL || dataTemp == NULL)
		return 0;

	Region rgnRect = initRgnRect(NULL, NULL);
	int iCnt = 0;
	unsigned int uiRecCnt = 0;
    //create a search rectangle that has boundaries at eps distance from point and pass it as search rectangle
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   
    	//if((dataTemp->iData[iCnt] - EPS) < 0)
	//		rgnRect->iBottomLeft[iCnt] = 0;
	//	else
			rgnRect->iBottomLeft[iCnt] = dataTemp->iData[iCnt] - EPS;
		rgnRect->iTopRight[iCnt] = dataTemp->iData[iCnt] + EPS;
	}

	//initialize neighbor header
	dataTemp->neighbors = initNbHdr();
    uiRecCnt = findRecords(hdrNdTree, rgnRect, dataTemp);
    //printf("uiRecCnt=%u\n",uiRecCnt );
    if(dataTemp->neighbors->nbhCnt >= MINPOINTS)
    {   
    	dataTemp->core_tag = TRUE;
        //printf("\nCore tag = %d", dataNdTemp->dataClstElem->core_tag);
    } 
	
	//dataNdTemp->dataClstElem->neighbors->nbhCnt = uiRecCnt; //number of neighbours
	
	
	free(rgnRect->iBottomLeft);
	free(rgnRect->iTopRight);
	free(rgnRect);
	//if(dataTemp->neighbors->nbhCnt > 0)
	//neighborhoodQueryCount++;//this variable stores the number of nbhood queries done for the entire program
	return uiRecCnt;
}
unsigned int getNeighborHood2(HdrNd hdrNdTree, Data dataTemp)
{   
	//printf("Region query %d\n",++rquery);
   //retriens the number of records in the eps neighborhood.
	// if(dataTemp->iNum==42368)
	// 	printf("inside getNeighborHood %d %d %d\n",dataTemp->iNum, dataTemp->timestamp, dataTemp->iNumNew );
	if(hdrNdTree == NULL || dataTemp == NULL)
		return 0;

	Region rgnRect = initRgnRect(NULL, NULL);
	int iCnt = 0;
	unsigned int uiRecCnt = 0;
    //create a search rectangle that has boundaries at eps distance from point and pass it as search rectangle
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   
	//    	if((dataTemp->iData[iCnt] - EPS) < 0)
	//		rgnRect->iBottomLeft[iCnt] = 0;
	//	else
			rgnRect->iBottomLeft[iCnt] = dataTemp->iData[iCnt] - EPS;
		rgnRect->iTopRight[iCnt] = dataTemp->iData[iCnt] + EPS;
	}

	//initialize neighbor header
	dataTemp->neighbors = initNbHdr();
    uiRecCnt = findRecords(hdrNdTree, rgnRect, dataTemp);
    //printf("uiRecCnt=%u\n",uiRecCnt );
    if(dataTemp->neighbors->nbhCnt >= MINPOINTS)
    {   
    	dataTemp->core_tag = TRUE;
        //printf("\nCore tag = %d", dataNdTemp->dataClstElem->core_tag);
    } 
	
	//dataNdTemp->dataClstElem->neighbors->nbhCnt = uiRecCnt; //number of neighbours
	
	
	free(rgnRect->iBottomLeft);
	free(rgnRect->iTopRight);
	free(rgnRect);
	//if(dataTemp->neighbors->nbhCnt > 0)
	//neighborhoodQueryCount++;//this variable stores the number of nbhood queries done for the entire program
	return uiRecCnt;
}
unsigned int getNeighborHoodUsingBruteForce(DataHdr dataList, Data dataTemp)
{   
	//printf("Region query %d\n",++rquery);
   //retriens the number of records in the eps neighborhood.
	if(dataList == NULL || dataTemp == NULL)
		return 0;

	Region rgnRect = initRgnRect(NULL, NULL);
	int iCnt = 0;
	unsigned int uiRecCnt = 0;
    //create a search rectangle that has boundaries at eps distance from point and pass it as search rectangle
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   
    	if((dataTemp->iData[iCnt] - EPS) < 0)
			rgnRect->iBottomLeft[iCnt] = 0;
		else
			rgnRect->iBottomLeft[iCnt] = dataTemp->iData[iCnt] - EPS;
		rgnRect->iTopRight[iCnt] = dataTemp->iData[iCnt] + EPS;
	}

	//initialize neighbor header
	dataTemp->neighbors = initNbHdr();
    uiRecCnt = findRecordsUsingBruteForce(dataList, rgnRect, dataTemp);

    if(dataTemp->neighbors->nbhCnt >= MINPOINTS)
    {   
    	dataTemp->core_tag = TRUE;
        //printf("\nCore tag = %d", dataNdTemp->dataClstElem->core_tag);
    } 
	
	//dataNdTemp->dataClstElem->neighbors->nbhCnt = uiRecCnt; //number of neighbours
	
	
	free(rgnRect->iBottomLeft);
	free(rgnRect->iTopRight);
	free(rgnRect);

	return uiRecCnt;
}

// unsigned int getNeighborHood_1(HdrNd hdrNdTree, Data dataTemp)
// {   
// 	//printf("Region query %d\n",++rquery);
//    //retriens the number of records in the eps neighborhood.
// 	if(hdrNdTree == NULL || dataTemp == NULL)
// 		return 0;

// 	Region rgnRect = initRgnRect(NULL, NULL);
// 	int iCnt = 0;
// 	unsigned int uiRecCnt = 0;
//     //create a search rectangle that has boundaries at eps distance from point and pass it as search rectangle
// 	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
//     {   if((dataTemp->iData[iCnt] - EPS) < 0)
// 			rgnRect->iBottomLeft[iCnt] = 0;
// 		else
// 			rgnRect->iBottomLeft[iCnt] = dataTemp->iData[iCnt] - EPS;
// 		rgnRect->iTopRight[iCnt] = dataTemp->iData[iCnt] + EPS;
// 	}

// 	//initialize neighbor header
// 	dataTemp->neighbors = initNbHdr();
//     uiRecCnt = findRecords(hdrNdTree, rgnRect, dataTemp);

//     if(dataTemp->neighbors->nbhCnt >= MINPOINTS)
//     {   
//     	dataTemp->core_tag = TRUE;
//         //printf("\nCore tag = %d", dataNdTemp->dataClstElem->core_tag);
//     } 
	
// 	//dataNdTemp->dataClstElem->neighbors->nbhCnt = uiRecCnt; //number of neighbours
	
	
// 	free(rgnRect->iBottomLeft);
// 	free(rgnRect->iTopRight);
// 	free(rgnRect);

// 	return uiRecCnt;
// }

unsigned int findRecords(HdrNd hdrNdTree, Region rgnRect, Data dataTemp)
{   
	//finds the record in the given search rectangle and returns  number of records found
    //if datapoint is passed in iData finds the number of records in eps neighborhood

	if(isLstEmpty(hdrNdTree) || rgnRect == NULL || rgnRect->iBottomLeft == NULL || rgnRect->iTopRight == NULL)
		return;

	unsigned int uiRecCnt = 0;
	int iCnt = 0;
    double t;
    static int flag = 0;
	LstNd lstNdTemp = hdrNdTree->ptrFirstNd;

	while(lstNdTemp != NULL)
    {	
    	nodesVisitedInRtree++;
		switch(lstNdTemp->tnInfo->ndType)
        {   case INTNODE:   //incase of internal node if the node is over lapping with search rectangle descend into the node and add to the count the number of new records found if any
				            if(isOverLap(lstNdTemp->tnInfo->tdInfo->rgnRect, rgnRect))
                                   uiRecCnt += findRecords(lstNdTemp->ptrChildLst, rgnRect, dataTemp);
                            //printf("\nuiRecCnt = %d", uiRecCnt);
                            break;
            case EXTNODE:   //incase of external node if the child node is contanied in serach rectangle increment the count and return the count to the parent
				            if(isContains(rgnRect, lstNdTemp->tnInfo->tdInfo->dataClstElem->iData))
                            {      if(dataTemp->iData == NULL)
                                   {     //for( iCnt = 0; iCnt < DIMENSION; iCnt++ )
						                 //	printf( "%d ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[ iCnt ] );
			                             //printf( "\n" );
			                             //printf("Never true \n"); //Sonal printed
                                         insertNeighbors(dataTemp, lstNdTemp->tnInfo->tdInfo->dataClstElem, 0);
						                 uiRecCnt++;
                                   }
					               else
                                   {      //see if the node is with in the esp neighborhood and increment the count if it is
                                          if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData != dataTemp->iData)
                                          {     //double t;
							                    if((t = findDist(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData, dataTemp->iData)) <= EPS)
                                                {
		                                        /*for( iCnt = 0; iCnt < DIMENSION; iCnt++ )
							                    printf("nbg %.0f ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
						                        printf("\n");*/
						                        //printf("\nDistance = %d", t);
								                //insertDataLstNd(dataSeeds, lstNdTemp->tnInfo->tdInfo->dataClstElem);
								                insertNeighbors(dataTemp, lstNdTemp->tnInfo->tdInfo->dataClstElem, t);
								                uiRecCnt++;
								                //printf("\nuiRecCnt = %d", uiRecCnt);
								                //printf("\nuiRecCnt : %d", uiRecCnt);
			                                    }
                                          }
		                           }
                             }
	                         break;

		}//switch
		lstNdTemp = lstNdTemp->ptrNextNd;
	}  
	return uiRecCnt;
}
unsigned int findRecordsUsingBruteForce(DataHdr dataList, Region rgnRect, Data dataTemp)
{   
	//finds the record in the given search rectangle and returns  number of records found
    //if datapoint is passed in iData finds the number of records in eps neighborhood

	if(dataList == NULL || rgnRect == NULL || rgnRect->iBottomLeft == NULL || rgnRect->iTopRight == NULL)
		return;

	unsigned int uiRecCnt = 0;
	int iCnt = 0;
    double t;
    static int flag = 0;
	//LstNd lstNdTemp = hdrNdTree->ptrFirstNd;
	Data currentDataElem;
    for(iCnt=0;iCnt<dataList->uiCnt;iCnt++)
    {
    	currentDataElem=dataList->dataClstElem + iCnt;
    	if( currentDataElem->toBeDeleted==FALSE && isContains(rgnRect, currentDataElem->iData) && currentDataElem->iData!=dataTemp->iData )
    	{
    		if((t = findDist(currentDataElem->iData, dataTemp->iData)) <= EPS)
            {
           	 	insertNeighbors(dataTemp, currentDataElem, t);
            	uiRecCnt++;
            }
    	}
    }
	// while(lstNdTemp != NULL)
 //    {	nodesVisited++;
	// 	switch(lstNdTemp->tnInfo->ndType)
 //        {   case INTNODE:   //incase of internal node if the node is over lapping with search rectangle descend into the node and add to the count the number of new records found if any
	// 			            if(isOverLap(lstNdTemp->tnInfo->tdInfo->rgnRect, rgnRect))
 //                                   uiRecCnt += findRecords(lstNdTemp->ptrChildLst, rgnRect, dataTemp);
 //                            //printf("\nuiRecCnt = %d", uiRecCnt);
 //                            break;
 //            case EXTNODE:   //incase of external node if the child node is contanied in serach rectangle increment the count and return the count to the parent
	// 			            if(isContains(rgnRect, lstNdTemp->tnInfo->tdInfo->dataClstElem->iData))
 //                            {      if(dataTemp->iData == NULL)
 //                                   {     //for( iCnt = 0; iCnt < DIMENSION; iCnt++ )
	// 					                 //	printf( "%d ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[ iCnt ] );
	// 		                             //printf( "\n" );
	// 		                             //printf("Never true \n"); //Sonal printed
 //                                         insertNeighbors(dataTemp, lstNdTemp->tnInfo->tdInfo->dataClstElem, 0);
	// 					                 uiRecCnt++;
 //                                   }
	// 				               else
 //                                   {      //see if the node is with in the esp neighborhood and increment the count if it is
 //                                          if(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData != dataTemp->iData)
 //                                          {     //double t;
	// 						                    if((t = findDist(lstNdTemp->tnInfo->tdInfo->dataClstElem->iData, dataTemp->iData)) <= EPS)
 //                                                {
	// 	                                        /*for( iCnt = 0; iCnt < DIMENSION; iCnt++ )
	// 						                    printf("nbg %.0f ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
	// 					                        printf("\n");*/
	// 					                        //printf("\nDistance = %d", t);
	// 							                //insertDataLstNd(dataSeeds, lstNdTemp->tnInfo->tdInfo->dataClstElem);
	// 							                insertNeighbors(dataTemp, lstNdTemp->tnInfo->tdInfo->dataClstElem, t);
	// 							                uiRecCnt++;
	// 							                //printf("\nuiRecCnt = %d", uiRecCnt);
	// 							                //printf("\nuiRecCnt : %d", uiRecCnt);
	// 		                                    }
 //                                          }
	// 	                           }
 //                             }
	//                          break;

	// 	}//switch
	// 	lstNdTemp = lstNdTemp->ptrNextNd;
	// }  
	return uiRecCnt;
}
Boolean isContains(Region rgnRect, DataPoint iData)
{   //return TRUE if the data point is contained in a rectangle
	int iCnt = 0;
	Boolean bIsContains = TRUE;
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   if((rgnRect->iBottomLeft[iCnt] > iData[iCnt]) || (rgnRect->iTopRight[iCnt] < iData[iCnt]))
        {   bIsContains = FALSE;
			break;
		}
	}
	return bIsContains;
}

Boolean isOverLap(Region rgnRectOne, Region rgnRectTwo)
{   //returs TRUE if the two rectangles are over lapping
	int iDim = 0;
	for(iDim = 0; iDim < DIMENSION; iDim++)
		if(rgnRectOne->iTopRight[iDim] < rgnRectTwo->iBottomLeft[iDim] || rgnRectTwo->iTopRight[iDim] < rgnRectOne->iBottomLeft[iDim])
			return FALSE;
	return TRUE;
}
Region findOverLap(Region rgnRectOne, Region rgnRectTwo)
{

// find the overlapping region of two rectangles
	//printf("before isOverLap\n");
	if( rgnRectOne == NULL || rgnRectTwo == NULL )
		return NULL;
	
	if( !isOverLap( rgnRectOne, rgnRectTwo ) )
	{
		//printf("exiting isOverLap\n");
		return NULL;
	}
	//printf("isOverLap is true\n");
	int iDim = 0;
	Region rgnRectOverLap = initRgnRect( NULL, NULL );

	for( iDim = 0; iDim < DIMENSION; iDim++ ){

		rgnRectOverLap->iBottomLeft[ iDim ] = ( rgnRectOne->iBottomLeft[ iDim ] > rgnRectTwo->iBottomLeft[ iDim ] ) ? rgnRectOne->iBottomLeft[ iDim ] : rgnRectTwo->iBottomLeft[ iDim ];
		rgnRectOverLap->iTopRight[ iDim ] = ( rgnRectOne->iTopRight[ iDim ] < rgnRectTwo->iTopRight[ iDim ] ) ? rgnRectOne->iTopRight[ iDim ] : rgnRectTwo->iTopRight[ iDim ];

	}
	//printf("end of findOverLap\n");

	return rgnRectOverLap;

}




void appendRTree(HdrNd hdrNdTree, DataHdr dataHdrLst)
{   // build RTree with the list of elements passed
	
    //DataNd dataTemp = dataHdrLst->dataFirstNd;
    //should be there: printf("MIn %d max %d\n",MINENTRIES,MAXENTRIES);
	int cnt = 0,i;
    //retirve element one by one and insert them into tree invoking create root incase of root split
    for(i=0;i<dataHdrLst->uiCnt;i++)
	{
		insertTree(hdrNdTree, initExtNd(dataHdrLst->dataClstElem+i));
	}

	/*while(dataTemp != NULL)
    {   //should be there: printf("\niNum : %d", dataTemp->dataClstElem->iNum);
		//should be there: printf("im also inside buildrtree\t%d\t%d\n",dataTemp->dataClstElem->iData[0],dataTemp->dataClstElem->iData[1]);
		insertTree(hdrNdTree, initExtNd(dataTemp->dataClstElem));
		//if(hdrNdTree->uiCnt > 1)
		//	hdrNdTree = createRoot(hdrNdTree);

		//should be there: printf("********************************\n");
		//printTree(hdrNdTree);   
    	//should be there: printf("********************************\n");
		dataTemp = dataTemp->dataNextNd;
    }*/
	//return	hdrNdTree;
}
// void deleteRecord(LstNd parent,HdrNd root,Data record)
// {
// 	Boolean check,dis;
// 	int iCnt=0;
// 	int matchrecs=0;//matching records for the point//
// 	LstNd Temp;
// 	LstNd lstNdTemp = root->ptrFirstNd;
// 	LstNd prev=lstNdTemp;
// 	HdrNd pre = root;//prev is the poinetr previous to lstndtemp--->needed for deletion//
// 	for(;lstNdTemp!=NULL;lstNdTemp=lstNdTemp->ptrNextNd)
// 	{
// 		if(lstNdTemp->tnInfo->ndType==INTNODE)
// 		{
// 			if(isContains(lstNdTemp->tnInfo->tdInfo->rgnRect,record->iData))
// 			{
// 				#ifdef PRINT
// 					printf("Checked in rect : ");
// 					for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 						printf("%f ",lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
// 					printf(",");
// 					for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 						printf("%f ",lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
// 					printf(" for the rec: ");
// 					for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 						printf("%f ",record->iData[iCnt]);
// 					printf("\n");
// 				#endif			
// 				//findLeaf(prev,lstNdTemp,record);//d is the datanode from which the element is to removed//
// 				if(lstNdTemp->ptrChildLst->ptrFirstNd!=NULL && lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->ndType==INTNODE)
// 				{
// 					deleteRecord(lstNdTemp,lstNdTemp->ptrChildLst,record);
// 					adjustRect(lstNdTemp);
// 				}
// 				else if(lstNdTemp->ptrChildLst->ptrFirstNd!=NULL) //===================IN THIS ELSE CASE CHECK FOR COUNT++++++++++++++++++++++++//
// 				{	
// 					check=lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->tdInfo->dataClstElem == record ? TRUE :FALSE;
// 					//check = matchRecord(lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->tdInfo->dataClstElem->iData,record);
// 					if(check==TRUE)
// 					{
// 						#ifdef PRINT
// 							printf("======Record Found==:");
// 							for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 								 printf("%f ",record->iData[iCnt]);
// 							printf("\n");
// 							//decrement the count
// 							printf("======the count of entries is : : %d \n",lstNdTemp->ptrChildLst->uiCnt);
// 							//deletion  done here..
// 							printf("...hence removing: ");
// 							printNode(lstNdTemp->ptrChildLst->ptrFirstNd);
// 						#endif
// 						lstNdTemp->ptrChildLst->uiCnt--;
// 						lstNdTemp->ptrChildLst->ptrFirstNd = lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd;
// 						#ifdef PRINT
// 							printNode(lstNdTemp->ptrChildLst->ptrFirstNd);
// 						#endif
// 						if((lstNdTemp->ptrChildLst->uiCnt) < MINENTRIES)
// 						{
// 							#ifdef PRINT
// 								printf("/// There is problem with the node so removing : ");
// 								printf("..............");
// 								for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 									printf("%f ",lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
// 								printf(",");
// 								for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 									printf("%f ",lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
// 								printf("\n");
// 							#endif
// 							if(prev!=lstNdTemp)
// 							{
// 								prev->ptrNextNd = lstNdTemp->ptrNextNd;
// 							}
// 							if(prev==lstNdTemp)
// 							{
// 								root->ptrFirstNd = lstNdTemp->ptrNextNd;
// 							}
// 							prev->ptrNextNd = lstNdTemp->ptrNextNd;//deletion of the node bcos of its prop..loss//
// 							root->uiCnt--;
// 							#ifdef PRINT
// 								printNode(prev->ptrNextNd);
// 								printf("\n");
// 							#endif

// 							reDistributeEnt(parent,root,lstNdTemp);
// 							adjustRect(parent);
							
// 							#ifdef PRINT
// 								printf("after.....adjust");
// 								printNode(parent);
// 								printf("\n");
// 							#endif

// 						}
// 						else
// 						{
// 							//just adjust..
// 							#ifdef PRINT
// 								printf("----No PROBLEM ---SO DELETED THE RECORD : ");
// 								for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 									printf("%f ",record->iData[iCnt]);
// 								printf("\n");
// 								printf("........Remaining entries is:%d \n",lstNdTemp->ptrChildLst->uiCnt);
// 							#endif
// 							adjustRect(lstNdTemp);
// 						}
// 					}
// 					//first node does nt match so check others//
// 					else
// 					{
// 						//we call the findleaf to check list of external nodes//
// 						//findLeaf(lstNdTemp->ptrChildLst->ptrFirstNd, lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd,record);
// 						//lstNdTemp->ptrChildLst->uiCnt--;
// 						//findLeaf(lstNdTemp->ptrChildLst->ptrFirstNd, lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd,record);
// 						#ifdef PRINT
// 							printf("--the count of entries is : : %d \n",lstNdTemp->ptrChildLst->uiCnt);
// 						#endif
// 						findLeaf(lstNdTemp->ptrChildLst,lstNdTemp->ptrChildLst->ptrFirstNd, lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd,record);
// 						if(lstNdTemp->ptrChildLst->uiCnt < MINENTRIES)
// 						{
// 							#ifdef PRINT
// 								printf("The node  ..");
// 								printNode(lstNdTemp);
// 								printf("is deleted..\n");
// 							#endif
// 							if(prev!=lstNdTemp)
// 							{
// 								//that means node that is violating is just the first in children list
// 								prev->ptrNextNd = lstNdTemp->ptrNextNd;
// 							}
// 							if(prev==lstNdTemp)
// 							{
// 								//violating node is not first node..
// 								root->ptrFirstNd = lstNdTemp->ptrNextNd;
// 							}
// 							root->uiCnt--;
// 							#ifdef PRINT
// 								printf("............................wwwwwww......parent is");
// 								printNode(parent);
// 								printf("\n");
// 							#endif
// 							reDistributeEnt(parent,root,lstNdTemp);
// 							adjustRect(lstNdTemp);
// 						}
// 						else
// 							adjustRect(lstNdTemp);
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			//this case occurs if the 2nd level irself is EXTERNAL//
// 			// check = matchRecord(root->ptrFirstNd->tnInfo->tdInfo->dataClstElem->iData,record);
// 			check=root->ptrFirstNd->tnInfo->tdInfo->dataClstElem == record ? TRUE :FALSE;
// 			if(check == TRUE)
// 			{
// 				//occurs when ist node in the list is itself matching
// 				root->ptrFirstNd = root->ptrFirstNd->ptrNextNd;
// 				root->uiCnt--; //added by Dhruv Kumar 8 feb 2014
// 				//adjustRect(
// 			}
// 			else
// 				findLeaf(root,root->ptrFirstNd, root->ptrFirstNd->ptrNextNd,record);

// 		}
// 		prev = lstNdTemp;

// 	}
// 	/*free(Temp);
// 	free(lstNdTemp);
// 	free(prev);*/
// }
Boolean deleteRecord(LstNd parent,HdrNd root,Data record)
{
	Boolean check,dis;
	int iCnt=0;
	int matchrecs=0;//matching records for the point//
	LstNd Temp;
	LstNd lstNdTemp = root->ptrFirstNd;
	LstNd prev=lstNdTemp;
	HdrNd pre = root;//prev is the poinetr previous to lstndtemp--->needed for deletion//
	for(;lstNdTemp!=NULL;lstNdTemp=lstNdTemp->ptrNextNd)
	{
		if(lstNdTemp->tnInfo->ndType==INTNODE)
		{
			if(isContains(lstNdTemp->tnInfo->tdInfo->rgnRect,record->iData))
			{
				if(lstNdTemp->ptrChildLst->ptrFirstNd!=NULL && lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->ndType==INTNODE)
				{
					check = deleteRecord(lstNdTemp,lstNdTemp->ptrChildLst,record);
					if(check==TRUE)
					{
						adjustRect(lstNdTemp);
						return check;
					}
					
				}
				else if(lstNdTemp->ptrChildLst->ptrFirstNd!=NULL) //===================IN THIS ELSE CASE CHECK FOR COUNT++++++++++++++++++++++++//
				{	
					check=lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->tdInfo->dataClstElem == record ? TRUE :FALSE;
					if(check==TRUE)
					{
						
						lstNdTemp->ptrChildLst->uiCnt--;
						lstNdTemp->ptrChildLst->ptrFirstNd = lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd;
						if((lstNdTemp->ptrChildLst->uiCnt) < MINENTRIES)
						{
							if(prev!=lstNdTemp)
							{
								prev->ptrNextNd = lstNdTemp->ptrNextNd;
							}
							if(prev==lstNdTemp)
							{
								root->ptrFirstNd = lstNdTemp->ptrNextNd;
							}
							prev->ptrNextNd = lstNdTemp->ptrNextNd;//deletion of the node bcos of its prop..loss//
							root->uiCnt--;
							reDistributeEnt(parent,root,lstNdTemp);
							adjustRect(parent);
						}
						else
						{
							adjustRect(lstNdTemp);
						}
						return TRUE;
					}
					else
					{
						check = findLeaf(lstNdTemp->ptrChildLst,lstNdTemp->ptrChildLst->ptrFirstNd, lstNdTemp->ptrChildLst->ptrFirstNd->ptrNextNd,record);
						if(check==TRUE)
						{
							if(lstNdTemp->ptrChildLst->uiCnt < MINENTRIES)
							{
								if(prev!=lstNdTemp)
								{
									prev->ptrNextNd = lstNdTemp->ptrNextNd;
								}
								if(prev==lstNdTemp)
								{
									root->ptrFirstNd = lstNdTemp->ptrNextNd;
								}
								root->uiCnt--;
								reDistributeEnt(parent,root,lstNdTemp);
								adjustRect(lstNdTemp);
							}
							else
								adjustRect(lstNdTemp);
							return check;
						}
					}
				}
			}
		}
		else
		{
			check=root->ptrFirstNd->tnInfo->tdInfo->dataClstElem == record ? TRUE :FALSE;
			if(check == TRUE)
			{
				root->ptrFirstNd = root->ptrFirstNd->ptrNextNd;
				root->uiCnt--; //added by Dhruv Kumar 8 feb 2014
				return TRUE;
			}
			else
			{
				check=findLeaf(root,root->ptrFirstNd, root->ptrFirstNd->ptrNextNd,record);
				if(check==TRUE)
					return check;
			}

		}
		prev = lstNdTemp;

	}
	return FALSE;
}
void reDistributeEnt(LstNd parent,HdrNd childHdr,LstNd removed)
{
	int iCnt;
	LstNd temp;
	LstNd cur;
	if(parent!=NULL)
	{
		cur = removed->ptrChildLst->ptrFirstNd;
		for(;cur!=NULL;cur=cur->ptrNextNd)
		{
			temp = pickChild(childHdr,cur->tnInfo);
			if( temp!=NULL && insertTree( temp->ptrChildLst, cur->tnInfo ) )
			{
				splitNode( temp );
				childHdr->uiCnt++;
			}
		}
		if(childHdr->uiCnt==1)
		{
			parent->tnInfo=childHdr->ptrFirstNd->tnInfo;
			parent->ptrChildLst=childHdr->ptrFirstNd->ptrChildLst;
		}
	}
	else
	{
		cur = removed->ptrChildLst->ptrFirstNd;
		for(;cur!=NULL;cur=cur->ptrNextNd)
		{
			temp = pickChild(childHdr,cur->tnInfo);
			if( temp=NULL && insertTree( temp->ptrChildLst, cur->tnInfo ) ){
				splitNode( temp );
				childHdr->uiCnt++;
			}
		}
		if(childHdr->uiCnt==1)
		{
			if(parent==NULL)
				printf("parent is actually NULL\n");
			else
				printf("in parent NULL case\n");
			parent->tnInfo=childHdr->ptrFirstNd->tnInfo;
			parent->ptrChildLst=childHdr->ptrFirstNd->ptrChildLst;
		}
 	}
}
//leaf node  function called for external nodes only//
Boolean findLeaf(HdrNd curLstHd,LstNd prev,LstNd cur,Data record)
{
	LstNd pre=prev;//for deletion of any node other than ist node//
	LstNd temp=cur;
	Boolean check;
	int iCnt;
		while(temp!=NULL)
		{
			if(temp->tnInfo->ndType == INTNODE)
			{
				if(isContains(temp->tnInfo->tdInfo->rgnRect,record->iData))
				{
				}
			}
			else
			{
				check= temp->tnInfo->tdInfo->dataClstElem ==record ? TRUE :FALSE;
				if(check == TRUE)
				{
					curLstHd->uiCnt--;
					pre->ptrNextNd = temp->ptrNextNd;
					return TRUE;
				}
			}
			pre=temp;
			temp = temp->ptrNextNd;
		}
		return FALSE;
}
// void reDistributeEnt(LstNd parent,HdrNd childHdr,LstNd removed)
// {
// 	#ifdef PRINT
// 	printf(".........redistributing in ...........");
// 	#endif
// 	int iCnt;
// 	LstNd temp;
// 	LstNd cur;
// 	if(parent!=NULL)
// 	{
// 		#ifdef PRINT
// 			for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 			printf(" %f ",parent->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
// 			printf("..................................\n");
// 		#endif

// 		cur = removed->ptrChildLst->ptrFirstNd;
// 		#ifdef PRINT
// 			printNode(cur);
// 		#endif
// 		for(;cur!=NULL;cur=cur->ptrNextNd)
// 		{
// 			temp = pickChild(childHdr,cur->tnInfo);
// 			#ifdef PRINT
// 				printf("......and inserted..");
// 				for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 				{
// 					if(cur->tnInfo->ndType==EXTNODE)
// 					printf(" %f ",cur->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
// 					else if(temp!=NULL)
// 					printf(" %f ",temp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);

				
// 				}
// 			#endif

// 			#ifdef PRINT
// 				printf("in ===================\n");
// 				for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 					if(temp!=NULL)
// 						printf(" %f ",temp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
// 			#endif
// 			if( temp!=NULL && insertTree( temp->ptrChildLst, cur->tnInfo ) )
// 			{
// 				#ifdef PRINT
// 				 printf(".......................y da hell it is not spltiing...\n");
// 				 #endif
// 				splitNode( temp );
// 				childHdr->uiCnt++;
// 			}
// 		}
// 		if(childHdr->uiCnt==1)
// 		{
// 			#ifdef PRINT
// 				printf("childHdr count 1\n");
// 			#endif
// 			parent->tnInfo=childHdr->ptrFirstNd->tnInfo;
// 			parent->ptrChildLst=childHdr->ptrFirstNd->ptrChildLst;
// 		}
// 		//printf(".........redistributing in ...........");

// 	}
// 	else
// 	{
// 		#ifdef PRINT
// 		printf(".........redistributing in ...........");
// 		#endif
// 		cur = removed->ptrChildLst->ptrFirstNd;
// 		for(;cur!=NULL;cur=cur->ptrNextNd)
// 		{
// 			temp = pickChild(childHdr,cur->tnInfo);
// 			if( temp=NULL && insertTree( temp->ptrChildLst, cur->tnInfo ) ){
// 				splitNode( temp );
// 				childHdr->uiCnt++;
// 			}
// 		}
// 		if(childHdr->uiCnt==1)
// 		{
// 			if(parent==NULL)
// 				printf("parent is actually NULL\n");
// 			else
// 				printf("in parent NULL case\n");
// 			parent->tnInfo=childHdr->ptrFirstNd->tnInfo;
// 			parent->ptrChildLst=childHdr->ptrFirstNd->ptrChildLst;
// 		}
// 		//changing root is left...where in if root has only one child..make dat child the root//
//  	}
// }
// //leaf node  function called for external nodes only//
// Boolean findLeaf(HdrNd curLstHd,LstNd prev,LstNd cur,Data record)
// {
// 	//HdrNd pre=childLstFst;
// 	LstNd pre=prev;//for deletion of any node other than ist node//
// 	LstNd temp=cur;
// 	Boolean check;
// 	int iCnt;
// 		while(temp!=NULL){
// 			if(temp->tnInfo->ndType == INTNODE){//shud be immaterial//
// 				if(isContains(temp->tnInfo->tdInfo->rgnRect,record->iData)){
// 				//	findLeaf(pre->ptrChildLst->ptrFirstNd,temp->ptrChildLst->ptrFirstNd,record);
// 				}
// 			//return FALSE;
// 			}
// 			else{//only this case shud arise//
// 				check= temp->tnInfo->tdInfo->dataClstElem ==record ? TRUE :FALSE;
// 				//check = matchRecord(temp->tnInfo->tdInfo->dataClstElem->iData,record);
// 				if(check == TRUE){
// 					// printf("----****Record found for: ");
// 					// for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 					// 	printf("%f ",record[iCnt]);

// 			//		printf("\n");
// 					//deletion to be done basd on 1st node of list or some middle node//
// 					//this if case occurs only once ..when ist external node in da list is matching
// 						/*if(pre->ptrFirstNd == temp){
// 							pre->ptrFirstNd = temp->ptrNextNd;
// 						}
// 						else//for deletion node other than ist node*/
// 						//deletion nt done....
// 					//if((curLstHd->uiCnt-1)>=MINENTRIES){
// 					//printf("----NO PROBLEM so deleted th node///\n");
// 					curLstHd->uiCnt--;
// 				//	printf("...++++++++++...Remaining nodes is: %d \n",curLstHd->uiCnt);
// 					//printf("...............haloooooooooooo\n");
// 					//printNode(pre->ptrNextNd);
// 					pre->ptrNextNd = temp->ptrNextNd;//deleted//--------------------------------
// 				//	printNode(pre->ptrNextNd);	//adjustRect(dataTree->ptrFirstNd);
// 					/*printf("after adjusting the parent int ..node is:");
// 					for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 						printf("%f ",curLstHd->ptrParentNd->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
// 					printf(",");
// 					for(iCnt=0;iCnt<DIMENSION;iCnt++)
// 						printf("%f ",curLstHd->ptrParentNd->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
// 					printf("\n");
// 						//return FALSE;
// 					}*/
// 					//else
// 						//return TRUE;//true indiacates that the leaf node has to be removed and merged
// 					return TRUE;
// 				}


// 			}
// 			pre=temp;
// 			temp = temp->ptrNextNd;
// 		}
// 		return FALSE;
// }
Boolean matchRecord(DataPoint rec1,DataPoint rec2)
{
	int iCnt;
	Boolean mtchTest = TRUE;
	for(iCnt=0;iCnt<DIMENSION;iCnt++){
		if(rec1[iCnt] != rec2[iCnt]){
	 		mtchTest = FALSE;
		}
	}
	return mtchTest;
}

void adjustRect(LstNd l)
{
//l whose children are internal nodes
	/*if(l->ptrChildLst->ptrFirstNd->tnInfo->ndType!=INTNODE){
		adjustLeafRect(l);*/
	if(l==NULL) return;
	int iCnt;
	//Dimension *bleft = (Dimension *)malloc(2 * (sizeof (Dimension)));
	Dimension bleft;
	//Dimension tright;
	LstNd cur = l->ptrChildLst->ptrFirstNd;
	/*for(iCnt=0;iCnt<DIMENSION;iCnt++){
		l->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ]=  cur->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ];
		l->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ]=  cur->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ];
	}*/
	if(cur!=NULL && cur->tnInfo->ndType==INTNODE)
	{
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
		{
			l->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ]=  cur->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ];
			 l->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ]=  cur->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ];
		}
		for(;cur!=NULL; cur = cur->ptrNextNd)
		{
			//adjustRect(cur);
			for(iCnt=0;iCnt<DIMENSION;iCnt++)
			{
				if(cur->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ] < l->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt])
				{
					l->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] = cur->tnInfo->tdInfo->rgnRect->iBottomLeft[ iCnt ];
				}
				if( cur->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ] > l->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ])
				{
					l->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ] = cur->tnInfo->tdInfo->rgnRect->iTopRight[ iCnt ];
				}
			}
		}

	}
	else if(cur!=NULL)
		 adjustLeafRect(l);
		 return;

}


void adjustLeafRect(LstNd l)
{
		int iCnt;
		LstNd cur = l->ptrChildLst->ptrFirstNd;
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
		{
			l->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] =  cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ];
			l->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ];
		}
		for(;cur!=NULL;cur = cur->ptrNextNd)
		{
			for(iCnt=0;iCnt<DIMENSION;iCnt++)
			{
				if(cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ] < l->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt])
				{
					l->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]=cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ];
				}
				if(cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ] > l->tnInfo->tdInfo->rgnRect->iTopRight[iCnt])
				{
					l->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] = cur->tnInfo->tdInfo->dataClstElem->iData[ iCnt ];
				}
			}
		}
}

void printTree(HdrNd hdrNdTree)
{   //travers along the tree and print the tree
	LstNd lstNdTemp = hdrNdTree->ptrFirstNd;
	int iCnt = 0;
	static int iIndent = 0;
	static int count=0;
	// if(hdrNdTree->ptrParentNd==NULL)
	// {
	// 	count=0;
	// 	iIndent=0;
	// }
	iIndent++;
	int childId=0;
	while(lstNdTemp != NULL)
     {   
     	childId++;
     for(iCnt = 0; iCnt < iIndent; iCnt++)
			printf("--- ");
		printf("child %d\n",childId );


		if(lstNdTemp->tnInfo->ndType == INTNODE)
        {   
      // if(lstNdTemp->ptrChildLst->uiCnt <2)
      // {
        	
			printf(" BottomLeft: ");
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%f ", lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]);
            printf(" TopRight: ");
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%f ", lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
            printf("\n");
            printf("child %d has %d children..\n",childId,lstNdTemp->ptrChildLst->uiCnt);
			//printf("i hav %d children..\n",lstNdTemp->ptrChildLst->uiCnt);
        
			// }
			printTree(lstNdTemp->ptrChildLst);
		}
		else
       {   
       printf(" DataPoint %d: ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iNum);
			for(iCnt = 0; iCnt < DIMENSION; iCnt++)
				printf("%f ", lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
			printf("\n");
			// printf(".. & ..TIMESTAMP is:_%d_",lstNdTemp->tnInfo->tdInfo->dataClstElem->timestamp);
			// // printf(".. & .ClustID is:_%d_",*lstNdTemp->tnInfo->tdInfo->dataClstElem->clstId);//RBK addn to test clstid consist in lstnd and dataNd
			// // printf(".. & .InitID is:_%d_",*lstNdTemp->tnInfo->tdInfo->dataClstElem->initId);
			// printf("\n");
			count++;
		}
		lstNdTemp = lstNdTemp->ptrNextNd;
	}
	iIndent--;
	if(iIndent==0)
		printf("tree count=%d\n",count );
	return;
}

void printNode(LstNd lstNdTemp)
{

	int iCnt;
	//printf("haloooooooooooooo\n");
	if(lstNdTemp==NULL) {
	printf("NULL pointer\n");
	return;}
	if(lstNdTemp->tnInfo->ndType==INTNODE){
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
            printf("%lf %lf\n",lstNdTemp->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt],lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
		// printf("  ");
		// for(iCnt=0;iCnt<DIMENSION;iCnt++)
		//     	printf("%lf ",lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
	}
	else{
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
			printf("%lf ",lstNdTemp->tnInfo->tdInfo->dataClstElem->iData[iCnt]);
		//printf(".clusterid is :%u ",*lstNdTemp->tnInfo->tdInfo->dataClstElem->clstId);
		printf(".& TimeStamp is :%d ",lstNdTemp->tnInfo->tdInfo->dataClstElem->timestamp);
		//printf(".NBRHD cnt is:%d:",lstNdTemp->tnInfo->tdInfo->dataClstElem->uiNbhood);//added by RBK

	}
}
Boolean isContainsForRegion(LstNd parent, LstNd child)
{   //return TRUE if the data point is contained in a rectangle
	int iCnt = 0;
	Boolean bIsContains = TRUE;
	Region parentRect=parent->tnInfo->tdInfo->rgnRect;
	Region childRect=child->tnInfo->tdInfo->rgnRect;
	for(iCnt = 0; iCnt < DIMENSION; iCnt++)
    {   
    	//if((parent->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] > child->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt]) || (parent->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] < child->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]))
    	if(childRect->iBottomLeft[iCnt] < parent->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] ||childRect->iBottomLeft[iCnt] > parent->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] ||  parent->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] < child->tnInfo->tdInfo->rgnRect->iTopRight[iCnt] || parent->tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt] > child->tnInfo->tdInfo->rgnRect->iTopRight[iCnt])
        {   
        	printf("parent : ");
        	printNode(parent);
        	printf("child : ");
			printNode(child);
			printTree(parent->ptrChildLst);
			getchar();
        	bIsContains = FALSE;
			break;
		}
	}
	return bIsContains;
}
void compareChildrenBoundary(LstNd lstNdTemp)
{
	LstNd currentChild=lstNdTemp->ptrChildLst->ptrFirstNd;
	while(currentChild!=NULL)
	{	
		if(currentChild->tnInfo->ndType==INTNODE)
			isContainsForRegion(lstNdTemp,currentChild);
		else
		{
			if(!isContains(lstNdTemp->tnInfo->tdInfo->rgnRect,currentChild->tnInfo->tdInfo->dataClstElem->iData))
			{
				printf("parent : ");
        		printNode(lstNdTemp);
        		printf("child : ");
				printNode(currentChild);
				printTree(lstNdTemp->ptrChildLst);
				getchar();
			}
		}
		currentChild=currentChild->ptrNextNd;
	}
	return;
}
void verifyRTree(LstNd lstNdTemp)
{
	static count=0;
	if(lstNdTemp==NULL || lstNdTemp->tnInfo->ndType==EXTNODE)
	{
		count++;
		return;
	}
	//printf("hello\n");
	// if(lstNdTemp->ptrChildLst->ptrFirstNd->tnInfo->ndType==EXTNODE)
	// {
	// 	//printf("helo 2\n");
	// 	compareChildrenBoundary(lstNdTemp);
	// 	//printf("helo 3\n");
	// 	return;
	// }
	//printf("hi\n");
	LstNd currentChild=lstNdTemp->ptrChildLst->ptrFirstNd;
	while(currentChild!=NULL)
	{	
		verifyRTree(currentChild);
		currentChild=currentChild->ptrNextNd;
	}
	compareChildrenBoundary(lstNdTemp);
	printf("count=%d\n",count );
	return;
}
void checkForCycle(HdrNd root)
{
	LstNd currentChild=root->ptrFirstNd;
	int count=0;
	while(currentChild!=NULL)
	{
		count++;
		currentChild=currentChild->ptrNextNd;
		if(count > MAXENTRIES+1)
		{
			printf("possible loop\n");
			getchar();
			while(currentChild!=NULL)
			{
				printNode(currentChild);
				currentChild=currentChild->ptrNextNd;
				getchar();
			}
		}
	}
}
void printAllChildrenOfNode(LstNd lstNdTemp)
{
	printf("node is \n");
	printNode(lstNdTemp);
	printf("printing its children\n");
	LstNd currentChild=lstNdTemp->ptrChildLst->ptrFirstNd;
	while(currentChild!=NULL)
	{
		printNode(currentChild);
		printf("\n");
		currentChild=currentChild->ptrNextNd;
	}
	printf("printing finished\n");
}
void printAllChildrenInList(HdrNd lstNdTemp)
{
	// printf("node is \n");
	// printNode(lstNdTemp);
	printf("printing list\n");
	LstNd currentChild=lstNdTemp->ptrFirstNd;
	while(currentChild!=NULL)
	{
		printNode(currentChild);
		printf("\n");
		currentChild=currentChild->ptrNextNd;
	}
	printf("printing finished\n");
}
void printTnInfo(TreeNode tnInfo)
{

	int iCnt;
	//printf("haloooooooooooooo\n");
	if(tnInfo==NULL) {
	printf("NULL pointer\n");
	return;}
	if(tnInfo->ndType==INTNODE){
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
            printf("%lf %lf\n",tnInfo->tdInfo->rgnRect->iBottomLeft[iCnt],tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
		// printf("  ");
		// for(iCnt=0;iCnt<DIMENSION;iCnt++)
		//     	printf("%lf ",lstNdTemp->tnInfo->tdInfo->rgnRect->iTopRight[iCnt]);
	}
	else{
		for(iCnt=0;iCnt<DIMENSION;iCnt++)
			printf("%lf ",tnInfo->tdInfo->dataClstElem->iData[iCnt]);
		//printf(".clusterid is :%u ",*lstNdTemp->tnInfo->tdInfo->dataClstElem->clstId);
		printf(".& TimeStamp is :%d ",tnInfo->tdInfo->dataClstElem->timestamp);
		//printf(".NBRHD cnt is:%d:",lstNdTemp->tnInfo->tdInfo->dataClstElem->uiNbhood);//added by RBK

	}
}
