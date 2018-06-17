#include "Optics.h"
int count0=0;
int count1=0;
int count3=0;
int count4=0;
int count5=0;
void Optics(DataHdr dataList, HdrNd hdrNdTree, PointHdr OrderList)
{
    Cluster_order ord, tempCO;
    int pts,i;
    PQueue * pq = pqueue_new(&compare_pq, dataList->uiCnt);
    for(i=0; i<dataList->uiCnt; i++)
    {
        if(dataList->dataClstElem[i].isProcessed == FALSE)
        {
            expandCluster(dataList, hdrNdTree, dataList->dataClstElem + i, OrderList,pq);
        }
    }
    for(i=0; i<dataList->uiCnt; i++)
    {
        if(dataList->dataClstElem[i].isProcessed == FALSE)
        {
            AddToOrderList(OrderList, i+1,dataList->dataClstElem[i].timestamp);
        }
    }
}
void expandCluster(DataHdr dataList, HdrNd hdrNdTree, Data dataTemp, PointHdr OrderList,PQueue *pq)
{
    int iCnt = 0, pts,i,j;
    Data dequeued;
    NB nb_temp,nb_next;
    Data dt=(Data)malloc(sizeof(struct data));
    pts = getNeighborHood2(hdrNdTree, dataTemp);
    //pts = getNeighborHoodUsingBruteForce(dataList, dataTemp);
    if(dataTemp->core_tag == TRUE)
    {
        //printf("%d %d\n",dataTemp->iNum,dataTemp->timestamp );
        dataTemp->isProcessed = TRUE;
        dataTemp->reachability_distance = UNDEFINED;
        dataTemp->core_distance = find_core_dist(dataTemp);
        AddToOrderList(OrderList, dataTemp->iNum,dataTemp->timestamp);
        Update(dataList, dataTemp,pq);

        while(pq->size > 0)
        {
            dequeued=(Data)pqueue_dequeue(pq);
            pts = getNeighborHood2(hdrNdTree, dequeued);
            //pts =  getNeighborHoodUsingBruteForce(dataList, dequeued);
            dequeued->isProcessed = TRUE;
            AddToSuccessorList(dataList,dequeued);
            if(dequeued->core_tag == TRUE)
            {
                dequeued->core_distance = find_core_dist(dequeued);
                Update(dataList, dequeued,pq);
            }
            else
            {
                freeNeighbourhoodList(dequeued->neighbors);
            }
            AddToOrderList(OrderList, dequeued->iNum,dequeued->timestamp);

        }
    }
    else
    {
        freeNeighbourhoodList(dataTemp->neighbors);
    }
}
void Update(DataHdr dataList, Data dataTemp,PQueue *pq)
{
    double c_dist, new_r_dist, temp;
    int i;
    NB nbTemp,freeTemp;
    Data searchNd;
    c_dist = dataTemp->core_distance;
    for(nbTemp = dataTemp->neighbors->nbFirst; nbTemp!= NULL;)
    {
        searchNd = dataList->dataClstElem + nbTemp->n -1;
        if(searchNd->isProcessed == FALSE)
        {
            new_r_dist = (c_dist > nbTemp->dist) ? c_dist : nbTemp->dist;
            if(searchNd->reachability_distance == UNDEFINED)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumR = dataTemp->iNum;
                searchNd->iNumRef = dataTemp->timestamp;
                pqueue_enqueue(pq,searchNd);
            }
            else
            {
                if(new_r_dist < searchNd->reachability_distance)
                {
                    searchNd->reachability_distance = new_r_dist;
                    searchNd->iNumR = dataTemp->iNum;
                    searchNd->iNumRef = dataTemp->timestamp;
                    decrease(searchNd,pq);
                }
            }
        }
        freeTemp=nbTemp;
        nbTemp = nbTemp->nbNext;
        free(freeTemp);
    }
    //free(dataTemp->neighbors);
}
double find_core_dist(Data dataTemp)
{
    if(dataTemp->neighbors->nbhCnt< MINPOINTS)
        return UNDEFINED;
    NbHdr nbInfo=dataTemp->neighbors;
    NB nbTemp1, nbTemp2, temp, pos;
    double min_dist;
    temp = (NB)malloc(sizeof(struct nb));
    temp->nbNext = NULL;
    int min=(nbInfo->nbhCnt >= MINPOINTS) ? MINPOINTS : nbInfo->nbhCnt;
    int i=0;
    for(nbTemp1 = nbInfo->nbFirst; nbTemp1 != NULL && i< min; nbTemp1 = nbTemp1->nbNext, i++)
    {
        min_dist = nbTemp1->dist;
        pos = nbTemp1;
        for(nbTemp2 = nbTemp1->nbNext; nbTemp2 != NULL; nbTemp2 = nbTemp2->nbNext)
        {
            if(nbTemp2->dist < min_dist)
            {
                min_dist = nbTemp2->dist;
                pos = nbTemp2;
            }
        }
        if(min_dist != nbTemp1->dist)
        {
            assign_nbData(temp, nbTemp1);
            assign_nbData(nbTemp1, pos);
            assign_nbData(pos, temp);
        }
    }
    free(temp);
    if(min==MINPOINTS)
        return min_dist;
    else
        return UNDEFINED;
}
void assign_nbData(NB temp1, NB temp2)
{
    temp1->n = temp2->n;
    temp1->dist = temp2->dist;
}
void decrease(Data searchNd,PQueue *pq)
{
    Data tmp;
    int tag = 0,i;
    for(i=0; i<pq->size; i++)
    {
        if(((Data)pq->data[i])->iNum == searchNd->iNum)
        {
            tag = 1;
            ((Data)pq->data[i])->reachability_distance = searchNd->reachability_distance;
            break;
        }
    }
    if(tag==1)
    {
        while(i > 0 && pq->cmp(pq->data[i], pq->data[PARENT(i)]) > 0)
        {
            tmp = pq->data[i];
            pq->data[i] = pq->data[PARENT(i)];
            pq->data[PARENT(i)] = tmp;
            i = PARENT(i);
        }
    }
}
void freeSuccessorList(Data data)
{
    SuccessorListHeader sList=data->sListOld;
    SuccessorNode current,temp;
    if(sList==NULL)
        return;
    current=sList->First;
    int count=0;
    while(current!=NULL)
    {
        temp=current;
        current=current->next;
        count++;
        free(temp);
    }
    sList->successorCount=0;
    sList->First = NULL;
    free(sList);
}

void AddToSuccessorList(DataHdr dataList,Data dataTemp)
{
    if(dataTemp->iNumR==-1)
        return;
    SuccessorNode sNode=(SuccessorNode)malloc(sizeof(struct successor_node));
    sNode->iNum=dataTemp->iNum;
    sNode->next=NULL;
    
    SuccessorNode current=dataList->dataClstElem[dataTemp->iNumR-1].sListOld->First;
    if(current==NULL)
    {
        dataList->dataClstElem[dataTemp->iNumR-1].sListOld->First=sNode;
        dataList->dataClstElem[dataTemp->iNumR-1].sListOld->successorCount++;
    }
    else
    {
        sNode->next=dataList->dataClstElem[dataTemp->iNumR-1].sListOld->First;
        dataList->dataClstElem[dataTemp->iNumR-1].sListOld->First=sNode;
        dataList->dataClstElem[dataTemp->iNumR-1].sListOld->successorCount++;
    }
    return;
}
Region getRegion(LstNd node)
{
  if(node->tnInfo->ndType == EXTNODE)
    return NULL;

  int iDim;
  Region temp = initRgnRect( NULL, NULL );
  if(temp==NULL)
  return(NULL);
  for( iDim = 0; iDim < DIMENSION; iDim++ )
  {
    temp->iBottomLeft[iDim] = node->tnInfo->tdInfo->rgnRect->iBottomLeft[iDim];
    temp->iTopRight[iDim] = node->tnInfo->tdInfo->rgnRect->iTopRight[iDim];
  }
  return temp;
}
void insertList(myList *head, myList node) 
{
  myList temp;
  if(*head == NULL) 
  {
    *head = node;
    node->next = NULL;
    return;
  }
  temp = *head;
  *head = node;
  node->next = temp;
  return;
}
void GetAffectedList(HdrNd RTree1, DataHdr dataList1, DataHdr dataList2)
{
    int pts, i, j;
    int count=0;
    Data dataTemp;
    Data tempNd1;
    Cluster_order ord3;
    NB nbTemp, nb_next,freetemp;
    //FILE * fp=fopen("affected_list","w");
    //PointHdr affectedObjects = initPointHdr();
    for(i = 0; i < dataList2->uiCnt; i++)
    {
        pts = getNeighborHood(RTree1, dataList2->dataClstElem+i); //dataList1->uiCnt is dumb variable
        if(pts > 0)
        {
            for(nbTemp = dataList2->dataClstElem[i].neighbors->nbFirst; nbTemp!= NULL;)
            {
                if(dataList1->dataClstElem[nbTemp->n -1].affected_tag == FALSE)
                {
                    count++;
                    dataList1->dataClstElem[nbTemp->n -1].affected_tag = TRUE;
                }
                freetemp=nbTemp;
                nbTemp=nbTemp->nbNext;
                free(freetemp);
            }
            if(dataList2->dataClstElem[i].affected_tag == FALSE)
            {
                count++;
                dataList2->dataClstElem[i].affected_tag = TRUE;
            }

        }

        //free(dataList2->dataClstElem[i].neighbors);
    }
    //fclose(fp);
    //printf("no of mutated points=%d\n",count );
}
void GetAffectedListUsingOverlapStrategy(HdrNd RTree1, HdrNd RTree2, DataHdr dataList1, DataHdr dataList2)  
{    
  Region oldRtreeRegion, newRtreeRegion,intersectRegion;
  LstNd tempOld,tempNew;
  int iDim;
  tempOld = RTree1->ptrFirstNd;
  tempNew = RTree2->ptrFirstNd;
  if(RTree1->ptrFirstNd->ptrChildLst->uiCnt==0 || RTree2->ptrFirstNd->ptrChildLst->uiCnt==0)
    return;
  while(tempOld!=NULL)
  {
    oldRtreeRegion = getRegion(tempOld);
    for( iDim = 0; iDim < DIMENSION; iDim++ )
    {
      oldRtreeRegion->iBottomLeft[ iDim ] -= EPS;
      oldRtreeRegion->iTopRight[ iDim ] += EPS;
    }
    while(tempNew!=NULL)
    {
      //nodesVisited++;
      newRtreeRegion = getRegion(tempNew);
      for( iDim = 0; iDim < DIMENSION; iDim++ )
      {
        newRtreeRegion->iBottomLeft[ iDim ] -= EPS;
        newRtreeRegion->iTopRight[ iDim ] += EPS;
      }
      intersectRegion = findOverLap(oldRtreeRegion,newRtreeRegion);
      if(intersectRegion!=NULL)
      {
        //printf(" intersection not null \n");
        //areaDenom = area(newRtreeRegion);
        findIntersectionPoints(RTree1, RTree2, dataList1, dataList2, tempOld, tempNew, intersectRegion);
        //printf(" intersection not null ..1\n");
      }
      if(intersectRegion!=NULL)
        free(newRtreeRegion);
      tempNew=tempNew->ptrNextNd;
    }
    if(oldRtreeRegion!=NULL)
      free(oldRtreeRegion);
    tempNew = RTree2->ptrFirstNd;
    tempOld = tempOld->ptrNextNd;
  }
}

void findIntersectionPoints(HdrNd hdrOldRtree, HdrNd hdrNewRtree, DataHdr dataList1, DataHdr dataList2, LstNd nodeOld, LstNd nodeNew,  Region intRegion)
{
  //printf("inside findIntersectionPoints\n");
  //count++;
  myList queueOld=NULL, queueNew=NULL, temp;
  LstNd tempOld, tempNew;
  tempOld = nodeOld->ptrChildLst->ptrFirstNd;
  tempNew = nodeNew->ptrChildLst->ptrFirstNd;
  int iDim;
  Region newIntRegion,t;
  queueOld = NULL;
  queueNew = NULL;
  if(tempNew->tnInfo->ndType == EXTNODE && tempOld->tnInfo->ndType == EXTNODE)
  {
   // printf("both nodes are external\n");
    //if(deletionFlag == FALSE)
    {
        processPenultimate(hdrOldRtree, hdrNewRtree,nodeOld, nodeNew , dataList1, dataList2, intRegion);//...*** ACTUAL IMPLEMENTATION..UNCOMMENTlater//
    }
    // if(deletionFlag == TRUE)
    // {
    //     processPenulForDeletion(hdrOldRtree, hdrNewRtree,nodeOld, nodeNew , dataList1, dataList2, intRegion);
    //     //printf("Deletion flag is True...\n");
    // }
    return;
  }

  if(tempNew->tnInfo->ndType == EXTNODE) 
  {
    //printf("tempNew is external\n");
    while(tempOld != NULL) 
    {
      Region tOld = getRegion(tempOld);
      for( iDim = 0; iDim < DIMENSION; iDim++ )
      {
        tOld->iBottomLeft[ iDim ] -= EPS;
        tOld->iTopRight[ iDim ] += EPS;
      }

      t = findOverLap(tOld, intRegion);
      if(t != NULL)
      {
        findIntersectionPoints(hdrOldRtree, hdrNewRtree, dataList1, dataList2, tempOld, nodeNew , t);
        free(t);
      }
      if(tOld!=NULL)
        free(tOld);
      tempOld = tempOld->ptrNextNd;
     // nodesVisited++;
    }
    return;
  }
  if(tempOld->tnInfo->ndType == EXTNODE) 
  {
    //printf("tempOld is external\n");
    while(tempNew != NULL) 
    {
      Region tNew = getRegion(tempNew);
      for( iDim = 0; iDim < DIMENSION; iDim++ )
      {
        tNew->iBottomLeft[ iDim ] -= EPS;
        tNew->iTopRight[ iDim ] += EPS;
      }
      t = findOverLap(tNew, intRegion);
      if(t!=NULL)
      {
        findIntersectionPoints(hdrOldRtree, hdrNewRtree, dataList1, dataList2, nodeOld, tempNew , t);
        free(t);
      }
      if(tNew!=NULL)
        free(tNew);
      tempNew = tempNew->ptrNextNd;
      //nodesVisited++;
    }
    return;
  }
  while(tempOld != NULL) 
   {
    
    Region tOld = getRegion(tempOld);
    for( iDim = 0; iDim < DIMENSION; iDim++ )
    {
      tOld->iBottomLeft[ iDim ] -= EPS;
      tOld->iTopRight[ iDim ] += EPS;
    }
    if(isOverLap(tOld,intRegion) ) 
    {
      temp = (myList) malloc(sizeof (struct mylist));
      assert(temp!=NULL);
      temp->listNode = tempOld;
      temp->next = NULL;
      insertList(&queueOld, temp);
    }
    if(tOld!=NULL)
      free(tOld);
    //nodesVisited++;
    tempOld = tempOld->ptrNextNd;
  }

  while(tempNew != NULL) 
  {
    Region tNew = getRegion(tempNew);
    for( iDim = 0; iDim < DIMENSION; iDim++ )
    {
      tNew->iBottomLeft[ iDim ] -= EPS;
      tNew->iTopRight[ iDim ] += EPS;
    }
    if(isOverLap(tNew, intRegion) ) 
    {
      temp = (myList) malloc(sizeof(struct mylist));
            assert(temp!=NULL);

      temp->listNode = tempNew;
      temp->next = NULL;
      insertList(&queueNew, temp);
    }
    if(tNew!=NULL)
      free(tNew);
    //nodesVisited++;
    tempNew = tempNew->ptrNextNd;
  }
  myList temp1, temp2;
  temp1 = queueOld;
  //printf("both are not external\n");
  while(temp1 != NULL) 
  {
   // printf("temp1 no null\n");
    temp2 = queueNew;
    Region tempRegionOld = getRegion(temp1->listNode);
    for( iDim = 0; iDim < DIMENSION; iDim++ )
    {
      tempRegionOld->iBottomLeft[ iDim ] -= EPS;
      tempRegionOld->iTopRight[ iDim ] += EPS;
    }
    while(temp2 != NULL) 
    {
      //printf("temp2 no null\n");
      Region tempRegionNew = getRegion(temp2->listNode);
      Region temp = findOverLap(tempRegionOld, intRegion);
      if(temp != NULL)
      {
        //printf("temp not null\n");
        for( iDim = 0; iDim < DIMENSION; iDim++ )
        {
          tempRegionNew->iBottomLeft[ iDim ] -= EPS;
          tempRegionNew->iTopRight[ iDim ] += EPS;
        }
        //printf("before findOverLap\n");
        newIntRegion = findOverLap(tempRegionNew, temp);
        //printf("after findOverLap\n");
        if(newIntRegion != NULL) 
        {
          //printf("newIntRegion not null\n");
          findIntersectionPoints(hdrOldRtree, hdrNewRtree, dataList1, dataList2, temp1->listNode, temp2->listNode, newIntRegion);
        }
        //else
          //printf("newIntRegion is null\n");
      }
      if(newIntRegion!=NULL)
        free(newIntRegion);
      if(tempRegionNew!=NULL)
        free(tempRegionNew);
      if(temp!=NULL)
        free(temp);
      //nodesVisited++;
      temp2 = temp2->next;
      // if(temp2==NULL)
      //   printf("temp2 is null after update\n");
      // else
      //   printf("temp2 no null after update\n");
    }
    if(tempRegionOld!=NULL)
      free(tempRegionOld);
    temp1 = temp1->next;
    // if(temp1==NULL)
    //     printf("temp1 is null after update\n");
    //   else
    //     printf("temp1 no null after update\n");
  }
  //printf("Hii\n");
 ///count--;
  //printf("exiting findIntersectionPoints\n");

}

void processPenultimate(HdrNd hdrOldRtree, HdrNd hdrNewRtree, LstNd nodeOld, LstNd nodeNew, DataHdr dataList1, DataHdr dataList2, Region intRegion)
{
  LstNd tempNd,tempNd1;
  tempNd = nodeNew->ptrChildLst->ptrFirstNd;
  Data dataPoint1,dataPoint2;
  double t;
  while(tempNd != NULL)
  {
    dataPoint1=dataList2->dataClstElem + tempNd->tnInfo->tdInfo->dataClstElem->iNum-1;
    if(isContains(intRegion,dataPoint1->iData))
    {
        tempNd1 = nodeOld->ptrChildLst->ptrFirstNd;
        while(tempNd1 != NULL)
        {
          dataPoint2=dataList1->dataClstElem + tempNd1->tnInfo->tdInfo->dataClstElem->iNum-1;
          if(isContains(intRegion, dataPoint2->iData))
          {
            if(( t= findDist(dataPoint1->iData, dataPoint2->iData)) <= EPS)
            {
                dataPoint2->neighbors->nbhCnt++;
                dataPoint1->neighbors->nbhCnt++;
                if( dataPoint2->neighbors->nbhCnt >=MINPOINTS)
                    dataPoint2->affected_tag = TRUE;
                if( dataPoint1->neighbors->nbhCnt >=MINPOINTS)
                    dataPoint1->affected_tag = TRUE;
                  //printf("%d %d\n",dataPoint2->iNum,dataPoint1->iNum );
            }
          }
          tempNd1 = tempNd1->ptrNextNd;
        }
    }
    tempNd = tempNd->ptrNextNd;
  }
}
void fixForAddition(DataHdr dataList1,DataHdr dataList2,PointHdr finalOrderList,Data currentDataNd,int i,HdrNd dataTree1, HdrNd dataTree2)
{
  DataHdr initialDataList=(currentDataNd->timestamp==0)? dataList1:dataList2;
  if(currentDataNd->fixed==0)
  {
    if(currentDataNd->core_distance!=UNDEFINED)
    {
      AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
      currentDataNd->fixed=1;
      if(currentDataNd->iNumRNew==-1)
      {
        if(currentDataNd->iNumR!=-1 && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==0)
        {
          currentDataNd->reachability_distance=UNDEFINED;
          currentDataNd->iNumR=-1;
          currentDataNd->iNumRef=-1;
        } 
      }
      #ifdef PRINT
     //printf("fixing1%d %d  %d\n",i,currentDataNd->iNum);
      #endif
    }
    else if(currentDataNd->reachability_distance!=UNDEFINED)
    {
      if(currentDataNd->iNumRNew!=-1)
      {
        AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing2%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else if(initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1)
      {
        AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing3%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else
      {
        printf("error in fixing..reachability_distance undefined but iNumR not fixed %d %d %d\n",currentDataNd->iNum, currentDataNd->iNumR,i );
        getchar();
      }
    }
  }
}
void reorganizingClusterOrdering(DataHdr dataList1,DataHdr dataList2,HdrNd dataTree1,HdrNd dataTree2,PointHdr OrderList1,PointHdr OrderList2,PointHdr ReorganizedOrderList)
{
    OrderedDataNd current=OrderList1->dataFirstNd;
    int iCnt = 0, pts, pts1, i=0, j,flag=0;
    Data currentDataNd,  dequeued, dataTemp1, dataTemp2,dataTemp;
    NB nb_temp, nb_next;
    PQueue *pq = pqueue_new(&compare_pq, dataList1->uiCnt + dataList2->uiCnt);
    while(current!=NULL)
    {
        if(dataList1->dataClstElem[current->iNum-1].affected_tag==TRUE && dataList1->dataClstElem[current->iNum-1].isProcessed==FALSE)
        {
            processAffectedPoint(dataList1->dataClstElem + current->iNum-1,dataList1,dataList2, dataTree1, dataTree2,pq);
            
        }
        
        // else
        // {

        //     fixForAddition( dataList1, dataList2,ReorganizedOrderList, dataList1->dataClstElem + current->iNum-1,1);
        // }
        if(pq->size>0)
        {
            fixForAddition( dataList1, dataList2,ReorganizedOrderList, dataList1->dataClstElem + current->iNum-1,0,dataTree1,dataTree2);
            current=current->dataNextNd;
            break;
        }
        current=current->dataNextNd;
        
    }
    while(current!=NULL)
    {
      currentDataNd=dataList1->dataClstElem+current->iNum-1;
      // if(currentDataNd->affected_tag==TRUE && currentDataNd->NeighborhoodComputed==0)
      //   computeNeighborhood_CoreDistanceAndReachabilityDist(currentDataNd,initialDataList,DataTree);
      if(currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
      
      {
        if(pq->size>0)
        {
          // if(((Data)pqueue_top(pq))->affected_tag==TRUE &&  ((Data)pqueue_top(pq))->NeighborhoodComputed==0)
          //   computeNeighborhood_CoreDistanceAndReachabilityDist(((Data)pqueue_top(pq)),initialDataList,DataTree);
          if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && dataList1->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
          {
            ProcessOtherFixedPointsOfReorganizedList(currentDataNd, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
            fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,2,dataTree1,dataTree2);
            current=current->dataNextNd;
          }
          else
          {
            dequeued=(Data)pqueue_dequeue(pq);
           // checkPQ(pq,dequeued,6);

            ProcessOtherFixedPointsOfReorganizedList(dequeued, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
             fixForAddition( dataList1, dataList2,ReorganizedOrderList, dequeued,3,dataTree1,dataTree2);
          }
        }
        else
        {
          while(current!=NULL)
          {
              currentDataNd=dataList1->dataClstElem+current->iNum-1;
              if(currentDataNd->affected_tag==TRUE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0 && currentDataNd->isProcessed==FALSE)
              {
                  processAffectedPoint(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq);
              }
              if(pq->size>0)
              {
                  fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,0,dataTree1,dataTree2);
                  current=current->dataNextNd;
                  break;
              }
              current=current->dataNextNd;              
          }
          // ProcessOtherFixedPointsOfReorganizedList(currentDataNd, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
          //   fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,4);
          // current=current->dataNextNd;
        }
      }
      else
      {
        current=current->dataNextNd;
      }

    }
    while(pq->size>0)
    {
      
      dequeued=(Data)pqueue_dequeue(pq);
     // checkPQ(pq,dequeued,7);
      // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      ProcessOtherFixedPointsOfReorganizedList(dequeued, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
             fixForAddition( dataList1, dataList2,ReorganizedOrderList, dequeued,5,dataTree1,dataTree2);
    }
    current = OrderList2->dataFirstNd;
     while(current!=NULL)
    {
      currentDataNd=dataList2->dataClstElem+current->iNum-1;
      // if(currentDataNd->affected_tag==TRUE && currentDataNd->NeighborhoodComputed==0)
      //   computeNeighborhood_CoreDistanceAndReachabilityDist(currentDataNd,initialDataList,DataTree);
      if(currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
      
      {
        if(pq->size>0)
        {
          // if(((Data)pqueue_top(pq))->affected_tag==TRUE &&  ((Data)pqueue_top(pq))->NeighborhoodComputed==0)
          //   computeNeighborhood_CoreDistanceAndReachabilityDist(((Data)pqueue_top(pq)),initialDataList,DataTree);
          if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && dataList2->dataClstElem[currentDataNd->iNumR-1].fixed==1)
          {
            ProcessOtherFixedPointsOfReorganizedList(currentDataNd, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
            fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,2,dataTree1,dataTree2);
             current=current->dataNextNd;
          }
          else
          {
            dequeued=(Data)pqueue_dequeue(pq);
            //checkPQ(pq,dequeued,8);
            ProcessOtherFixedPointsOfReorganizedList(dequeued, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
             fixForAddition( dataList1, dataList2,ReorganizedOrderList, dequeued,3,dataTree1,dataTree2);
          }
        }
        else
        {
          while(current!=NULL)
          {
              currentDataNd=dataList2->dataClstElem+current->iNum-1;
              if(currentDataNd->affected_tag==TRUE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0&& currentDataNd->isProcessed==0)
              {
                  processAffectedPoint(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq);
              }
              if(pq->size>0)
              {
                  fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,0,dataTree1,dataTree2);
                  current=current->dataNextNd;
                  break;
              }
              current=current->dataNextNd;
              
          }
          // ProcessOtherFixedPointsOfReorganizedList(currentDataNd, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
          //   fixForAddition( dataList1, dataList2,ReorganizedOrderList, currentDataNd,4);
          // current=current->dataNextNd;
        }
      }
      else
      {
        current=current->dataNextNd;
      }

    }
    while(pq->size>0)
    {
      
      dequeued=(Data)pqueue_dequeue(pq);
      //checkPQ(pq,dequeued,9);
      // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      ProcessOtherFixedPointsOfReorganizedList(dequeued, dataList1, dataList2,dataTree1,dataTree2,OrderList1,OrderList2, pq);
             fixForAddition( dataList1, dataList2,ReorganizedOrderList, dequeued,3,dataTree1,dataTree2);
    }
    //int countIndependent=0;
    current = OrderList1->dataFirstNd;
    while(current!=NULL)
    {
        if(dataList1->dataClstElem[current->iNum-1].fixed==0 )
        {

            AddToOrderList(ReorganizedOrderList,current->iNum,current->timestamp);
            //AddToNewSuccessorList(dataList1,dataList2,dataList2->dataClstElem + current->iNum-1);
            dataList1->dataClstElem[current->iNum-1].fixed=1;
            //countIndependent++;
            //printf("fixing rest of 2nd list %d %d %lf %lf\n", dataList2->dataClstElem[current->iNum-1].iNum, dataList2->dataClstElem[current->iNum-1].timestamp, dataList2->dataClstElem[current->iNum-1].core_distance, dataList2->dataClstElem[current->iNum-1].reachability_distance);
        }
        current=current->dataNextNd;
    }
    current = OrderList2->dataFirstNd;
    while(current!=NULL)
    {
        if(dataList2->dataClstElem[current->iNum-1].fixed==0 )
        {

            AddToOrderList(ReorganizedOrderList,current->iNum,current->timestamp);
            //AddToNewSuccessorList(dataList1,dataList2,dataList2->dataClstElem + current->iNum-1);
            dataList2->dataClstElem[current->iNum-1].fixed=1;
           // countIndependent++;
            //printf("fixing rest of 2nd list %d %d %lf %lf\n", dataList2->dataClstElem[current->iNum-1].iNum, dataList2->dataClstElem[current->iNum-1].timestamp, dataList2->dataClstElem[current->iNum-1].core_distance, dataList2->dataClstElem[current->iNum-1].reachability_distance);
        }
        current=current->dataNextNd;
    }
    // for(i=0; i<dataList1->uiCnt; i++)
    // {
    //     if(dataList1->dataClstElem[i].fixed==0)
    //     {
    //         AddToOrderList(ReorganizedOrderList,i+1,0);
    //         //AddToNewSuccessorList(dataList1,dataList2,dataList1->dataClstElem + i);
    //         dataList1->dataClstElem[i].fixed=1;
    //         //printf("fixing rest of 1st list %d %d %lf %lf\n", dataList1->dataClstElem[i].iNum, dataList1->dataClstElem[i].timestamp, dataList1->dataClstElem[i].core_distance, dataList1->dataClstElem[i].reachability_distance);
    //     }
    // }
    //printf("final reorganized ordered list\n");
    //PrintUpdateList(ReorganizedOrderList);
    // printf("countIndependent=%d errorCount=%d minVoileted=%d\n",countIndependent,errorCount,minVoileted );
}
void ProcessOtherFixedPointsOfReorganizedList(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PointHdr OrderList1,PointHdr OrderList2, PQueue* pq)
{
    int iCnt = 0, pts, pts1, i, j;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;

    if(dataTemp->affected_tag==TRUE&& dataTemp->isProcessed==0)
    {
        //compute neighbourhood of this point in both datalists
        processAffectedPoint(dataTemp,dataList1,dataList2, hdrNdTree1, hdrNdTree2,pq);
    }
    else
    {
        //      printf("processing %d %d\n",dataTemp->iNum,dataTemp->timestamp );
        if(dataTemp->timestamp==0 && dataTemp->core_distance!=UNDEFINED)
        {
            //predecessor of moving objects
            if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
            {
                if(dataList1->dataClstElem[dataTemp->iNumR-1].fixed==0)
                {
                    distance=findDist(dataTemp->iData,dataList1->dataClstElem[dataTemp->iNumR-1].iData);
                    new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                    // printf("new reachability_distance for predecessor %d %d is %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                    if(dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                    {
                        dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                        dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                        dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                        // printf("enqueing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                        pqueue_enqueue(pq, dataList1->dataClstElem + dataTemp->iNumR-1);
                        //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                        //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                        dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                    }
                    else
                    {
                        if(dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                        {
                            dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                            // printf("decreasing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                            decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                            dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                        }
                    }
                }
            }
            //successor of moving objects
            processSuccessors(dataTemp,dataList1,pq);
        }
        else if(dataTemp->core_distance!=UNDEFINED)
        {
            //predecessor of moving objects
            if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
            {
                if(dataList2->dataClstElem[dataTemp->iNumR-1].fixed==0)
                {
                    distance=findDist(dataTemp->iData,dataList2->dataClstElem[dataTemp->iNumR-1].iData);
                    new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                    // printf("new reachability_distance for predecessor %d %d is %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                    if(dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                    {
                        dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                        dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                        dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                        //printf("enqueing %d %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                        pqueue_enqueue(pq, dataList2->dataClstElem + dataTemp->iNumR-1);
                        //decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                        dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                    }
                    else
                    {
                        if(dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                        {
                            dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                            dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                            dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                            //printf("decreasing %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                            decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                            dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                        }
                    }
                }
                

            }
            processSuccessors(dataTemp,dataList2,pq);
        }

    }
}

void processSuccessors(Data dataTemp,DataHdr dataList1,PQueue * pq)
{
    int i;
    double new_r_dist,distance;
    SuccessorNode current=dataTemp->sListOld->First;

    while(current!=NULL)
    {
        //printf("%d\t",current->iNum );
        i=current->iNum-1;
        if(dataList1->dataClstElem[i].fixed==0)
        {
            distance=findDist(dataTemp->iData,dataList1->dataClstElem[i].iData);
            new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
            //printf("new reachability_distance for successor %d %d is %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp ,new_r_dist );
            if(dataList1->dataClstElem[i].addedToPQ==0)
            {
                dataList1->dataClstElem[i].reachability_distance=new_r_dist;
                dataList1->dataClstElem[i].iNumRNew=dataTemp->iNum;
                dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
                // printf("enqueing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
                pqueue_enqueue(pq, dataList1->dataClstElem + i);
                //decrease1(dataList1->dataClstElem + i, pq);
                dataList1->dataClstElem[i].addedToPQ=1;
            }
            else
            {
                if(dataList1->dataClstElem[i].reachability_distance > new_r_dist)
                {
                    dataList1->dataClstElem[i].reachability_distance=new_r_dist;
                    dataList1->dataClstElem[i].iNumRNew=dataTemp->iNum;
                    dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
                    // printf("decreasing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
                    decrease1(dataList1->dataClstElem + i, pq);
                    dataList1->dataClstElem[i].addedToPQ=1;
                }
            }
        }
        current=current->next;
    }
    //printf("\n");
    //getchar();

}

//below one is latest modified
void processAffectedPoint(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PQueue* pq)
{
    int iCnt = 0, pts, pts1, i, j,flag=0;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;
    dataTemp1 = (Data)malloc(sizeof(struct data));
    dataTemp2 = (Data)malloc(sizeof(struct data));
    *dataTemp1 = *(dataTemp);
    *dataTemp2 = *(dataTemp);
    //pts = getNeighborHoodUsingBruteForce(dataList1, dataTemp1);
    pts = getNeighborHood(hdrNdTree1, dataTemp1);
    neighborhoodQueryCountInTree1++;
    dataTemp->isProcessed=TRUE;
    dataTemp->neighbors = initNbHdr(0);
    if(dataTemp->timestamp==0)
    {
        for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            //if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
            }
        }
    }
    else
    {
        for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            //if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
                // flag=1;
            }
        }
    }
    pts1 = getNeighborHood2(hdrNdTree2, dataTemp2);
    neighborhoodQueryCountInTree2++;
    //pts1 = getNeighborHoodUsingBruteForce(dataList2, dataTemp2);
    if(dataTemp->timestamp==0)
    {
        for(nb_temp = dataTemp2->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            //if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList2->dataClstElem + nb_temp->n-1, nb_temp->dist);
                // flag=1;
            }
        }
    }
    else
    {
        for(nb_temp = dataTemp2->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            //if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList2->dataClstElem + nb_temp->n-1, nb_temp->dist);
            }
        }
    }
    if(pts+pts1 >= MINPOINTS)
    {
        dataTemp->core_tag = TRUE;
        dataTemp1->core_tag = TRUE;
        dataTemp2->core_tag = TRUE;
    }
    //pq = pqueue_new(&compare_pq, dataList1->uiCnt + dataList2->uiCnt);
    if(dataTemp->core_tag == TRUE)
    {
        // if(flag==1)
            dataTemp->core_distance = find_core_dist(dataTemp);
        dataTemp1->core_distance = dataTemp->core_distance;
        dataTemp2->core_distance = dataTemp->core_distance;
        //AddToOrderList(ReorganizedOrderList, dataTemp);
        UpdateForReorganizingCO( dataList1, dataList2, dataTemp1, dataTemp2,pq);
        ///////////Free data temp neighbors//////////////
        
        dataTemp->neighbors->nbhCnt=dataTemp1->neighbors->nbhCnt + dataTemp2->neighbors->nbhCnt;
        //free(dataTemp->neighbors);
    }
    else
    {
      freeNeighbourhoodList(dataTemp1->neighbors);
      freeNeighbourhoodList(dataTemp2->neighbors);
    }
    freeNeighbourhoodList(dataTemp->neighbors);

}
void UpdateForReorganizingCO(DataHdr dataList1, DataHdr dataList2, Data dataTemp1, Data dataTemp2, PQueue *pq)
{
    double c_dist, new_r_dist;
    NB nbTemp,freetemp;
    Data searchNd;
    c_dist = dataTemp1->core_distance;
    for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL; ) //In dataList1
    {
        searchNd = dataList1->dataClstElem + nbTemp->n -1;
        if(searchNd->fixed==0)
        {
            new_r_dist = (c_dist > nbTemp->dist) ? c_dist : nbTemp->dist;
            if(searchNd->addedToPQ==0)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp1->iNum;
                searchNd->iNumRefNew = dataTemp1->timestamp;
                pqueue_enqueue(pq,searchNd);
                //decrease1(searchNd, pq);
                searchNd->addedToPQ=1;
            }
            else if(new_r_dist <  searchNd->reachability_distance)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp1->iNum;
                searchNd->iNumRefNew = dataTemp1->timestamp;
                decrease1(searchNd, pq);
                searchNd->addedToPQ=1;
            }
        }
        freetemp=nbTemp;
        nbTemp = nbTemp->nbNext ;
        free(freetemp);

    }
    //free(dataTemp1->neighbors);
    for(nbTemp = dataTemp2->neighbors->nbFirst; nbTemp!= NULL;) //In dataList2
    {

        searchNd = dataList2->dataClstElem + nbTemp->n -1;
        if(searchNd->fixed==0)
        {
            new_r_dist = (c_dist > nbTemp->dist) ? c_dist : nbTemp->dist;
            if(searchNd->addedToPQ==0)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp2->iNum;
                searchNd->iNumRefNew = dataTemp2->timestamp;
                pqueue_enqueue(pq,searchNd);
                searchNd->addedToPQ=1;
            }
            else if(new_r_dist <  searchNd->reachability_distance)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp2->iNum;
                searchNd->iNumRefNew = dataTemp2->timestamp;
                decrease1(searchNd, pq);
                searchNd->addedToPQ=1;
            }
        }
        freetemp=nbTemp;
        nbTemp = nbTemp->nbNext ;
        free(freetemp);
    }
    //free(dataTemp2->neighbors);

}
void decrease1(Data searchNd,PQueue *pq)
{
    int tag = 0,i;
    Data tmp;
    for(i=0; i<pq->size; i++)
    {
        if(((Data)pq->data[i])->iNum == searchNd->iNum && ((Data)pq->data[i])->timestamp == searchNd->timestamp )
        {
            tag = 1;
            ((Data)pq->data[i])->reachability_distance = searchNd->reachability_distance;
            ((Data)pq->data[i])->iNumRNew = searchNd->iNumRNew;
            ((Data)pq->data[i])->iNumRefNew = searchNd->iNumRefNew;
            break;
        }
    }


    //AddNodePointHdr(UpdatedList, searchNd);
    if (tag == 0)
    {
        pqueue_enqueue(pq,searchNd);

        //    fprintf(f1, "%d %d %d %d %d %d\n", searchNd->iNum, searchNd->core_distance, searchNd->reachability_distance, searchNd->timestamp, searchNd->iNumR, searchNd->iNumRef);
        // printf( "%d %d %d %d %d %d\n", searchNd->iNum, searchNd->core_distance, searchNd->reachability_distance, searchNd->timestamp, searchNd->iNumR, searchNd->iNumRef);
    }
    else
    {
        pqueue_heapify(pq, i);
        while(i > 0 && pq->cmp(pq->data[i], pq->data[PARENT(i)]) > 0)
        {
            tmp = pq->data[i];
            pq->data[i] = pq->data[PARENT(i)];
            pq->data[PARENT(i)] = tmp;
            i = PARENT(i);
        }
    }
}
void AddToOrderList(PointHdr UpdatedList, int iNum,int timestamp)
{
    OrderedDataNd dtTemp, dt_temp;
    dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
    dtTemp->iNum=iNum;
    dtTemp->timestamp=timestamp;
    dtTemp->dataNextNd = NULL;

    if(UpdatedList->uiCnt == 0) //islistempty check
    {
        UpdatedList->dataFirstNd = dtTemp;
        UpdatedList->dataLastNd = dtTemp;
        UpdatedList->uiCnt++;
    }
    else
    {
        UpdatedList->dataLastNd->dataNextNd = dtTemp;
        UpdatedList->dataLastNd = dtTemp;
        UpdatedList->uiCnt++;

    }

}
// void buildClusterOrdering( char * strFileName,PointHdr orderList, DataHdr initialDataList)
// {
//    if(strFileName == NULL)
//         return;
//     FILE *f1;
//     int timestamp, iNum, iNumR,nbhCnt;
//     double cD, rD;
//     int iCnt = 0;
//     f1 = fopen(strFileName, "r");
//     fscanf(f1," ");
//     while(!feof(f1))
//     {
//         fscanf(f1, "%d %d %lf %lf %d %d\n", &iNum,&timestamp,&cD,&rD,&iNumR,&nbhCnt);
//         AddToOrderList(orderList,iNum,0);
//         //printf("%d %d\n",iNum,timestamp );
//         ///getchar();
//         initialDataList->dataClstElem[iNum-1].globalTimestamp=timestamp;
//         initialDataList->dataClstElem[iNum-1].core_distance=cD;
//         if(cD < UNDEFINED)
//           initialDataList->dataClstElem[iNum-1].core_tag=TRUE;
//         initialDataList->dataClstElem[iNum-1].reachability_distance=rD;
//         initialDataList->dataClstElem[iNum-1].iNumR=iNumR;
//         if(iNumR!=-1)
//         {
//           initialDataList->dataClstElem[iNum-1].iNumRef=0;
//           AddToSuccessorList(initialDataList,initialDataList->dataClstElem+iNum-1);
//         }
//         initialDataList->dataClstElem[iNum-1].neighbors=initNbHdr();
//         initialDataList->dataClstElem[iNum-1].neighbors->nbhCnt=nbhCnt;
//         fscanf(f1," ");
//       //    if(iNum==4835+16063 || iNum==32440+16063 ||iNum==61637+16063 ||iNum==65940+16063||iNum==76142)
//       // printf("reading %d %d %lf %lf %d\n",iNum,timestamp,cD,rD,iNumR);
//     }
//     fclose(f1);
// }
int buildClusterOrdering( char * strFileName,PointHdr orderList, DataHdr initialDataList)
{
   if(strFileName == NULL)
        return;
    FILE *f1;
    int timestamp, iNum, iNumR,nbhCnt;
    double cD, rD;
    int clusterId = 0;
    f1 = fopen(strFileName, "r");
    fscanf(f1," ");
    while(!feof(f1))
    {
        fscanf(f1, "%d %d %lf %lf %d %d\n", &iNum,&timestamp,&cD,&rD,&iNumR,&nbhCnt);
        AddToOrderList(orderList,iNum,0);
        //printf("%d %d\n",iNum,timestamp );
        ///getchar();
        initialDataList->dataClstElem[iNum-1].globalTimestamp=timestamp;
        initialDataList->dataClstElem[iNum-1].core_distance=cD;
        if(cD < UNDEFINED)
          initialDataList->dataClstElem[iNum-1].core_tag=TRUE;
        initialDataList->dataClstElem[iNum-1].reachability_distance=rD;
        initialDataList->dataClstElem[iNum-1].iNumR=iNumR;
        if(iNumR!=-1)
        {
          initialDataList->dataClstElem[iNum-1].iNumRef=0;
          AddToSuccessorList(initialDataList,initialDataList->dataClstElem+iNum-1);
        }
        initialDataList->dataClstElem[iNum-1].neighbors=initNbHdr();
        initialDataList->dataClstElem[iNum-1].neighbors->nbhCnt=nbhCnt;
        if(rD > EPS)
        {
            if(cD <= EPS)
            {
                clusterId+=1;
                initialDataList->dataClstElem[iNum-1].clusterId=clusterId;
            }
            else
            {
                initialDataList->dataClstElem[iNum-1].clusterId=-1;
            }
        }
        else
        {
            initialDataList->dataClstElem[iNum-1].clusterId=clusterId;
        }
        fscanf(f1," ");
          //if(iNum==42368 || iNum==24542)
       //printf("reading %d %d %d %lf %lf %d %d\n",iNum,iNum-16063,timestamp,cD,rD,iNumR,nbhCnt);
    }
    fclose(f1);
    return clusterId;
}
int buildClusterOrderingForOPTICS( char * strFileName,PointHdr orderList, DataHdr initialDataList)
{
   if(strFileName == NULL)
        return;
    FILE *f1;
    int timestamp, iNum, iNumR,nbhCnt;
    double cD, rD;
    int clusterId = 0;
    f1 = fopen(strFileName, "r");
    fscanf(f1," ");
    while(!feof(f1))
    {
        fscanf(f1, "%d %d %lf %lf %d %d\n", &iNum,&timestamp,&cD,&rD,&iNumR,&nbhCnt);
        // AddToOrderList(orderList,iNum,0);
        //printf("%d %d\n",iNum,timestamp );
        ///getchar();
        initialDataList->dataClstElem[iNum-1].globalTimestamp=timestamp;
        // initialDataList->dataClstElem[iNum-1].core_distance=cD;
        // if(cD < UNDEFINED)
        //   initialDataList->dataClstElem[iNum-1].core_tag=TRUE;
        // initialDataList->dataClstElem[iNum-1].reachability_distance=rD;
        // initialDataList->dataClstElem[iNum-1].iNumR=iNumR;
        // if(iNumR!=-1)
        // {
        //   initialDataList->dataClstElem[iNum-1].iNumRef=0;
        //   AddToSuccessorList(initialDataList,initialDataList->dataClstElem+iNum-1);
        // }
        // initialDataList->dataClstElem[iNum-1].neighbors=initNbHdr();
        // initialDataList->dataClstElem[iNum-1].neighbors->nbhCnt=nbhCnt;
        // if(rD > EPS)
        // {
        //     if(cD <= EPS)
        //     {
        //         clusterId+=1;
        //         initialDataList->dataClstElem[iNum-1].clusterId=clusterId;
        //     }
        //     else
        //     {
        //         initialDataList->dataClstElem[iNum-1].clusterId=-1;
        //     }
        // }
        // else
        // {
        //     initialDataList->dataClstElem[iNum-1].clusterId=clusterId;
        // }
        fscanf(f1," ");
          //if(iNum==42368 || iNum==24542)
       //printf("reading %d %d %d %lf %lf %d %d\n",iNum,iNum-16063,timestamp,cD,rD,iNumR,nbhCnt);
    }
    fclose(f1);
    return clusterId;
}
void PrintOrderedListInToFileForAddtion(PointHdr UpdatedList,DataHdr dataList1,DataHdr dataList2, char * strFileName,int noOfObjects )
{
    if(UpdatedList->dataFirstNd == NULL)
    {
        printf("Empty Ordered List\n");
        return;
    }
    int iCnt = 0,iNumR,iNum,globalTimestamp;
    double cD,rD;
    FILE *fp=fopen(strFileName,"w");
    OrderedDataNd dataTemp = UpdatedList->dataFirstNd;
    Data currentDataNd;
    while(dataTemp != NULL)
    {
        if(dataTemp->timestamp==0)
        {
            currentDataNd=dataList1->dataClstElem + dataTemp->iNum -1;
            if(currentDataNd->iNumRefNew==1)
              currentDataNd->iNumRNew=currentDataNd->iNumRNew + noOfObjects;
        }
        else
        {
            currentDataNd=dataList2->dataClstElem + dataTemp->iNum -1;
            if(currentDataNd->iNumRefNew==1)
              currentDataNd->iNumRNew=currentDataNd->iNumRNew + noOfObjects;
            else if(currentDataNd->iNumRefNew==-1 && currentDataNd->iNumRef==1)
              currentDataNd->iNumR=currentDataNd->iNumR + noOfObjects;
            currentDataNd->iNum+=noOfObjects;
        }

        globalTimestamp=currentDataNd->globalTimestamp;
        cD=currentDataNd->core_distance;
        rD=currentDataNd->reachability_distance;
        if(currentDataNd->iNumRefNew!=-1)
          iNumR=currentDataNd->iNumRNew;
        else
          iNumR=currentDataNd->iNumR;
        iNum=currentDataNd->iNum;
        fprintf(fp,"%d %d %.100lf %.100lf %d %d\n",iNum,globalTimestamp,cD,rD,iNumR,currentDataNd->neighbors->nbhCnt);
        dataTemp = dataTemp->dataNextNd;
    }
    fclose(fp);
    return;
}
void writeBothInputFilesInToIntermediateInputFile(DataHdr initialDataList,DataHdr newDataList,char* strFileName)
{
  FILE *fp=fopen(strFileName,"w");
  fprintf(fp, "%d\n%d",initialDataList->uiCnt+newDataList->uiCnt,DIMENSION );
  int i=0,j;
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].toBeDeleted!=TRUE)
    {
      fprintf(fp, "\n" );
      for(j=0;j<DIMENSION;j++)
        fprintf(fp, "%lf ",initialDataList->dataClstElem[i].iData[j] );
    }

  }
  for(i=0;i<newDataList->uiCnt;i++)
  {
    fprintf(fp, "\n" );
    for(j=0;j<DIMENSION;j++)
      fprintf(fp, "%lf ",newDataList->dataClstElem[i].iData[j] );
    
  }
  fclose(fp);
}
void  addPointsToOrderList(int currentTimestamp,char * initialDataListFile,char * initialClusterOrderingFile,char * newDataListFile,char *finalOutputFile,char*finalInputFile)
{
  //FILE * fp=fopen(currentTimestampFile,"r");
  int i,affectedPoints=0,intersectionPoints=0;
  //fscanf(fp,"%d",&currentTimestamp);
  //fclose(fp);
 //printf("inside %s\n",finalInputFile );
  DataHdr initialDataList=readData(initialDataListFile);
  //printf("data list 1 read\n");
  PointHdr initialOrderList=initPointHdr(),newOrderList=initPointHdr(),finalOrderList=initPointHdr();
  buildClusterOrdering(initialClusterOrderingFile,initialOrderList,initialDataList);
  //printf("initial cluster ordering built\n");
  DataHdr newDataList=readData(newDataListFile);
  //buildAppendedFile(initialDataList,newDataList);
  HdrNd newDataTree=buildRTree(newDataList),initialDataTree=buildRTree(initialDataList);
  for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].globalTimestamp=currentTimestamp;
    newDataList->dataClstElem[i].timestamp=1;
  }
  Optics(newDataList,newDataTree,newOrderList );
   for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].isProcessed=FALSE;
  }
  //PrintOrderedListInToFileForAddtion(newOrderList,initialDataList,newDataList,finalOutputFile,initialDataList->uiCnt);
  GetAffectedListUsingOverlapStrategy(initialDataTree,newDataTree,initialDataList,newDataList);
  for(i=0;i<newDataList->uiCnt;i++)
  {
    if(newDataList->dataClstElem[i].affected_tag==TRUE)
      intersectionPoints++;
  }
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].affected_tag==TRUE)
      affectedPoints++;
  }
  //neighborhoodQueryCount=0;
  reorganizingClusterOrdering(initialDataList,newDataList,initialDataTree,newDataTree,initialOrderList,newOrderList,finalOrderList);
  PrintOrderedListInToFileForAddtion(finalOrderList,initialDataList,newDataList,finalOutputFile,initialDataList->uiCnt);
  // currentTimestamp++;
  // fp=fopen(currentTimestampFile,"w");
  // fprintf(fp,"%d",currentTimestamp);
  // fclose(fp);
  //printf("before\n");
  writeBothInputFilesInToIntermediateInputFile(initialDataList,newDataList,finalInputFile);
  //printf("after\n");
  FILE *fp,*fp1,*fp2,*fp3,*fp4;
  char tempString1[500],tempString2[500],tempString3[500],tempString4[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"IntersectionPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString3,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString4,"neighborhoodQueryCountInTree2_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  if(currentTimestamp==1)
  {
    fp=fopen(tempString1,"w");
    fp1=fopen(tempString2,"w");
    fp2=fopen(tempString3,"w");
    fp3=fopen(tempString4,"w");
    fp4=fopen(tempString5,"w");
  }
  else
  {
    fp=fopen(tempString1,"a");
    fp1=fopen(tempString2,"a");
    fp2=fopen(tempString3,"a");
    fp3=fopen(tempString4,"a");
    fp4=fopen(tempString5,"a");
  }
  fprintf(fp,"%d\n",affectedPoints + intersectionPoints );
  fprintf(fp1,"%d\n", intersectionPoints );
  fprintf(fp2,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp3,"%d\n", neighborhoodQueryCountInTree2 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
}
DataHdr mergeDataList(DataHdr initialDataList,DataHdr newDataList)
{
  int count=0;
  int i,j=0;
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].toBeDeleted)  
      count++;
  }

  DataHdr dataList=initDataHdr(initialDataList->uiCnt + newDataList->uiCnt - count);
  //printf("dataList size=%d %d %d %d\n",dataList->uiCnt,initialDataList->uiCnt,newDataList->uiCnt,count );
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(!initialDataList->dataClstElem[i].toBeDeleted)
    {  
      dataList->dataClstElem[j++]=initialDataList->dataClstElem[i];
      dataList->uiCnt++;
    }
  }
  for(i=0;i<newDataList->uiCnt;i++)
  {
    dataList->dataClstElem[j++]=newDataList->dataClstElem[i];
    dataList->uiCnt++;
  }
  for(j=0;j<dataList->uiCnt;j++)
    dataList->dataClstElem[j].iNum=j+1;
  //printDataLst(dataList);
  return dataList;
}
void  runClassicalOpticsForInsertion(int currentTimestamp,char * initialDataListFile,char * initialClusterOrderingFile,char * newDataListFile,char *finalOutputFile,char*finalInputFile)
{
  //FILE * fp=fopen(currentTimestampFile,"r");
  int i,affectedPoints=0,intersectionPoints=0;
  //fscanf(fp,"%d",&currentTimestamp);
  //fclose(fp);
 //printf("inside %s\n",finalInputFile );
  DataHdr initialDataList=readData(initialDataListFile);
  //printf("data list 1 read\n");
  PointHdr initialOrderList=initPointHdr(),newOrderList=initPointHdr(),finalOrderList=initPointHdr();
  int noOfClusters=buildClusterOrderingForOPTICS(initialClusterOrderingFile,initialOrderList,initialDataList);
  // buildClusterOrdering(initialClusterOrderingFile,initialOrderList,initialDataList);
  //printf("initial cluster ordering built\n");
  DataHdr newDataList=readData(newDataListFile);
  for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].globalTimestamp=currentTimestamp;
    newDataList->dataClstElem[i].timestamp=1;
  }
  for(i=0;i<initialDataList->uiCnt;i++)
    initialDataList->dataClstElem[i].timestamp=1;
  DataHdr tempDataList=mergeDataList(initialDataList,newDataList);
  DataHdr tempDataList2=initDataHdr(0);
  //printDataLst(tempDataList);
  //buildAppendedFile(initialDataList,newDataList);
  // newDataList=tempDataList;
  HdrNd newDataTree=buildRTree(tempDataList);
  // HdrNd initialDataTree=buildRTree(initialDataList);
  
  Optics(tempDataList,newDataTree,newOrderList );
  // for(i=0;i<newDataList->uiCnt;i++)
  // {
  //   newDataList->dataClstElem[i].isProcessed=FALSE;
  // }
  // //PrintOrderedListInToFileForAddtion(newOrderList,initialDataList,newDataList,finalOutputFile,initialDataList->uiCnt);
  // GetAffectedListUsingOverlapStrategy(initialDataTree,newDataTree,initialDataList,newDataList);
  // for(i=0;i<newDataList->uiCnt;i++)
  // {
  //   if(newDataList->dataClstElem[i].affected_tag==TRUE)
  //     intersectionPoints++;
  // }
  // for(i=0;i<initialDataList->uiCnt;i++)
  // {
  //   if(initialDataList->dataClstElem[i].affected_tag==TRUE)
  //     affectedPoints++;
  // }
  //neighborhoodQueryCount=0;
  // reorganizingClusterOrdering(initialDataList,newDataList,initialDataTree,newDataTree,initialOrderList,newOrderList,finalOrderList);
  //printf("before\n");
  PrintOrderedListInToFileForAddtion(newOrderList,tempDataList2,tempDataList,finalOutputFile,tempDataList2->uiCnt);
  //printf("after\n");
  // currentTimestamp++;
  // fp=fopen(currentTimestampFile,"w");
  // fprintf(fp,"%d",currentTimestamp);
  // fclose(fp);
  //printf("before\n");
  writeBothInputFilesInToIntermediateInputFile(initialDataList,newDataList,finalInputFile);
  //printf("after\n");
  FILE *fp,*fp1,*fp2,*fp3,*fp4;
  char tempString1[500],tempString2[500],tempString3[500],tempString4[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"IntersectionPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString3,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString4,"neighborhoodQueryCountInTree2_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  if(currentTimestamp==1)
  {
    fp=fopen(tempString1,"w");
    fp1=fopen(tempString2,"w");
    fp2=fopen(tempString3,"w");
    fp3=fopen(tempString4,"w");
    fp4=fopen(tempString5,"w");
  }
  else
  {
    fp=fopen(tempString1,"a");
    fp1=fopen(tempString2,"a");
    fp2=fopen(tempString3,"a");
    fp3=fopen(tempString4,"a");
    fp4=fopen(tempString5,"a");
  }
  fprintf(fp,"%d\n",affectedPoints + intersectionPoints );
  fprintf(fp1,"%d\n", intersectionPoints );
  fprintf(fp2,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp3,"%d\n", neighborhoodQueryCountInTree2 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
}
// int MarkAffectedPointsForDeletion(DataHdr initialDataList,HdrNd initialDataTree, int timestamp)
// {
//   int pts, i, j=1;
//   int count=0,flag=0;
//   Data dataDel,dataTemp;
//   Data tempNd1;
//   Cluster_order ord3;
//   NB nbTemp, nb_next,freetemp;
//   //this is only for debugging purpose
//   // for(i=50000;i<initialDataList->uiCnt;i++)
//   //   initialDataList->dataClstElem[i].toBeDeleted=TRUE;
//   for(i = 0; i < initialDataList->uiCnt; i++)
//   {
//      dataDel=initialDataList->dataClstElem+i;
//      if(dataDel->globalTimestamp==timestamp)
//      {
//        dataDel->toBeDeleted=TRUE;
//        count++;
//        //pts = getNeighborHoodUsingBruteForce(initialDataList, dataDel);
//        pts = getNeighborHood(initialDataTree, dataDel);
//        if(pts > 0)
//        {
//            for(nbTemp = dataDel->neighbors->nbFirst; nbTemp!= NULL;)
//            {           
//               dataTemp=initialDataList->dataClstElem+nbTemp->n -1;
//               if(dataTemp->globalTimestamp!=timestamp)
//               {
//                 dataTemp->neighbors->nbhCnt--;
//                 if(dataTemp->neighbors->nbhCnt >=MINPOINTS)
//                   dataTemp->affected_tag = TRUE;
//                 else
//                 {
//                   dataTemp->affected_tag = FALSE;
//                   dataTemp->core_distance=UNDEFINED;
//                   dataTemp->core_tag=FALSE;
//                   makeReachabilityInvalid(dataTemp,initialDataList,timestamp);
//                 }
//               }
//               freetemp=nbTemp;
//               nbTemp=nbTemp->nbNext;
//               free(freetemp);
//            }
//       }
      
//       free(dataDel->neighbors);
//       makeReachabilityInvalid(dataDel,initialDataList,timestamp);
//     }
//     else
//       initialDataList->dataClstElem[i].iNumNew= j++ ;
//   }
//   return count;
//  // printf("count=%d\n",count );
// }
int MarkAffectedPointsForDeletion(DataHdr initialDataList,HdrNd initialDataTree, int timestamp,int *clusterFlagArray)
{
  int pts, i, j=1;
  int count=0,flag=0;
  Data dataDel,dataTemp;
  Data tempNd1;
  Cluster_order ord3;
  NB nbTemp, nb_next,freetemp;
  //this is only for debugging purpose
  // for(i=50000;i<initialDataList->uiCnt;i++)
  //   initialDataList->dataClstElem[i].toBeDeleted=TRUE;
  for(i = 0; i < initialDataList->uiCnt; i++)
  {
     dataDel=initialDataList->dataClstElem+i;
     if(dataDel->globalTimestamp==timestamp)
     {
       dataDel->toBeDeleted=TRUE;
       count++;
       //pts = getNeighborHoodUsingBruteForce(initialDataList, dataDel);
       pts = getNeighborHood(initialDataTree, dataDel);
       if(pts > 0)
       {
           for(nbTemp = dataDel->neighbors->nbFirst; nbTemp!= NULL;)
           {           
              dataTemp=initialDataList->dataClstElem+nbTemp->n -1;
              if(dataTemp->toBeDeleted==FALSE)
              {
                dataTemp->neighbors->nbhCnt--;
                if(dataTemp->neighbors->nbhCnt >=MINPOINTS)
                  dataTemp->affected_tag = TRUE;
                else
                {
                  dataTemp->affected_tag = FALSE;
                  dataTemp->core_distance=UNDEFINED;
                  dataTemp->core_tag=FALSE;
                  makeReachabilityInvalid(dataTemp,initialDataList,clusterFlagArray);
                }
              }
              freetemp=nbTemp;
              nbTemp=nbTemp->nbNext;
              free(freetemp);
             }
       }
      
        free(dataDel->neighbors);
        makeReachabilityInvalid(dataDel,initialDataList,clusterFlagArray);
     }
     else
      dataDel->iNumNew= j++ ;
  }
  return count;
 // printf("count=%d\n",count );
}
void UpdateForDeletion(Data dataTemp,DataHdr dataList,PQueue* pq)
{
    double c_dist, new_r_dist, temp;
    int i;
    NB nbTemp,freeTemp;
    Data searchNd;
    c_dist = dataTemp->core_distance;
    // if(dataTemp->iNumNew==32768 || dataTemp->iNumNew==61291)
    // {
    //   printf("neighbourhood of %d\n",dataTemp->iNumNew );
    //   getchar();
    //   printNbhLst(dataTemp->neighbors, dataList);
    //   getchar();
    // }
    //printf("processing nbh of %d %lf %lf\n",dataTemp->iNum,dataTemp->iData[0],dataTemp->iData[1]);
    for(nbTemp = dataTemp->neighbors->nbFirst; nbTemp!= NULL;)
    {
        searchNd = dataList->dataClstElem + nbTemp->n -1;
       // printf(" before processing %d %d %lf %lf %lf %lf\n",searchNd->iNum,searchNd->iNumRNew,searchNd->core_distance,searchNd->reachability_distance,searchNd->iData[0],searchNd->iData[1] );
        if(searchNd->fixed == 0)
        {    
            new_r_dist = (c_dist > nbTemp->dist) ? c_dist : nbTemp->dist;                                  
            if(searchNd->addedToPQ == 0)
            {    
               
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp->iNumNew;
                searchNd->iNumRefNew = dataTemp->timestamp;
                pqueue_enqueue(pq,searchNd);
                searchNd->addedToPQ=1;
                // printf("%d %d %lf %lf\n",searchNd->iNum,searchNd->iNumRNew,searchNd->core_distance,searchNd->reachability_distance );
            }
            else
            {
                if(new_r_dist < searchNd->reachability_distance)
                {   
                  searchNd->reachability_distance = new_r_dist;
                  searchNd->iNumRNew = dataTemp->iNumNew;
                  searchNd->iNumRefNew = dataTemp->timestamp;
                  decrease1(searchNd,pq);
                   //printf("%d %d %lf %lf\n",searchNd->iNum,searchNd->iNumRNew,searchNd->core_distance,searchNd->reachability_distance );
                }
            }   
        }
        freeTemp=nbTemp;
        nbTemp = nbTemp->nbNext;
        free(freeTemp);
    }
    //free(dataTemp->neighbors);
    //dataTemp->NeighborhoodComputed=0;
}
void processAffectedPointForDeletion(DataHdr initialDataList,HdrNd DataTree,Data currentDataNd,PQueue *pq, PointHdr finalOrderList)
{
  //int pts=getNeighborHoodUsingBruteForce(initialDataList,currentDataNd);
  int pts=getNeighborHood(DataTree,currentDataNd);
  NB nbTemp,prev, cur;
  Data searchNd;
  double new_r_dist;
  SuccessorNode freeTemp;
  //count0++;
  currentDataNd->isProcessed=TRUE;
  if(pts>=MINPOINTS)
  {
    currentDataNd->core_tag=TRUE;
    currentDataNd->core_distance=find_core_dist(currentDataNd);
    UpdateForDeletion(currentDataNd,initialDataList,pq);
  }
  else
  {

  }
  return;
}
void processAffectedPointForPointWiseDeletion(DataHdr initialDataList,HdrNd DataTree,Data currentDataNd,PQueue *pq, PointHdr finalOrderList)
{
  //int pts=getNeighborHoodUsingBruteForce(initialDataList,currentDataNd);
  int pts=getNeighborHood(DataTree,currentDataNd),i;
  neighborhoodQueryCountInTree1++;
  NB nbTemp,prev, cur;
  Data searchNd;
  double new_r_dist;
  SuccessorNode freeTemp;
  //count0++;
  currentDataNd->isProcessed=TRUE;
  if(pts>=MINPOINTS)
  {
    currentDataNd->core_tag=TRUE;
    currentDataNd->core_distance=find_core_dist(currentDataNd);
    UpdateForDeletion(currentDataNd,initialDataList,pq);
  }
  else
  {
    currentDataNd->core_tag=FALSE;
    currentDataNd->core_distance=UNDEFINED;
    SuccessorNode current=currentDataNd->sListOld->First,freeTemp;
    while(current!=NULL)
    {
      //printf("%d\t",current->iNum );
      i=current->iNum-1;
      if(initialDataList->dataClstElem[i].iNumR==currentDataNd->iNum  && initialDataList->dataClstElem[i].toBeDeleted==FALSE )
      {
        initialDataList->dataClstElem[i].iNumR=-1;
        initialDataList->dataClstElem[i].iNumRef=-1;
        initialDataList->dataClstElem[i].reachability_distance=UNDEFINED;
        //makeEntireClusterReachabilityUndefined(initialDataList->dataClstElem +i, initialDataList);
        //recomputeRdForRdUndefNew(initialDataList->dataClstElem + i,initialDataList,DataTree,finalOrderList);
      }
      freeTemp=current;
      current=current->next;
      free(freeTemp);
    }
    currentDataNd->sListOld->First=NULL;
  }
  return;
}
void processSuccessorsForDeletion(Data dataTemp,DataHdr dataList1,PQueue * pq)
{
  int i;
  double new_r_dist,distance;
  SuccessorNode current=dataTemp->sListOld->First;
  
  while(current!=NULL)
  {
    //printf("%d\t",current->iNum );
    i=current->iNum-1;
    if(dataList1->dataClstElem[i].toBeDeleted==FALSE && dataList1->dataClstElem[i].fixed==0)
    {
      distance=findDist(dataTemp->iData,dataList1->dataClstElem[i].iData);
      new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
      //printf("new reachability_distance for successor %d %d is %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp ,new_r_dist );
      if(dataList1->dataClstElem[i].addedToPQ==0)
      {
        dataList1->dataClstElem[i].reachability_distance=new_r_dist;
        dataList1->dataClstElem[i].iNumRNew=dataTemp->iNumNew;
        dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
       // printf("enqueing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
        pqueue_enqueue(pq, dataList1->dataClstElem + i);
        //decrease1(dataList1->dataClstElem + i, pq);
        dataList1->dataClstElem[i].addedToPQ=1;
      }
      else
      {
        if(dataList1->dataClstElem[i].reachability_distance > new_r_dist)
        {
          dataList1->dataClstElem[i].reachability_distance=new_r_dist;
          dataList1->dataClstElem[i].iNumRNew=dataTemp->iNumNew;
          dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
         // printf("decreasing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
          decrease1(dataList1->dataClstElem + i, pq);
          dataList1->dataClstElem[i].addedToPQ=1;
        }
      }
    }
    current=current->next;
  }
  //printf("\n");
  //getchar();

}
void processOtherPointsForDeletion(Data dataTemp,DataHdr dataList1,HdrNd initialDataTree,PQueue* pq, PointHdr finalOrderList)
{
  NB nb_temp,nb_next;
  double distance, new_r_dist;
  if(dataTemp->toBeDeleted==FALSE)
  {
  if(dataTemp->affected_tag==TRUE && dataTemp->isProcessed==0)
    {
      processAffectedPointForDeletion(dataList1,initialDataTree,dataTemp,pq,finalOrderList);
      // if(dataTemp->core_tag==TRUE)
      //   UpdateForDeletion(dataTemp,dataList1,pq);
      // else
      // {
      //    nb_temp=dataTemp->neighbors->nbFirst;
      //     while(nb_temp!=NULL)
      //     {
      //       nb_next=nb_temp->nbNext;
      //       free(nb_temp);
      //       nb_temp=nb_next;
      //     }
      //     free(dataTemp->neighbors);
      //     dataTemp->NeighborhoodComputed=0;
      // }
    }
    
  else
  {
    if(dataTemp->core_distance!=UNDEFINED )
    {
      processSuccessorsForDeletion(dataTemp,dataList1,pq);
      if( dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
      {
        if(dataList1->dataClstElem[dataTemp->iNumR-1].toBeDeleted==FALSE && dataList1->dataClstElem[dataTemp->iNumR-1].fixed==0)
        {
          distance=findDist(dataTemp->iData,dataList1->dataClstElem[dataTemp->iNumR-1].iData);
          new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
         // printf("new reachability_distance for predecessor %d %d is %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
          if(dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
          {
            dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNumNew;
            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
           // printf("enqueing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
            pqueue_enqueue(pq, dataList1->dataClstElem + dataTemp->iNumR-1);
            //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
            //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
            dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
          }
          else
          {
            if(dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
            {
              dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNumNew;
              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
             // printf("decreasing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
              decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
              //dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
            }
          }
        }
      }
    }
  }
}
}
void processOtherPointsForPointwiseDeletion(Data dataTemp,DataHdr dataList1,HdrNd initialDataTree,PQueue* pq, PointHdr finalOrderList)
{
  NB nb_temp,nb_next;
  double distance, new_r_dist;
  if(dataTemp->toBeDeleted==FALSE)
  {
  if(dataTemp->affected_tag==TRUE && dataTemp->isProcessed==0)
    {
      processAffectedPointForPointWiseDeletion(dataList1,initialDataTree,dataTemp,pq,finalOrderList);
      // if(dataTemp->core_tag==TRUE)
      //   UpdateForDeletion(dataTemp,dataList1,pq);
      // else
      // {
      //    nb_temp=dataTemp->neighbors->nbFirst;
      //     while(nb_temp!=NULL)
      //     {
      //       nb_next=nb_temp->nbNext;
      //       free(nb_temp);
      //       nb_temp=nb_next;
      //     }
      //     free(dataTemp->neighbors);
      //     dataTemp->NeighborhoodComputed=0;
      // }
    }
    
  else
  {
    if(dataTemp->core_distance!=UNDEFINED )
    {
      processSuccessorsForDeletion(dataTemp,dataList1,pq);
      if( dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
      {
        if(dataList1->dataClstElem[dataTemp->iNumR-1].toBeDeleted==FALSE && dataList1->dataClstElem[dataTemp->iNumR-1].fixed==0)
        {
          distance=findDist(dataTemp->iData,dataList1->dataClstElem[dataTemp->iNumR-1].iData);
          new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
         // printf("new reachability_distance for predecessor %d %d is %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
          if(dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
          {
            dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNumNew;
            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
           // printf("enqueing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
            pqueue_enqueue(pq, dataList1->dataClstElem + dataTemp->iNumR-1);
            //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
            //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
            dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
          }
          else
          {
            if(dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
            {
              dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNumNew;
              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
             // printf("decreasing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
              decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
              //dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
            }
          }
        }
      }
    }
  }
}
}
void AddToOrderListForDeletion(PointHdr UpdatedList, int iNum,int timestamp,int oldiNum) 
{
  OrderedDataNd dtTemp, dt_temp;
  dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
  dtTemp->iNum=iNum;
  dtTemp->timestamp=timestamp;
  dtTemp->oldiNum=oldiNum;
  dtTemp->dataNextNd = NULL;
  
  if(UpdatedList->uiCnt == 0) //islistempty check
  {   
      UpdatedList->dataFirstNd = dtTemp;
      UpdatedList->dataLastNd = dtTemp;
      UpdatedList->uiCnt++;
  }
  else
  {       
    UpdatedList->dataLastNd->dataNextNd = dtTemp;
    UpdatedList->dataLastNd = dtTemp;
    UpdatedList->uiCnt++;
    
  }
  
}
void fixForDeletion(DataHdr initialDataList,PointHdr finalOrderList,Data currentDataNd,int i)
{
  // if(currentDataNd->iNum==668)
  //   printf("inside fixFordeletion %d %lf %lf %d %d\n",currentDataNd-> iNum,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumNew );
  if(currentDataNd->core_distance!=UNDEFINED)
  {
    AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,0,currentDataNd->iNum);
    currentDataNd->fixed=1;
    if(currentDataNd->iNumRNew==-1)
    {
      if(currentDataNd->iNumR!=-1 && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==0)
      {
        currentDataNd->reachability_distance=UNDEFINED;
          currentDataNd->iNumR=-1;
          currentDataNd->iNumRef=-1;
         //printf(" %d reachability_distance made undefined %d %d %d\n",i,currentDataNd-> iNum,currentDataNd->iNumR,currentDataNd->iNumNew);
      } 
    }
    #ifdef PRINT
   //printf("fixing1%d %d  %d\n",i,currentDataNd->iNum,currentDataNd->iNumNew);
    #endif
  }
  else if(currentDataNd->reachability_distance!=UNDEFINED)
  {
    if(currentDataNd->iNumRNew!=-1)
    {
      AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,0,currentDataNd->iNum);
      currentDataNd->fixed=1;
      #ifdef PRINT
      //printf("fixing2%d %d  %d\n",i,currentDataNd->iNum,currentDataNd->iNumNew);
      #endif
    }
    else if(initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1)
    {
      AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,0,currentDataNd->iNum);
      currentDataNd->fixed=1;
      #ifdef PRINT
      //printf("fixing3%d %d  %d\n",i,currentDataNd->iNum,currentDataNd->iNumNew);
      #endif
    }
    else
    {
      // currentDataNd->reachability_distance=UNDEFINED;
      // currentDataNd->iNumR=-1;
      // AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,0,currentDataNd->iNum);
      // currentDataNd->fixed=1;
      //printf(" fixing %d %d %d %lf %d %d\n",i,currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->reachability_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].toBeDeleted,initialDataList->dataClstElem[currentDataNd->iNumR-1].affected_tag);
     #ifdef PRINT
      //printf("not fixing %d %d %d %lf %d %d\n",i,currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->reachability_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].toBeDeleted,initialDataList->dataClstElem[currentDataNd->iNumR-1].affected_tag);
      #endif
    }
  }
  // if(currentDataNd->fixed==1)
  // {
  //   if(currentDataNd->iNumNew==4835 ||currentDataNd->iNumNew==32440 ||currentDataNd->iNumNew==61637 ||currentDataNd->iNumNew==65940||currentDataNd->iNumNew==60079||currentDataNd->iNumNew==65940 )
  //     printf("fixing %d %lf %lf %d %d %d %d\n",currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,currentDataNd->affected_tag,currentDataNd->rdUndef );
  // }
}
void makeReachabilityInvalid(Data currentDataNd,DataHdr initialDataList,int *clusterFlagArray)
{
    currentDataNd->core_tag=FALSE;
    currentDataNd->core_distance=UNDEFINED;
    int i;
    SuccessorNode current=currentDataNd->sListOld->First,freeTemp;
  
    while(current!=NULL)
    {
      //printf("%d\t",current->iNum );
      i=current->iNum-1;
      if(initialDataList->dataClstElem[i].iNumR==currentDataNd->iNum  && initialDataList->dataClstElem[i].toBeDeleted==FALSE )
      {
        initialDataList->dataClstElem[i].iNumR=-1;
        initialDataList->dataClstElem[i].iNumRef=-1;
        initialDataList->dataClstElem[i].reachability_distance=UNDEFINED;
        initialDataList->dataClstElem[i].rdUndef=TRUE;
        clusterFlagArray[initialDataList->dataClstElem[i].clusterId-1]=1;
        //makeEntireClusterReachabilityUndefined(initialDataList->dataClstElem +i, initialDataList);
        //recomputeRdForRdUndefNew(initialDataList->dataClstElem + i,initialDataList,DataTree,finalOrderList);
      }
      freeTemp=current;
      current=current->next;
      free(freeTemp);
    }
    currentDataNd->sListOld->First=NULL;
    return;
}
void recomputeRdForRdUndefModified(Data currentDataNd,DataHdr initialDataList,HdrNd DataTree)
{
  getNeighborHood(DataTree,currentDataNd);
  NB nbTemp,freeTemp;
  Data searchNd;
  double new_r_dist;
  int flag=0;
  double tempRd=currentDataNd->reachability_distance;
  currentDataNd->reachability_distance=UNDEFINED;
  for(nbTemp = currentDataNd->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = initialDataList->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(currentDataNd->reachability_distance > new_r_dist)
        {
          currentDataNd->reachability_distance=new_r_dist;
          currentDataNd->iNumRNew=searchNd->iNumNew;
          currentDataNd->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      
       //freeTemp=nbTemp;
      nbTemp = nbTemp->nbNext;
     // free(freeTemp);

  }
  if(flag==0)
    currentDataNd->reachability_distance=tempRd;
}
void recomputeRdForRdUndef(Data currentDataNd,DataHdr initialDataList,HdrNd DataTree)
{
  
  getNeighborHood(DataTree,currentDataNd);
  NB nbTemp,freeTemp;
  Data searchNd;
  double new_r_dist;
  int flag=0;
  double tempRd=currentDataNd->reachability_distance;
  currentDataNd->reachability_distance=UNDEFINED;
  for(nbTemp = currentDataNd->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = initialDataList->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(currentDataNd->reachability_distance > new_r_dist)
        {
          currentDataNd->reachability_distance=new_r_dist;
          currentDataNd->iNumRNew=searchNd->iNumNew;
          currentDataNd->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      
       //freeTemp=nbTemp;
      nbTemp = nbTemp->nbNext;
     // free(freeTemp);

  }
  if(flag==0)
  {
    //currentDataNd->affected_tag=TRUE;
    //currentDataNd->reachability_distance=tempRd;
    for(nbTemp = currentDataNd->neighbors->nbFirst; nbTemp!= NULL;)
    {
        searchNd = initialDataList->dataClstElem + nbTemp->n -1;
        if(searchNd->fixed == 0 && searchNd->core_tag==TRUE  && searchNd->toBeDeleted==FALSE)
        {    
          new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
          if(currentDataNd->reachability_distance > new_r_dist)
          {
            currentDataNd->reachability_distance=new_r_dist;
            currentDataNd->iNumR=searchNd->iNum;
            currentDataNd->iNumRef=searchNd->timestamp;
            AddToSuccessorList(initialDataList,currentDataNd);
            //currentDataNd->rdUndef=FALSE;
            //printf("here 0\n");
            
            flag=1;
          } 
        }
        // freeTemp=nbTemp;
        nbTemp = nbTemp->nbNext;
        // free(freeTemp);

    }
    //printf("no fixed core point found %d\n",currentDataNd->iNum );
  }
  if(flag==0)
    currentDataNd->reachability_distance=tempRd;
  
}
void recomputeRdForRdUndefForCombinedInsertionAndDeletion(Data dataTemp,DataHdr dataList1,HdrNd dataTree1,DataHdr dataList2,HdrNd dataTree2,PQueue* pq, PointHdr finalOrderList)
{
  
  NB nbTemp,freeTemp;
  Data searchNd;
  double new_r_dist;
  int flag=0;
  Data dataTemp1 = (Data)malloc(sizeof(struct data));
  Data dataTemp2 = (Data)malloc(sizeof(struct data));
  *dataTemp1 = *(dataTemp);
  *dataTemp2 = *(dataTemp);
 if(dataTemp->iNum==74325)
    {
      printf("hello\n");
    }
  double tempRd=dataTemp->reachability_distance;
  dataTemp->reachability_distance=UNDEFINED;
  getNeighborHood(dataTree1, dataTemp1);
  neighborhoodQueryCountInTree1++;
  for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList1->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }
  getNeighborHood2(dataTree2, dataTemp2);
  neighborhoodQueryCountInTree2++;
  for(nbTemp = dataTemp2->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList2->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }

  if(flag==0)
  {
    //currentDataNd->affected_tag=TRUE;
    //currentDataNd->reachability_distance=tempRd;
    for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL;)
    {
        searchNd = dataList1->dataClstElem + nbTemp->n -1;
        if(searchNd->fixed == 0 && searchNd->core_tag==TRUE  && searchNd->toBeDeleted==FALSE)
        {    
          new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
          if(dataTemp->reachability_distance > new_r_dist)
          {
            dataTemp->reachability_distance=new_r_dist;
            dataTemp->iNumR=searchNd->iNum;
            dataTemp->iNumRef=searchNd->timestamp;
            AddToSuccessorList(dataList1,dataTemp);
            //currentDataNd->rdUndef=FALSE;
            //printf("here 0\n");
            
            flag=1;
          } 
        }
        // freeTemp=nbTemp;
        nbTemp = nbTemp->nbNext;
        // free(freeTemp);

    }
    //printf("no fixed core point found %d\n",currentDataNd->iNum );
  }
  if(flag==0)
    dataTemp->reachability_distance=tempRd;
  if(dataTemp->iNumRNew!=-1)
  {
    // count1++;
    //printf("hello 1\n");
    if(dataTemp->iNum==74325)
    {
      printf("hello\n");
    }
    insertInBetweenTheOrderListForCombinedInsertionAndDeletion(dataTemp,dataTemp1,dataTemp2,dataList1, dataList2, dataTree1, dataTree2, pq, finalOrderList);
   // printf("hello 2\n");
  }
  //freeNeighbourhoodList(dataTemp2->neighbors);
  //freeNeighbourhoodList(dataTemp1->neighbors);
  
}
void recomputeRdForRdUndefModifiedForCombinedInsertionAndDeletion(Data dataTemp,DataHdr dataList1,HdrNd dataTree1,DataHdr dataList2,HdrNd dataTree2,PQueue* pq, PointHdr finalOrderList)
{
  
  NB nbTemp,freeTemp;
  Data searchNd;
  double new_r_dist;
  int flag=0;
  Data dataTemp1 = (Data)malloc(sizeof(struct data));
  Data dataTemp2 = (Data)malloc(sizeof(struct data));
  *dataTemp1 = *(dataTemp);
  *dataTemp2 = *(dataTemp);
 
  double tempRd=dataTemp->reachability_distance;
  dataTemp->reachability_distance=UNDEFINED;
  getNeighborHood(dataTree1, dataTemp1);
  for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList1->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }
  getNeighborHood(dataTree2, dataTemp2);
  for(nbTemp = dataTemp2->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList2->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }

  // if(flag==0)
  // {
  //   //currentDataNd->affected_tag=TRUE;
  //   //currentDataNd->reachability_distance=tempRd;
  //   for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL;)
  //   {
  //       searchNd = dataList1->dataClstElem + nbTemp->n -1;
  //       if(searchNd->fixed == 0 && searchNd->core_tag==TRUE  && searchNd->toBeDeleted==FALSE)
  //       {    
  //         new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
  //         if(dataTemp->reachability_distance > new_r_dist)
  //         {
  //           dataTemp->reachability_distance=new_r_dist;
  //           dataTemp->iNumR=searchNd->iNum;
  //           dataTemp->iNumRef=searchNd->timestamp;
  //           AddToSuccessorList(dataList1,dataTemp);
  //           //currentDataNd->rdUndef=FALSE;
  //           //printf("here 0\n");
            
  //           flag=1;
  //         } 
  //       }
  //       // freeTemp=nbTemp;
  //       nbTemp = nbTemp->nbNext;
  //       // free(freeTemp);

  //   }
  //   //printf("no fixed core point found %d\n",currentDataNd->iNum );
  // }
  if(flag==0)
    dataTemp->reachability_distance=tempRd;
  if(dataTemp->iNumRNew!=-1)
  {
    // count1++;
    insertInBetweenTheOrderListForCombinedInsertionAndDeletion(dataTemp,dataTemp1,dataTemp2,dataList1, dataList2, dataTree1, dataTree2, pq, finalOrderList);
  }
  //freeNeighbourhoodList(dataTemp2->neighbors);
  //freeNeighbourhoodList(dataTemp1->neighbors);
  }

void insertInBetweenTheOrderListModified(PointHdr finalOrderList,Data currentDataNd,DataHdr dataList, PQueue *pq, HdrNd DataTree)
{ 
    OrderedDataNd current, previous,dtTemp;
    Data dequeued;
    int flag=0;
    dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
    dtTemp->iNum=currentDataNd->iNumNew;
    dtTemp->timestamp=currentDataNd->timestamp;
    dtTemp->oldiNum=currentDataNd->iNum;
    dtTemp->dataNextNd = NULL;
    if(finalOrderList->uiCnt==0)
      return;
    current= finalOrderList->dataFirstNd;
    previous=current;
    // if(flag==0)
    //   printf("hello 1 %d %d %lf %lf %d %d %d %lf\n",finalOrderList->uiCnt,currentDataNd->iNum,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,dataList->dataClstElem[currentDataNd->iNumRNew].fixed,dataList->dataClstElem[currentDataNd->iNumRNew].core_distance );
    while(current!=NULL)
    {
      // if(current->iNum==667 || current->iNum==668)
      //   printf("%d \n",current->iNum,dataList->dataClstElem[current->iNum].fixed,dataList->dataClstElem[current->iNum].core_distance );
      if(current->iNum == currentDataNd->iNumRNew)
      { 
        flag=1;
        previous=current;
        current=current->dataNextNd;
        while(current!=NULL && dataList->dataClstElem[current->oldiNum-1].reachability_distance < currentDataNd->reachability_distance)
        {
          previous=current;
          current=current->dataNextNd;
        }
        previous->dataNextNd=dtTemp;
        dtTemp->dataNextNd=current;
        if(current==NULL)
          finalOrderList->dataLastNd=dtTemp;
        finalOrderList->uiCnt++;
        break;
      }
        current=current->dataNextNd;

    }
    if(flag==0)
    {
      
      printf("hello %d %d %lf %lf %d %d %d %lf\n",finalOrderList->uiCnt,currentDataNd->iNum,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,dataList->dataClstElem[currentDataNd->iNumRNew].fixed,dataList->dataClstElem[currentDataNd->iNumRNew].core_distance );
      getchar();
      return;
    }
    currentDataNd->fixed=1;
    previous=dtTemp;
     // if(currentDataNd->toBeDeleted==FALSE && currentDataNd->isProcessed==0 && currentDataNd->core_distance!=UNDEFINED)
     //   processAffectedPointForDeletion(dataList,DataTree,currentDataNd,pq,finalOrderList);
    processOtherPointsForDeletion(currentDataNd,dataList,DataTree,pq,finalOrderList);
    while(current!=NULL && pq->size > 0)
    {
      currentDataNd=dataList->dataClstElem + current->oldiNum-1;
      if(currentDataNd->reachability_distance > ((Data)pqueue_top(pq))->reachability_distance)
      {
         dequeued=(Data)pqueue_dequeue(pq);
        // if(currentDataNd->toBeDeleted==FALSE && currentDataNd->isProcessed==0 && currentDataNd->core_distance!=UNDEFINED)
        //   processAffectedPointForDeletion(dataList,DataTree,currentDataNd,pq,finalOrderList);
         processOtherPointsForDeletion(currentDataNd,dataList,DataTree,pq,finalOrderList);
         // fixForDeletion(dataList,finalOrderList,dequeued,3);
         dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
         dtTemp->iNum=dequeued->iNumNew;
         dtTemp->timestamp=dequeued->timestamp;
         dtTemp->oldiNum=dequeued->iNum;
         dtTemp->dataNextNd = NULL;
         previous->dataNextNd=dtTemp;
         dtTemp->dataNextNd=current;
         if(current==NULL)
          finalOrderList->dataLastNd=dtTemp;
         finalOrderList->uiCnt++;
         previous=dtTemp;
         dequeued->fixed=1;
      }
      else
      {
        previous=current;
        current=current->dataNextNd;
      }
    }
    while(pq->size>0)
    {
      
      dequeued=(Data)pqueue_dequeue(pq);
      // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      processOtherPointsForDeletion(dequeued,dataList,DataTree,pq,finalOrderList);
     fixForDeletion(dataList,finalOrderList,dequeued,5);
    }
    
    // if(currentDataNd->iNumR!=-1 && dataList->dataClstElem[currentDataNd->iNumR-1].iNumNew==currentDataNd->iNumRNew)
    //   printf("%d %lf %lf\n",currentDataNd->iNum, currentDataNd->core_distance,currentDataNd->reachability_distance );
    return;
}
void processAffectedPointForDeletionModified(DataHdr initialDataList,HdrNd DataTree,Data currentDataNd,PQueue *pq,PointHdr finalOrderList)
{
  getNeighborHood(DataTree,currentDataNd);
  NB nbTemp,freeTemp;
  Data searchNd;
  double new_r_dist;
  int flag=0;
  double tempRd=currentDataNd->reachability_distance;
  currentDataNd->reachability_distance=UNDEFINED;
  for(nbTemp = currentDataNd->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = initialDataList->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(currentDataNd->reachability_distance > new_r_dist)
        {
          currentDataNd->reachability_distance=new_r_dist;
          currentDataNd->iNumRNew=searchNd->iNumNew;
          currentDataNd->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      
       //freeTemp=nbTemp;
      nbTemp = nbTemp->nbNext;
      //free(freeTemp);

  }
  if(flag==0)
  {
    //currentDataNd->affected_tag=TRUE;
    currentDataNd->reachability_distance=tempRd;
    //printf("no fixed core point found %d\n",currentDataNd->iNum );
  }
  if(currentDataNd->iNumRNew!=-1)
  {
    insertInBetweenTheOrderListModified(finalOrderList,currentDataNd,initialDataList,pq,DataTree);
  }
  else
  {
    
      if(currentDataNd->neighbors->nbhCnt>=MINPOINTS)
      {
        currentDataNd->core_tag=TRUE;
        currentDataNd->core_distance=find_core_dist(currentDataNd);
        UpdateForDeletion(currentDataNd,initialDataList,pq);
      }
  }
  currentDataNd->isProcessed=TRUE;
}
void reorganizeForDeletion(DataHdr initialDataList,HdrNd DataTree,PointHdr initialOrderList,PointHdr finalOrderList)
{
  OrderedDataNd current=initialOrderList->dataFirstNd;
  Data currentDataNd,dequeued;
  NB nb_temp,nb_next;
  int i;
  PQueue *pq = pqueue_new(&compare_pq, initialDataList->uiCnt);
  while(current!=NULL)
  {
    currentDataNd=initialDataList->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE)
    {
      if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0 && currentDataNd->rdUndef==FALSE)
      {
        
        processAffectedPointForDeletion(initialDataList,DataTree,currentDataNd,pq,finalOrderList);
      }
    }
    current=current->dataNextNd;
    if(pq->size>0)
    {
      //printf("initial %d\n",currentDataNd->iNumNew );
      fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
      break;
    }

  }
  while(current!=NULL)
  {
    currentDataNd=initialDataList->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
    {
      // if(currentDataNd->rdUndef==TRUE)
        // recomputeRdForRdUndef(currentDataNd,initialDataList,DataTree);
      if(pq->size>0)
      {
        // if(((Data)pqueue_top(pq))->affected_tag==TRUE &&  ((Data)pqueue_top(pq))->NeighborhoodComputed==0)
        //   computeNeighborhood_CoreDistanceAndReachabilityDist(((Data)pqueue_top(pq)),initialDataList,DataTree);
        if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
        {
          printf("inumR fixed found %d %d %d %lf %d %lf %d\n",currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->iNumR,currentDataNd->reachability_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].affected_tag,initialDataList->dataClstElem[currentDataNd->iNumR-1].core_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].rdUndef);
          processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
          fixForDeletion(initialDataList,finalOrderList,currentDataNd,2);
          current=current->dataNextNd;
        }
        else
        {
          dequeued=(Data)pqueue_dequeue(pq);
           processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
           fixForDeletion(initialDataList,finalOrderList,dequeued,3); 
        }
      }
      else
      {
        while(current!=NULL)
        {
          currentDataNd=initialDataList->dataClstElem+current->iNum-1;
          //if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0)
          if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
          {
            if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0 && currentDataNd->rdUndef==FALSE)
            //if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
            {
              processAffectedPointForDeletionModified(initialDataList,DataTree,currentDataNd,pq,finalOrderList);
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
            }
          }
          current=current->dataNextNd;
          if(pq->size>0 && currentDataNd->fixed==0)
          {
            //printf("between %d\n",currentDataNd->iNumNew );
            fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
            break;
          }

        }
        // processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq);
        // fixForDeletion(initialDataList,finalOrderList,currentDataNd,4);
        // current=current->dataNextNd;
      }
    }
    else
    {
      current=current->dataNextNd;
    }

  }
  while(pq->size>0)
  {
    
    dequeued=(Data)pqueue_dequeue(pq);
    // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
    //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
    processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
   fixForDeletion(initialDataList,finalOrderList,dequeued,5);
  }
  int countBefore=0,countAfter=0;
 current = initialOrderList->dataFirstNd;
 //printf("hello 1\n");

 int count2=0;
 current=initialOrderList->dataFirstNd;
 while(current!=NULL)
 {
  
    if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE && initialDataList->dataClstElem[current->iNum-1].rdUndef==FALSE )
        {
            
            currentDataNd=initialDataList->dataClstElem+current->iNum-1;
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
              //currentDataNd->fixed=1;
            //if(currentDataNd->iNumR==-1 || initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
            //{
              
              //processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
              //if(pq->size >0)
                fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
             //while(pq->size>0)
              //{
                
              //  dequeued=(Data)pqueue_dequeue(pq);
                // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
                //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
               // processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
              // fixForDeletion(initialDataList,finalOrderList,dequeued,5);
              //}
            //}
            
            
        }
        current=current->dataNextNd;
  }
      current=initialOrderList->dataFirstNd;
      count1=0;
        while(current!=NULL)
        {
            if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
            {
              currentDataNd = initialDataList->dataClstElem + current->iNum-1;
              if(currentDataNd->rdUndef==TRUE)
              {
              recomputeRdForRdUndef(currentDataNd,initialDataList,DataTree);
              if(currentDataNd->iNumRNew!=-1)
              {
                count1++;
                insertInBetweenTheOrderListModified(finalOrderList,currentDataNd,initialDataList,pq,DataTree);
              }
            }
            }
            
            current=current->dataNextNd;
        }
        current=initialOrderList->dataFirstNd;
      count1=0;
        while(current!=NULL)
        {
            if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
            {
              currentDataNd = initialDataList->dataClstElem + current->iNum-1;
              if(currentDataNd->rdUndef!=TRUE && currentDataNd->core_distance!=UNDEFINED)
              {
              recomputeRdForRdUndefModified(currentDataNd,initialDataList,DataTree);
              if(currentDataNd->iNumRNew!=-1)
              {
                //countBefore=finalOrderList->uiCnt;
                count1++;
                //printf("hk\n");
                // insertInBetweenTheOrderList(finalOrderList,currentDataNd,initialDataList);
                insertInBetweenTheOrderListModified(finalOrderList,currentDataNd,initialDataList,pq,DataTree);
                // countAfter=finalOrderList->uiCnt;
                // if(countAfter- countBefore !=1)
                // {
                //   printf("1 finalOrderList size=%d datalist size=%d %d %d\n",finalOrderList->uiCnt,initialDataList->uiCnt,countBefore,countAfter );
                //   getchar();
                // }
              }
            }
            }
            
            current=current->dataNextNd;
        }
       current = initialOrderList->dataFirstNd;
      while(current!=NULL)
      {
         if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE)
          {
            currentDataNd=initialDataList->dataClstElem+current->iNum-1;
            {
            processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
              fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
           while(pq->size>0)
            {
              
              dequeued=(Data)pqueue_dequeue(pq);
              // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
              //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
              processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
             fixForDeletion(initialDataList,finalOrderList,dequeued,5);
            }
            //count2=1;
            
          }
        }
              // initialDataList->dataClstElem[current->iNum-1].fixed=1;
              // initialDataList->dataClstElem[current->iNum-1].reachability_distance=UNDEFINED;
              // initialDataList->dataClstElem[current->iNum-1].iNumR=-1;
          current=current->dataNextNd;
      }
    current = initialOrderList->dataFirstNd;
    while(current!=NULL)
    {
       if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE)
        {
          currentDataNd=initialDataList->dataClstElem+current->iNum-1;
           AddToOrderListForDeletion(finalOrderList,initialDataList->dataClstElem[current->iNum-1].iNumNew,0,initialDataList->dataClstElem[current->iNum-1].iNum);
      //       if(currentDataNd->iNumNew==4835 ||currentDataNd->iNumNew==32440 ||currentDataNd->iNumNew==61637 ||currentDataNd->iNumNew==65940||currentDataNd->iNumNew==60079||currentDataNd->iNumNew==65940 )
      // printf("fixing %d %lf %lf %d %d %d %d\n",currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,currentDataNd->affected_tag,currentDataNd->rdUndef );

        }
        current=current->dataNextNd;
      }
      //printf("hello 5\n");
}
void reorganizeForPointwiseDeletion(DataHdr initialDataList,HdrNd DataTree,PointHdr initialOrderList,PointHdr finalOrderList)
{
  OrderedDataNd current=initialOrderList->dataFirstNd;
  Data currentDataNd,dequeued;
  NB nb_temp,nb_next;
  int i;
  PQueue *pq = pqueue_new(&compare_pq, initialDataList->uiCnt);
  while(current!=NULL)
  {
    currentDataNd=initialDataList->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE)
    {
      if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
      {
        
        processAffectedPointForPointWiseDeletion(initialDataList,DataTree,currentDataNd,pq,finalOrderList);
      }
    }
    current=current->dataNextNd;
    if(pq->size>0)
    {
      //printf("initial %d\n",currentDataNd->iNumNew );
      fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
      break;
    }

  }
  while(current!=NULL)
  {
    currentDataNd=initialDataList->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
    {
      // if(currentDataNd->rdUndef==TRUE)
        // recomputeRdForRdUndef(currentDataNd,initialDataList,DataTree);
      if(pq->size>0)
      {
        // if(((Data)pqueue_top(pq))->affected_tag==TRUE &&  ((Data)pqueue_top(pq))->NeighborhoodComputed==0)
        //   computeNeighborhood_CoreDistanceAndReachabilityDist(((Data)pqueue_top(pq)),initialDataList,DataTree);
        if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
        {
          printf("inumR fixed found %d %d %d %lf %d %lf %d\n",currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->iNumR,currentDataNd->reachability_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].affected_tag,initialDataList->dataClstElem[currentDataNd->iNumR-1].core_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].rdUndef);
          processOtherPointsForPointwiseDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
          fixForDeletion(initialDataList,finalOrderList,currentDataNd,2);
          current=current->dataNextNd;
        }
        else
        {
          dequeued=(Data)pqueue_dequeue(pq);
           processOtherPointsForPointwiseDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
           fixForDeletion(initialDataList,finalOrderList,dequeued,3); 
        }
      }
      else
      {
        while(current!=NULL)
        {
          currentDataNd=initialDataList->dataClstElem+current->iNum-1;
          //if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0)
          if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
          {
            if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0 )
            //if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
            {
              processAffectedPointForPointWiseDeletion(initialDataList,DataTree,currentDataNd,pq,finalOrderList);
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
            }
          }
          current=current->dataNextNd;
          if(pq->size>0 && currentDataNd->fixed==0)
          {
            //printf("between %d\n",currentDataNd->iNumNew );
            fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
            break;
          }

        }
        // processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq);
        // fixForDeletion(initialDataList,finalOrderList,currentDataNd,4);
        // current=current->dataNextNd;
      }
    }
    else
    {
      current=current->dataNextNd;
    }

  }
  while(pq->size>0)
  {
    
    dequeued=(Data)pqueue_dequeue(pq);
    // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
    //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
    processOtherPointsForPointwiseDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
   fixForDeletion(initialDataList,finalOrderList,dequeued,5);
  }
  int countBefore=0,countAfter=0;
 current = initialOrderList->dataFirstNd;
 //printf("hello 1\n");

 int count2=0;
 current=initialOrderList->dataFirstNd;
 while(current!=NULL)
 {
  
    if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
        {
            
            currentDataNd=initialDataList->dataClstElem+current->iNum-1;
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
              //currentDataNd->fixed=1;
            //if(currentDataNd->iNumR==-1 || initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
            //{
              
              //processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
              //if(pq->size >0)
                fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
             //while(pq->size>0)
              //{
                
              //  dequeued=(Data)pqueue_dequeue(pq);
                // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
                //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
               // processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
              // fixForDeletion(initialDataList,finalOrderList,dequeued,5);
              //}
            //}
            
            
        }
        current=current->dataNextNd;
  }
      // current=initialOrderList->dataFirstNd;
      // count1=0;
        // while(current!=NULL)
        // {
        //     if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
        //     {
        //       currentDataNd = initialDataList->dataClstElem + current->iNum-1;
        //       if(currentDataNd->rdUndef==TRUE)
        //       {
        //       recomputeRdForRdUndef(currentDataNd,initialDataList,DataTree);
        //       if(currentDataNd->iNumRNew!=-1)
        //       {
        //         count1++;
        //         insertInBetweenTheOrderListModified(finalOrderList,currentDataNd,initialDataList,pq,DataTree);
        //       }
        //     }
        //     }
            
        //     current=current->dataNextNd;
        // }
      //   current=initialOrderList->dataFirstNd;
      // count1=0;
      //   while(current!=NULL)
      //   {
      //       if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
      //       {
      //         currentDataNd = initialDataList->dataClstElem + current->iNum-1;
      //         if(currentDataNd->rdUndef!=TRUE && currentDataNd->core_distance!=UNDEFINED)
      //         {
      //         recomputeRdForRdUndefModified(currentDataNd,initialDataList,DataTree);
      //         if(currentDataNd->iNumRNew!=-1)
      //         {
      //           //countBefore=finalOrderList->uiCnt;
      //           count1++;
      //           //printf("hk\n");
      //           // insertInBetweenTheOrderList(finalOrderList,currentDataNd,initialDataList);
      //           insertInBetweenTheOrderListModified(finalOrderList,currentDataNd,initialDataList,pq,DataTree);
      //           // countAfter=finalOrderList->uiCnt;
      //           // if(countAfter- countBefore !=1)
      //           // {
      //           //   printf("1 finalOrderList size=%d datalist size=%d %d %d\n",finalOrderList->uiCnt,initialDataList->uiCnt,countBefore,countAfter );
      //           //   getchar();
      //           // }
      //         }
      //       }
      //       }
            
      //       current=current->dataNextNd;
      //   }
      //  current = initialOrderList->dataFirstNd;
      // while(current!=NULL)
      // {
      //    if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE)
      //     {
      //       currentDataNd=initialDataList->dataClstElem+current->iNum-1;
      //       {
      //       processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq,finalOrderList);
      //         fixForDeletion(initialDataList,finalOrderList,currentDataNd,30);
      //      while(pq->size>0)
      //       {
              
      //         dequeued=(Data)pqueue_dequeue(pq);
      //         // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //         //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      //         processOtherPointsForDeletion(dequeued,initialDataList,DataTree,pq,finalOrderList);
      //        fixForDeletion(initialDataList,finalOrderList,dequeued,5);
      //       }
      //       //count2=1;
            
      //     }
      //   }
      //         // initialDataList->dataClstElem[current->iNum-1].fixed=1;
      //         // initialDataList->dataClstElem[current->iNum-1].reachability_distance=UNDEFINED;
      //         // initialDataList->dataClstElem[current->iNum-1].iNumR=-1;
      //     current=current->dataNextNd;
      // }
    current = initialOrderList->dataFirstNd;
    while(current!=NULL)
    {
       if(initialDataList->dataClstElem[current->iNum-1].fixed==0 && initialDataList->dataClstElem[current->iNum-1].toBeDeleted==FALSE)
        {
          currentDataNd=initialDataList->dataClstElem+current->iNum-1;
           AddToOrderListForDeletion(finalOrderList,initialDataList->dataClstElem[current->iNum-1].iNumNew,0,initialDataList->dataClstElem[current->iNum-1].iNum);
      //       if(currentDataNd->iNumNew==4835 ||currentDataNd->iNumNew==32440 ||currentDataNd->iNumNew==61637 ||currentDataNd->iNumNew==65940||currentDataNd->iNumNew==60079||currentDataNd->iNumNew==65940 )
      // printf("fixing %d %lf %lf %d %d %d %d\n",currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,currentDataNd->affected_tag,currentDataNd->rdUndef );

        }
        current=current->dataNextNd;
      }
      //printf("hello 5\n");
}
HdrNd buildRTreeForDeletion(DataHdr dataHdrLst)
{   
  // build RTree with the list of elements passed
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
    if(dataHdrLst->dataClstElem[i].toBeDeleted==FALSE)
    {
      insertTree(hdrNdTree, initExtNd((dataHdrLst->dataClstElem)+i));
      if(hdrNdTree->uiCnt > 1)
        hdrNdTree = createRoot(hdrNdTree);
    }
  }
  return  hdrNdTree;
}

void PrintReorganizedListForDeletion(PointHdr finalOrderList,char *outputFile,DataHdr dataList1 )
{
 FILE *fp=fopen(outputFile,"w");
 OrderedDataNd dataTemp=finalOrderList->dataFirstNd;
 Data currentDataNd;
 int iNumR;
  while(dataTemp != NULL)
    {  
          currentDataNd=dataList1->dataClstElem + dataTemp->oldiNum-1;
          if(currentDataNd->iNumRNew!=-1)
            iNumR=currentDataNd->iNumRNew;
          else if(currentDataNd->iNumR!=-1)
            iNumR=dataList1->dataClstElem[currentDataNd->iNumR-1].iNumNew;
          else
            iNumR=-1;
          fprintf(fp,"%d %d %.100lf %.100lf %d %d\n",currentDataNd->iNumNew,currentDataNd->globalTimestamp, currentDataNd->core_distance,  currentDataNd->reachability_distance,iNumR,currentDataNd->neighbors->nbhCnt);

        //printf("%d %d %d\n", dataTemp->dataClstElem->iNum, dataTemp->dataClstElem->core_distance, dataTemp->dataClstElem->reachability_distance);
        
        dataTemp = dataTemp->dataNextNd;
    }
    fclose(fp);
    return;
}
void PrintReorganizedListForPointWiseDeletion(PointHdr finalOrderList,char *outputFile,DataHdr dataList1 )
{
 FILE *fp=fopen(outputFile,"w");
 OrderedDataNd dataTemp=finalOrderList->dataFirstNd;
 Data currentDataNd;
 int iNumR;
  while(dataTemp != NULL)
    {  
          currentDataNd=dataList1->dataClstElem + dataTemp->iNum-1;
          if(currentDataNd->core_distance < UNDEFINED && currentDataNd->neighbors->nbhCnt < MINPOINTS)
          {
            printf("error %d %d %.100lf %.100lf %d %d\n",dataTemp->iNum,currentDataNd->globalTimestamp, currentDataNd->core_distance,  currentDataNd->
              reachability_distance,currentDataNd->iNumR,currentDataNd->neighbors->nbhCnt);
            getchar();
          }
          fprintf(fp,"%d %d %.100lf %.100lf %d %d\n",dataTemp->iNum,currentDataNd->globalTimestamp, currentDataNd->core_distance,  currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->neighbors->nbhCnt);

        //printf("%d %d %d\n", dataTemp->dataClstElem->iNum, dataTemp->dataClstElem->core_distance, dataTemp->dataClstElem->reachability_distance);
        
        dataTemp = dataTemp->dataNextNd;
    }
    fclose(fp);
    return;
}
void writeInputFilesInToIntermediateInputFileWithoutDeletedPoints(DataHdr dataList,int count)
{
  FILE *fp=fopen("IntermediateInputFile.txt","w");
  fprintf(fp, "%d\n%d",dataList->uiCnt-count,DIMENSION );
  int i=0,j;
  for(i=0;i<dataList->uiCnt;i++)
  {
    if(dataList->dataClstElem[i].toBeDeleted==FALSE)
    {
      fprintf(fp, "\n" );
      for(j=0;j<DIMENSION;j++)
        fprintf(fp, "%lf ",dataList->dataClstElem[i].iData[j] );
    }

  }
  fclose(fp);
}
void writeInputFilesInToIntermediateInputFileWithoutDeletedPointsForPointWiseIncremental(DataHdr dataList,char *strFileName)
{
  FILE *fp=fopen(strFileName,"w");
  fprintf(fp, "%d\n%d",dataList->uiCnt,DIMENSION );
  int i=0,j;
  for(i=0;i<dataList->uiCnt;i++)
  {
    
      fprintf(fp, "\n" );
      for(j=0;j<DIMENSION;j++)
        fprintf(fp, "%lf ",dataList->dataClstElem[i].iData[j] );
    

  }
  fclose(fp);
}
void deletePointsFromOrderList(char * initialFile,char *initialOrderListFile ,int timestampTobeDeleted,char *outputFile) 
{
  int i, j=1,deletedCount,affectedPoints=0;
  DataHdr initialDataList=readData(initialFile);
  #ifdef PRINT
  printf("cnt1=%d\n",initialDataList->uiCnt);
  #endif
  PointHdr initialOrderList=initPointHdr(),finalOrderList=initPointHdr();
  #ifdef PRINT
  printf("buildRTree COMPLETE\n");
  #endif
  HdrNd initialDataTree=buildRTree(initialDataList);
  //printf("deletion start\n");
  int noOfClusters=buildClusterOrdering(initialOrderListFile,initialOrderList,initialDataList);
  int *clusterFlagArray=(int*)malloc(sizeof(int)*noOfClusters);
  assert(clusterFlagArray!=NULL);
  for(i=0;i<noOfClusters;i++)
    clusterFlagArray[i]=0;
  deletedCount= MarkAffectedPointsForDeletion(initialDataList,initialDataTree,timestampTobeDeleted,clusterFlagArray);
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].affected_tag==TRUE)
      affectedPoints++;
  }
  FILE *fp=fopen("AffectedPointsCount.txt","a");
  fprintf(fp,"affected points during deletion of partition %d are %d\n",timestampTobeDeleted,affectedPoints );
  fclose(fp);
  //printf("delete=%d\n",deletedCount );
  #ifdef PRINT
  printf("optics start\n");
  #endif
  //Optics(initialDataList,initialDataTree,initialOrderList);
  #ifdef PRINT
  printf("optics finish\n");
  #endif
  //printf("optics finish\n");
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].toBeDeleted==TRUE)
    {
      deleteRecord(NULL,initialDataTree,initialDataList->dataClstElem+i);
    }
  }

  //initialDataTree=buildRTreeForDeletion(initialDataList);
  HdrNd newDataTree=initialDataTree;
  // for(i=0;i<initialDataList->uiCnt;i++)
  // {
  //   if(initialDataList->dataClstElem[i].toBeDeleted==FALSE)
  //     initialDataList->dataClstElem[i].iNumNew= j++ ;
  // }
  reorganizeForDeletion(initialDataList,newDataTree,initialOrderList,finalOrderList);
  PrintReorganizedListForDeletion(finalOrderList,outputFile,initialDataList);
  writeInputFilesInToIntermediateInputFileWithoutDeletedPoints(initialDataList,deletedCount);
  //printf("count0=%d count1=%d\n",count0,count1 );

}
void fixForPointWiseInsertion(DataHdr initialDataList,PointHdr finalOrderList,Data currentDataNd,int i)
{

  if(currentDataNd->fixed==0)
  {
    if(currentDataNd->core_distance!=UNDEFINED)
    {
      AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
      currentDataNd->fixed=1;
      if(currentDataNd->iNumRNew==-1)
      {
        if(currentDataNd->iNumR!=-1 && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==0)
        {
          currentDataNd->reachability_distance=UNDEFINED;
            currentDataNd->iNumR=-1;
        } 
      }
      #ifdef PRINT
     //printf("fixing1%d %d  %d\n",i,currentDataNd->iNum);
      #endif
    }
    else if(currentDataNd->reachability_distance!=UNDEFINED)
    {
      if(currentDataNd->iNumRNew!=-1)
      {
        AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing2%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else if(initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1)
      {
        AddToOrderList(finalOrderList,currentDataNd->iNum,currentDataNd->timestamp);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing3%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else
      {
        // currentDataNd->reachability_distance=UNDEFINED;
        // currentDataNd->iNumR=-1;
       #ifdef PRINT
        //printf("not fixing %d %d %d %lf %d\n",i,currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->reachability_distance,initialDataList->dataClstElem[currentDataNd->iNumR-1].affected_tag);
        #endif
      }
    }
  }
}
void UpdateForPointWiseInsertion(DataHdr dataList1,Data dataTemp1,PQueue* pq)
{
  double c_dist, new_r_dist;
    NB nbTemp,freetemp;
    Data searchNd;
    c_dist = dataTemp1->core_distance;
    for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL; ) //In dataList1
    {
        searchNd = dataList1->dataClstElem + nbTemp->n -1;
        if(searchNd->fixed==0)
        {
            new_r_dist = (c_dist > nbTemp->dist) ? c_dist : nbTemp->dist;
            if(searchNd->addedToPQ==0)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp1->iNum;
                searchNd->iNumRefNew = dataTemp1->timestamp;
                pqueue_enqueue(pq,searchNd);
                //decrease1(searchNd, pq);
                searchNd->addedToPQ=1;
            }
            else if(new_r_dist <  searchNd->reachability_distance)
            {
                searchNd->reachability_distance = new_r_dist;
                searchNd->iNumRNew = dataTemp1->iNum;
                searchNd->iNumRefNew = dataTemp1->timestamp;
                decrease1(searchNd, pq);
                searchNd->addedToPQ=1;
            }
        }
        freetemp=nbTemp;
        nbTemp = nbTemp->nbNext ;
        free(freetemp);

    }
    //free(dataTemp1->neighbors);
}
// void processAffectedPointForPointWiseInsertion(Data dataTemp,DataHdr dataList1,HdrNd hdrNdTree1,PQueue* pq)
// {
//   int iCnt = 0, pts, pts1, i, j,flag=0;
//     Data dequeued, dataTemp1, dataTemp2;
//     NB nbTemp,nb_temp, nb_next;
//     double distance,new_r_dist;
//     Data searchNd;
//     dataTemp1 = (Data)malloc(sizeof(struct data));
//     *dataTemp1 = *(dataTemp);
//     pts = getNeighborHood(hdrNdTree1, dataTemp1);
//     neighborhoodQueryCountInTree1++;
//     dataTemp->isProcessed=TRUE;
//     dataTemp->neighbors = initNbHdr(0);
//     pts1=0;
//     for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
//     {
//         if(nb_temp->dist <=  dataTemp->core_distance)
//         {
//             pts1++;
//             insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
//             flag=1;
//         }
//     }
//     // if(dataTemp->iNum==107 && dataList1->uiCnt > 275)
//     // {
//     //   printf("inside affected processing dataList1->uiCnt=%d dataTemp->iNum=%d %lf %d %d pts=%d pts1=%d %d %lf %lf %d %d\n",dataList1->uiCnt,dataTemp->iNum,dataTemp->core_distance,dataTemp->iNumR,dataTemp->iNumRNew,pts, pts1,dataList1->dataClstElem[635].iNum,dataList1->dataClstElem[635].core_distance,dataList1->dataClstElem[635].reachability_distance,dataList1->dataClstElem[635].iNumR,dataList1->dataClstElem[635].iNumRNew );
//     // }
//     if(pts >= MINPOINTS)
//     {
//         dataTemp->core_tag = TRUE;
//         dataTemp1->core_tag=TRUE;
//     }
//     if(dataTemp->core_tag == TRUE)
//     {
//         //printf("before find_core_dist flag=%d pts1=%d %d %lf\n",flag,pts1,pts,dataTemp->core_distance );
//         if(flag==1)
//             dataTemp->core_distance = find_core_dist(dataTemp);
//           //printf("after find_core_dist %lf\n",dataTemp->core_distance );
//         dataTemp1->core_distance = dataTemp->core_distance;
//         UpdateForPointWiseInsertion( dataList1, dataTemp1,pq);
        
//     }
//     else
//     {
//       freeNeighbourhoodList(dataTemp1->neighbors);
//       //free(dataTemp1->neighbors);

//      }
//     freeNeighbourhoodList(dataTemp->neighbors);
//     free(dataTemp->neighbors);
//     dataTemp->neighbors=dataTemp1->neighbors;
//     free(dataTemp1);
//     //printf("core_distance=%lf pq_size=%d\n",dataTemp->core_distance,pq->size );
// }
void processAffectedPointForPointWiseInsertion(Data dataTemp,DataHdr dataList1,HdrNd hdrNdTree1,PQueue* pq)
{
  int iCnt = 0, pts, pts1, i, j,flag=0;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;
    // dataTemp1 = (Data)malloc(sizeof(struct data));
    // *dataTemp1 = *(dataTemp);
    pts = getNeighborHood(hdrNdTree1, dataTemp);
    neighborhoodQueryCountInTree1++;
    dataTemp->isProcessed=TRUE;
    //dataTemp->neighbors = initNbHdr(0);
    // pts1=0;
    // for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
    // {
    //     if(nb_temp->dist <=  dataTemp->core_distance)
    //     {
    //         pts1++;
    //         insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
    //         flag=1;
    //     }
    // }
    // if(dataTemp->iNum==107 && dataList1->uiCnt > 275)
    // {
    //   printf("inside affected processing dataList1->uiCnt=%d dataTemp->iNum=%d %lf %d %d pts=%d pts1=%d %d %lf %lf %d %d\n",dataList1->uiCnt,dataTemp->iNum,dataTemp->core_distance,dataTemp->iNumR,dataTemp->iNumRNew,pts, pts1,dataList1->dataClstElem[635].iNum,dataList1->dataClstElem[635].core_distance,dataList1->dataClstElem[635].reachability_distance,dataList1->dataClstElem[635].iNumR,dataList1->dataClstElem[635].iNumRNew );
    // }
    if(pts >= MINPOINTS)
    {
        dataTemp->core_tag = TRUE;
          dataTemp->core_distance = find_core_dist(dataTemp);
        UpdateForPointWiseInsertion( dataList1, dataTemp,pq);
        
    }
    else
    {
      freeNeighbourhoodList(dataTemp->neighbors);

     }
    //free(dataTemp->neighbors);
    //dataTemp->neighbors=dataTemp1->neighbors;
    //free(dataTemp1);
    //printf("core_distance=%lf pq_size=%d\n",dataTemp->core_distance,pq->size );
}
void processFirstAffectedPointForPointWiseInsertion(Data dataTemp,DataHdr dataList1,HdrNd hdrNdTree1,PQueue* pq)
{
  int iCnt = 0, pts, pts1, i, j,flag=0;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;
    dataTemp1 = (Data)malloc(sizeof(struct data));
    *dataTemp1 = *(dataTemp);
    //pts = getNeighborHood(hdrNdTree1, dataTemp1);
    pts = dataTemp1->neighbors->nbhCnt;
    dataTemp->isProcessed=TRUE;
    dataTemp->neighbors = initNbHdr(0);
    pts1=0;
    for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
    {
        if(nb_temp->dist <=  dataTemp->core_distance)
        {
            pts1++;
            insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
            flag=1;
        }
    }
    // if(dataTemp->iNum==107 && dataList1->uiCnt > 275)
    // {
    //   printf("inside affected processing dataList1->uiCnt=%d dataTemp->iNum=%d %lf %d %d pts=%d pts1=%d %d %lf %lf %d %d\n",dataList1->uiCnt,dataTemp->iNum,dataTemp->core_distance,dataTemp->iNumR,dataTemp->iNumRNew,pts, pts1,dataList1->dataClstElem[635].iNum,dataList1->dataClstElem[635].core_distance,dataList1->dataClstElem[635].reachability_distance,dataList1->dataClstElem[635].iNumR,dataList1->dataClstElem[635].iNumRNew );
    // }
    if(pts >= MINPOINTS)
    {
        dataTemp->core_tag = TRUE;
        dataTemp1->core_tag=TRUE;
    }
    if(dataTemp->core_tag == TRUE)
    {
        //printf("before find_core_dist %lf\n",dataTemp->core_distance );
        if(flag==1)
            dataTemp->core_distance = find_core_dist(dataTemp);
        dataTemp1->core_distance = dataTemp->core_distance;
        UpdateForPointWiseInsertion( dataList1, dataTemp1,pq);
        
    }
    else
    {
      freeNeighbourhoodList(dataTemp1->neighbors);
      //free(dataTemp1->neighbors);

     }
    freeNeighbourhoodList(dataTemp->neighbors);
    free(dataTemp->neighbors);
    dataTemp->neighbors=dataTemp1->neighbors;
    free(dataTemp1);
    //printf("core_distance=%lf pq_size=%d\n",dataTemp->core_distance,pq->size );
}
void ProcessOtherFixedPointsOfReorganizedListForPointWiseInsertion(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PointHdr OrderList1,PointHdr OrderList2, PQueue* pq)
{
    int iCnt = 0, pts, pts1, i, j;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;

    if(dataTemp->affected_tag==TRUE && dataTemp ->isProcessed==FALSE)
    {
        //compute neighbourhood of this point
        processAffectedPointForPointWiseInsertion(dataTemp,dataList1,hdrNdTree1, pq);
    }
    else
    {
        //      printf("processing %d %d\n",dataTemp->iNum,dataTemp->timestamp );
        if(dataTemp->timestamp==0 && dataTemp->core_distance!=UNDEFINED)
        {
            //predecessor of moving objects
            if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
            {
                if(dataList1->dataClstElem[dataTemp->iNumR-1].fixed==0)
                {
                    distance=findDist(dataTemp->iData,dataList1->dataClstElem[dataTemp->iNumR-1].iData);
                    new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                    // printf("new reachability_distance for predecessor %d %d is %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                    if(dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                    {
                        dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                        dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                        dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                        // printf("enqueing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                        pqueue_enqueue(pq, dataList1->dataClstElem + dataTemp->iNumR-1);
                        //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                        //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                        dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                    }
                    else
                    {
                        if(dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                        {
                            dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                            dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                            // printf("decreasing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                            decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                            dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                        }
                    }
                }
            }
            //successor of moving objects
            processSuccessors(dataTemp,dataList1,pq);
        }
        else if(dataTemp->core_distance!=UNDEFINED)
        {
            //predecessor of moving objects
            if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
            {
                if(dataList2->dataClstElem[dataTemp->iNumR-1].fixed==0)
                {
                    distance=findDist(dataTemp->iData,dataList2->dataClstElem[dataTemp->iNumR-1].iData);
                    new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                    // printf("new reachability_distance for predecessor %d %d is %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                    if(dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                    {
                        dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                        dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                        dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                        //printf("enqueing %d %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                        pqueue_enqueue(pq, dataList2->dataClstElem + dataTemp->iNumR-1);
                        //decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                        dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                    }
                    else
                    {
                        if(dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                        {
                            dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                            dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                            dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                            //printf("decreasing %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                            decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                            dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                        }
                    }
                }
                

            }
            processSuccessors(dataTemp,dataList2,pq);
        }
    }
}
void pointWiseInsertion(int currentTimestamp,char * initialDataListFile,char * initialClusterOrderingFile,char * newDataListFile,char *finalOutputFile,char*finalInputFile)
{
  int i,pts,count,j;
  DataHdr dataList2=readData(newDataListFile);
  DataHdr dataList1=readDataModified(initialDataListFile,dataList2->uiCnt);
  PointHdr OrderList1=initPointHdr(),finalOrderList;
  OrderedDataNd current;
  NB nbTemp,freetemp;
  SuccessorNode currentSNode,temp;
  Data currentDataNd,dequeued;
  buildClusterOrdering(initialClusterOrderingFile,OrderList1,dataList1);
  HdrNd dataTree1=buildRTree(dataList1);
  neighborhoodQueryCountInTree1=0;
  //printf("rtree build\n");
  int affectedCount=0;
  PQueue *pq = pqueue_new(&compare_pq, dataList1->uiCnt+ dataList2->uiCnt);
  for(i=0;i < dataList2->uiCnt;i++)
  {
    // if(dataList1->uiCnt > 822)
    // printf("core distance=%lf %d\n",dataList1->dataClstElem[822].core_distance,dataList1->dataClstElem[822].neighbors->nbhCnt );
    finalOrderList=initPointHdr();
    insertDataLstNd(dataList1 , dataList2->dataClstElem +i);
    dataList1->dataClstElem[dataList1->uiCnt-1].iNum=dataList1->uiCnt;
    dataList1->dataClstElem[dataList1->uiCnt-1].timestamp=0;
    dataList1->dataClstElem[dataList1->uiCnt-1].globalTimestamp=currentTimestamp;
    dataList1->dataClstElem[dataList1->uiCnt-1].affected_tag=TRUE;
    insertTree(dataTree1, initExtNd(dataList1->dataClstElem + dataList1->uiCnt-1));
    if(dataTree1->uiCnt > 1)
      dataTree1 = createRoot(dataTree1);
    //dataTree1=buildRTree(dataList1);
    pts = getNeighborHood(dataTree1, dataList1->dataClstElem + dataList1->uiCnt-1);
    neighborhoodQueryCountInTree1++;
    count=0;
    if(pts > 0)
    {
        for(nbTemp = dataList1->dataClstElem[dataList1->uiCnt-1].neighbors->nbFirst; nbTemp!= NULL;)
        {
            dataList1->dataClstElem[nbTemp->n -1].affected_tag = TRUE;
            affectedCount++;
            //freetemp=nbTemp;
            nbTemp=nbTemp->nbNext;
            //free(freetemp);
        }
    }
    //printf("count=%d\n",count );
    //free(dataList1->dataClstElem[dataList1->uiCnt-1].neighbors);
    processFirstAffectedPointForPointWiseInsertion(dataList1->dataClstElem + dataList1->uiCnt-1,dataList1, dataTree1,pq);
    if(pq->size>0)
    {
        fixForAddition(dataList1,NULL,finalOrderList,dataList1->dataClstElem + dataList1->uiCnt-1,0, dataTree1, NULL);
    }
    current= OrderList1->dataFirstNd;
    while(current!=NULL)
    {
      currentDataNd=dataList1->dataClstElem+current->iNum-1;
      if(currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
      {
        if(pq->size>0)
        {
          if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && dataList1->dataClstElem[currentDataNd->iNumR-1].fixed==1)
          {
            printf("inumr found fixed\n");
            ProcessOtherFixedPointsOfReorganizedListForPointWiseInsertion(currentDataNd, dataList1,NULL,dataTree1,NULL,OrderList1,NULL, pq);
            fixForAddition( dataList1, NULL,finalOrderList, currentDataNd,2,dataTree1,NULL);
            current=current->dataNextNd;
          }
          else
          {
            dequeued=(Data)pqueue_dequeue(pq);
            ProcessOtherFixedPointsOfReorganizedListForPointWiseInsertion(dequeued, dataList1, NULL,dataTree1,NULL,OrderList1,NULL, pq);
             fixForAddition( dataList1, NULL,finalOrderList, dequeued,3,dataTree1,NULL);
          }
        }
        else
        {
          while(current!=NULL)
          {
              currentDataNd=dataList1->dataClstElem+current->iNum-1;
              if(currentDataNd->affected_tag==TRUE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0 && currentDataNd->isProcessed==0)
              {
                //printf("1 pq_size=%d currentDataNd->core_distance=%lf\n",pq->size,currentDataNd->core_distance );
                processAffectedPointForPointWiseInsertion(currentDataNd,dataList1, dataTree1,pq);
               // printf("2 pq_size=%d currentDataNd->core_distance=%lf\n",pq->size,currentDataNd->core_distance );
              }
              if(pq->size>0)
              {
                  //printf("3 pq_size=%d currentDataNd->core_distance=%lf\n",pq->size,currentDataNd->core_distance );
                  fixForAddition( dataList1, NULL,finalOrderList, currentDataNd,4,dataTree1,NULL);
                  current=current->dataNextNd;
                  break;
              }
              current=current->dataNextNd;              
          }
        }
      }
      else
      {
        current=current->dataNextNd;
      }

    }
    while(pq->size>0)
    {
      
      dequeued=(Data)pqueue_dequeue(pq);
     // checkPQ(pq,dequeued,7);
      // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      ProcessOtherFixedPointsOfReorganizedListForPointWiseInsertion(dequeued, dataList1, NULL,dataTree1,NULL,OrderList1,NULL, pq);
      fixForAddition( dataList1, NULL,finalOrderList, dequeued,5,dataTree1,NULL);
    }
    current = OrderList1->dataFirstNd;
    while(current!=NULL)
    {
        if(dataList1->dataClstElem[current->iNum-1].fixed==0 )
        {

            AddToOrderList(finalOrderList,current->iNum,current->timestamp);
            //AddToNewSuccessorList(dataList1,dataList2,dataList2->dataClstElem + current->iNum-1);
            dataList1->dataClstElem[current->iNum-1].fixed=1;
            //countIndependent++;
            //printf("fixing rest of 2nd list %d %d %lf %lf\n", dataList2->dataClstElem[current->iNum-1].iNum, dataList2->dataClstElem[current->iNum-1].timestamp, dataList2->dataClstElem[current->iNum-1].core_distance, dataList2->dataClstElem[current->iNum-1].reachability_distance);
        }
        current=current->dataNextNd;
    }
    if(dataList1->dataClstElem[dataList1->uiCnt-1].fixed==0)
      AddToOrderList(finalOrderList,dataList1->uiCnt,dataList1->dataClstElem[dataList1->uiCnt-1].timestamp);
    for(j=0;j<dataList1->uiCnt;j++)
    {
      freeSuccessorList(dataList1->dataClstElem + j);
      dataList1->dataClstElem[j].sListOld= initSuccessorList();
    }
      
    for(j=0;j<dataList1->uiCnt;j++)
    {
      dataList1->dataClstElem[j].fixed=0;
      dataList1->dataClstElem[j].addedToPQ=0;
      dataList1->dataClstElem[j].affected_tag=FALSE;
      if(dataList1->dataClstElem[j].iNumRNew!=-1)
      {
        dataList1->dataClstElem[j].iNumR=dataList1->dataClstElem[j].iNumRNew;
        dataList1->dataClstElem[j].iNumRef=0;
      }
      dataList1->dataClstElem[j].iNumRNew=-1;
      dataList1->dataClstElem[j].iNumRefNew=-1;
      dataList1->dataClstElem[j].isProcessed=FALSE;
      if(dataList1->dataClstElem[j].iNumR!=-1)
        AddToSuccessorList(dataList1,dataList1->dataClstElem + j);
    }
    freePointHdr(OrderList1);
    OrderList1=finalOrderList;
  }
  PrintOrderedListInToFileForAddtion(finalOrderList,dataList1,NULL,finalOutputFile,0 );
  dataList2=initDataHdr(0);
  writeBothInputFilesInToIntermediateInputFile(dataList1,dataList2,finalInputFile);
  FILE *fp,*fp1,*fp4;
  char tempString1[500],tempString2[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  //printf("%s %s\n", tempString1,tempString2);
  if(currentTimestamp==1)
  {
    fp=fopen(tempString1,"w");
    fp1=fopen(tempString2,"w");
    fp4=fopen(tempString5,"w");
  }
  else
  {
    fp=fopen(tempString1,"a");
    fp1=fopen(tempString2,"a");
    fp4=fopen(tempString5,"a");
  }
  fprintf(fp,"%d\n",affectedCount );
  fprintf(fp1,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp4);
}

void pointWiseDeletion(char * initialFile,char *initialOrderListFile ,int timestampTobeDeleted,char *outputFile,char*finalInputFile)
{
  DataHdr dataList1=readData(initialFile);
  PointHdr OrderList1= initPointHdr(),finalOrderList;
  buildClusterOrdering(initialOrderListFile,OrderList1,dataList1);
  //printf(" buildClusterOrdering complete %s\n",outputFile );
  HdrNd dataTree1=buildRTree(dataList1);
  //printf("rtree cokmplete\n");
  Data currentDataNd,dataTemp;
  int i,pts,deletedCount=0,j,affectedCount=0;
  NB nbTemp,freetemp;
  SuccessorNode current,temp;
  neighborhoodQueryCountInTree1=0;
  //printf("delete %d\n",timestampTobeDeleted );
  for(i=0;i<dataList1->uiCnt;i++)
  {
    if(dataList1->dataClstElem[i].globalTimestamp==timestampTobeDeleted)
      deletedCount++;
  }
  for(i=0;i<deletedCount;i++)
  {
      // if( i%500==0)
      //   printf("delete i=%d\n",i );
      count0=0;
      count1=0;
      finalOrderList=initPointHdr();
      currentDataNd=dataList1->dataClstElem ;
      currentDataNd->toBeDeleted=TRUE;
      pts = getNeighborHood(dataTree1 , currentDataNd);
      neighborhoodQueryCountInTree1++;
      if(pts > 0)
      {
        for(nbTemp = currentDataNd->neighbors->nbFirst; nbTemp!= NULL;)
        {           
          dataTemp=dataList1->dataClstElem+nbTemp->n -1;
          dataTemp->affected_tag = TRUE;
          affectedCount++;
          freetemp=nbTemp;
          nbTemp=nbTemp->nbNext;
          free(freetemp);
        }
      }
      free(currentDataNd->neighbors);
      current= currentDataNd->sListOld->First;
      while(current!=NULL)
      {
        j=current->iNum;
        //if(dataList1->dataClstElem[j-1].toBeDeleted==FALSE)
        {
          //printf("successor %d\n",j );
          dataList1->dataClstElem[j-1].iNumR=-1;
          dataList1->dataClstElem[j-1].iNumRef=-1;
          dataList1->dataClstElem[j-1].reachability_distance=UNDEFINED;
          //dataList1->dataClstElem[j-1].rdUndef=TRUE;
          //makeEntireClusterReachabilityUndefined(dataList1->dataClstElem +j-1,dataList1);
          //recomputeRdForRdUndefNew(dataList1->dataClstElem + j-1,dataList1,dataTree1,finalOrderList);
        }
        temp=current;
        current=current->next;
        free(temp);
      }
      for(j=1;j<dataList1->uiCnt;j++)
      {
        dataList1->dataClstElem[j].iNumNew= dataList1->dataClstElem[j].iNum -1 ;
      }
      // printf("before delete from rtree\n");
      // printTree(dataTree1);
      deleteRecord(NULL,dataTree1,currentDataNd);
      //dataTree1=buildRTreeForDeletion(dataList1);
       //printf("after delete from rtree\n");
      // printTree(dataTree1);
      // getchar();
      // if(i==26 )
      //   {
      //     currentDataNd=dataList1->dataClstElem + 335;
      //     printf(" %d %d %d %.100lf %.100lf %d %d\n",i,currentDataNd->iNum,currentDataNd->globalTimestamp, currentDataNd->core_distance,  currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->neighbors->nbhCnt);
      //   }
      reorganizeForPointwiseDeletion(dataList1,dataTree1,OrderList1,finalOrderList);
      //printf("reorganizing deletion complete\n");
      for(j=1;j<dataList1->uiCnt;j++)
      {
        freeSuccessorList(dataList1->dataClstElem + j);
        dataList1->dataClstElem[j].sListOld= initSuccessorList();
      }
      dataList1->dataClstElem += 1;
      dataList1->uiCnt--;
      for(j=0;j<dataList1->uiCnt;j++)
      {
        dataList1->dataClstElem[j].fixed=0;
        dataList1->dataClstElem[j].addedToPQ=0;
        dataList1->dataClstElem[j].affected_tag=FALSE;
        if(dataList1->dataClstElem[j].iNumRNew!=-1)
          dataList1->dataClstElem[j].iNumR=dataList1->dataClstElem[j].iNumRNew;
        else if(dataList1->dataClstElem[j].iNumR!=-1)
          dataList1->dataClstElem[j].iNumR =  dataList1->dataClstElem[j].iNumR-1;
        dataList1->dataClstElem[j].iNumRNew=-1;
        dataList1->dataClstElem[j].iNumRefNew=-1;
        dataList1->dataClstElem[j].iNum--;
        dataList1->dataClstElem[j].isProcessed=FALSE;
        if(dataList1->dataClstElem[j].iNumR!=-1)
          AddToSuccessorList(dataList1,dataList1->dataClstElem + j);
        dataList1->dataClstElem[j].rdUndef=FALSE;
        currentDataNd=dataList1->dataClstElem + j;
        // if(currentDataNd->core_distance < UNDEFINED && currentDataNd->neighbors->nbhCnt < MINPOINTS)
        // {
        //   printf("error %d %d %d %.100lf %.100lf %d %d\n",i,currentDataNd->iNum,currentDataNd->globalTimestamp, currentDataNd->core_distance,  currentDataNd->
        //     reachability_distance,currentDataNd->iNumR,currentDataNd->neighbors->nbhCnt);
        //   getchar();
        // }
      }
   
      freePointHdr(OrderList1);
      OrderList1=finalOrderList;
      //printf("count0=%d count1=%d\n",count0,count1 );
      //printf("OrderList1 count=%d datalist count=%d\n",OrderList1->uiCnt,dataList1->uiCnt );
      // PrintOrderedListInToFile(OrderList1,dataList1,NULL,"d.txt",0);
      // getchar();
      //printf("at the end of iteration %d\n",i+1 );
  }
  //printf("deletion complete\n");
  PrintReorganizedListForPointWiseDeletion(OrderList1,outputFile,dataList1);
  writeInputFilesInToIntermediateInputFileWithoutDeletedPointsForPointWiseIncremental(dataList1,finalInputFile);
  FILE *fp,*fp1,*fp4;
  char tempString1[500],tempString2[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  //printf("%s %s\n", tempString1,tempString2);
  fp=fopen(tempString1,"a");
  fp1=fopen(tempString2,"a");
  fp4=fopen(tempString5,"a");
  fprintf(fp,"%d\n",affectedCount );
  fprintf(fp1,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp4);
 //printf("count0=%d count1=%d\n",count0,count1 );
}
void freeNeighbourhoodList(NbHdr neighbors)
{
   NB nb_temp=neighbors->nbFirst,nb_next;
    while(nb_temp!=NULL)
    {
      nb_next=nb_temp->nbNext;
      free(nb_temp);
      nb_temp=nb_next;
    }
    return;
}
void fixForCombinedInsertionAndDeletion(DataHdr dataList1,DataHdr dataList2,PointHdr finalOrderList,Data currentDataNd,int i)
{
  DataHdr initialDataList=(currentDataNd->timestamp==0)? dataList1:dataList2;
  if(currentDataNd->fixed==0)
  {
    if(currentDataNd->core_distance!=UNDEFINED)
    {
      AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,currentDataNd->timestamp,currentDataNd->iNum);
      currentDataNd->fixed=1;
      if(currentDataNd->iNumRNew==-1)
      {
        if(currentDataNd->iNumR!=-1 && initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==0)
        {
          currentDataNd->reachability_distance=UNDEFINED;
          currentDataNd->iNumR=-1;
          currentDataNd->iNumRef=-1;
        } 
      }
      #ifdef PRINT
     //printf("fixing1%d %d  %d\n",i,currentDataNd->iNum);
      #endif
    }
    else if(currentDataNd->reachability_distance!=UNDEFINED)
    {
      if(currentDataNd->iNumRNew!=-1)
      {
        AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,currentDataNd->timestamp,currentDataNd->iNum);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing2%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else if(initialDataList->dataClstElem[currentDataNd->iNumR-1].fixed==1)
      {
        AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,currentDataNd->timestamp,currentDataNd->iNum);
        currentDataNd->fixed=1;
        #ifdef PRINT
        //printf("fixing3%d %d  %d\n",i,currentDataNd->iNum);
        #endif
      }
      else
      {
        //printf("error in fixing..reachability_distance undefined but iNumR not fixed %d %d %d\n",currentDataNd->iNum, currentDataNd->iNumR, i );
      }
    }
  }
}
void processAffectedPointNeighbourhoods(Data dataTemp,Data dataTemp1,Data dataTemp2,PQueue* pq,DataHdr dataList1, DataHdr dataList2)
{
  int iCnt = 0, pts=dataTemp1->neighbors->nbhCnt, pts1=dataTemp2->neighbors->nbhCnt, i, j,flag=0;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    dataTemp->isProcessed=TRUE;
    dataTemp->neighbors = initNbHdr(0);
    if(dataTemp->timestamp==0)
    {
        for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
            }
        }
    }
    else
    {
        for(nb_temp = dataTemp1->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList1->dataClstElem + nb_temp->n-1, nb_temp->dist);
                flag=1;
            }
        }
    }
    //pts1 = getNeighborHood(hdrNdTree2, dataTemp2);
    //pts1 = getNeighborHoodUsingBruteForce(dataList2, dataTemp2);
    if(dataTemp->timestamp==0)
    {
        for(nb_temp = dataTemp2->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList2->dataClstElem + nb_temp->n-1, nb_temp->dist);
                flag=1;
            }
        }
    }
    else
    {
        for(nb_temp = dataTemp2->neighbors->nbFirst; nb_temp!= NULL; nb_temp = nb_temp->nbNext)
        {
            if(nb_temp->dist <=  dataTemp->core_distance)
            {
                insertNeighbors(dataTemp, dataList2->dataClstElem + nb_temp->n-1, nb_temp->dist);
            }
        }
    }
    if(pts+pts1 >= MINPOINTS)
    {
        dataTemp->core_tag = TRUE;
        dataTemp1->core_tag = TRUE;
        dataTemp2->core_tag = TRUE;
    }
    //pq = pqueue_new(&compare_pq, dataList1->uiCnt + dataList2->uiCnt);
    if(dataTemp->core_tag == TRUE)
    {
        if(flag==1)
            dataTemp->core_distance = find_core_dist(dataTemp);
        dataTemp1->core_distance = dataTemp->core_distance;
        dataTemp2->core_distance = dataTemp->core_distance;
        //AddToOrderList(ReorganizedOrderList, dataTemp);
        UpdateForReorganizingCO( dataList1, dataList2, dataTemp1, dataTemp2,pq);
        ///////////Free data temp neighbors//////////////
        
        dataTemp->neighbors->nbhCnt=dataTemp1->neighbors->nbhCnt + dataTemp2->neighbors->nbhCnt;
        //free(dataTemp->neighbors);
    }
    else
    {
      freeNeighbourhoodList(dataTemp1->neighbors);
      freeNeighbourhoodList(dataTemp2->neighbors);
    }
    freeNeighbourhoodList(dataTemp->neighbors);
}
void insertInBetweenTheOrderListForCombinedInsertionAndDeletion(Data currentDataNd,Data dataTemp1,Data dataTemp2,DataHdr dataList1,DataHdr dataList2,HdrNd dataTree1,HdrNd dataTree2,PQueue* pq,PointHdr finalOrderList)
{ 
   // printf("hello 8\n");
    OrderedDataNd current, previous,dtTemp;
    Data dequeued;
    DataHdr dataList;
    int flag=0;
    dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
    dtTemp->iNum=currentDataNd->iNumNew;
    dtTemp->timestamp=currentDataNd->timestamp;
    dtTemp->oldiNum=currentDataNd->iNum;
    dtTemp->dataNextNd = NULL;
   // printf("hello 7\n");
    if(finalOrderList->uiCnt==0)
      return;
    current= finalOrderList->dataFirstNd;
    previous=current;
    count4++;
   // printf("hello 3\n");
    // if(flag==0)
    //   printf("hello 1 %d %d %lf %lf %d %d %d %lf\n",finalOrderList->uiCnt,currentDataNd->iNum,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,dataList->dataClstElem[currentDataNd->iNumRNew].fixed,dataList->dataClstElem[currentDataNd->iNumRNew].core_distance );
    while(current!=NULL)
    {
      if(current->oldiNum == currentDataNd->iNumRNew && current->timestamp == currentDataNd->iNumRefNew)
      { 
        flag=1;
        previous=current;
        current=current->dataNextNd;
        dataList=(current->timestamp==0) ? dataList1 : dataList2;
        while(current!=NULL && dataList->dataClstElem[current->oldiNum-1].reachability_distance < currentDataNd->reachability_distance)
        {
          previous=current;
          current=current->dataNextNd;
          dataList=(current->timestamp==0) ? dataList1 : dataList2;
        }
        previous->dataNextNd=dtTemp;
        dtTemp->dataNextNd=current;
        if(current==NULL)
          finalOrderList->dataLastNd=dtTemp;
        finalOrderList->uiCnt++;
        break;
      }
        current=current->dataNextNd;

    }
    if(flag==0)
    {
      
      printf("not fixed above %d %d %lf %lf %d %d %d %lf\n",finalOrderList->uiCnt,currentDataNd->iNum,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRNew,dataList->dataClstElem[currentDataNd->iNumRNew].fixed,dataList->dataClstElem[currentDataNd->iNumRNew].core_distance );
      getchar();
      return;
    }
    //printf("hello 4\n");
    currentDataNd->fixed=1;
    previous=dtTemp;
    if(currentDataNd->affected_tag==TRUE)
      processAffectedPointNeighbourhoods(currentDataNd,dataTemp1,dataTemp2,pq, dataList1, dataList2);
    else
      ProcessOtherFixedPointsForCombinedInsertionAndDeletion(currentDataNd, dataList1, dataList2,dataTree1,dataTree2, pq);
   // printf("hello 5\n");
    while(current!=NULL && pq->size > 0)
    {
      dataList=(current->timestamp==0) ? dataList1 : dataList2;
      currentDataNd=dataList->dataClstElem + current->oldiNum-1;
      if(currentDataNd->reachability_distance > ((Data)pqueue_top(pq))->reachability_distance)
      {
         dequeued=(Data)pqueue_dequeue(pq);
        ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
         dtTemp = (OrderedDataNd)malloc(sizeof(struct orderedDataNd));
         dtTemp->iNum=dequeued->iNumNew;
         dtTemp->timestamp=dequeued->timestamp;
         dtTemp->oldiNum=dequeued->iNum;
         dtTemp->dataNextNd = NULL;
         previous->dataNextNd=dtTemp;
         dtTemp->dataNextNd=current;
         if(current==NULL)
          finalOrderList->dataLastNd=dtTemp;
         finalOrderList->uiCnt++;
         previous=dtTemp;
         dequeued->fixed=1;
      }
      else
      {
        previous=current;
        current=current->dataNextNd;
      }
    }
    //printf("hello 6\n");
    while(pq->size>0)
    {
      
      dequeued=(Data)pqueue_dequeue(pq);
      // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
      //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
      ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
     fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,0);
    }
   // printf("hello 7\n");
    
    // if(currentDataNd->iNumR!=-1 && dataList->dataClstElem[currentDataNd->iNumR-1].iNumNew==currentDataNd->iNumRNew)
    //   printf("%d %lf %lf\n",currentDataNd->iNum, currentDataNd->core_distance,currentDataNd->reachability_distance );
    return;
}
void processAffectedPointModified(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2,PQueue* pq,PointHdr finalOrderList)
{
  int iCnt = 0, pts, pts1, i, j,flag=0;
  Data dequeued, dataTemp1, dataTemp2,searchNd;
  NB nbTemp,nb_temp, nb_next;
  double distance,new_r_dist;
  dataTemp1 = (Data)malloc(sizeof(struct data));
  dataTemp2 = (Data)malloc(sizeof(struct data));
  *dataTemp1 = *(dataTemp);
  *dataTemp2 = *(dataTemp);
  count3++;
  pts = getNeighborHood(hdrNdTree1, dataTemp1);
 // printf("points from dataTree1=%d\n",pts );
  double tempRd=dataTemp->reachability_distance;
  dataTemp->reachability_distance=UNDEFINED;
  for(nbTemp = dataTemp1->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList1->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }
  pts1 = getNeighborHood(hdrNdTree2, dataTemp2);
  //printf("points from dataTree2=%d\n",pts1 );
  for(nbTemp = dataTemp2->neighbors->nbFirst; nbTemp!= NULL;)
  {
      searchNd = dataList2->dataClstElem + nbTemp->n -1;
      if(searchNd->fixed == 1 && searchNd->core_tag==TRUE)
      {    
        new_r_dist = (searchNd->core_distance > nbTemp->dist) ? searchNd->core_distance : nbTemp->dist;
        if(dataTemp->reachability_distance > new_r_dist)
        {
          dataTemp->reachability_distance=new_r_dist;
          dataTemp->iNumRNew=searchNd->iNum;
          dataTemp->iNumRefNew=searchNd->timestamp;
          flag=1;
        }

      }
      nbTemp = nbTemp->nbNext;
  }
  if(flag==0)
  {
    dataTemp->reachability_distance=tempRd;
  }
  dataTemp1->reachability_distance=dataTemp1->reachability_distance;
  dataTemp2->reachability_distance=dataTemp1->reachability_distance;
  if(dataTemp->iNumRNew!=-1)
  {
    insertInBetweenTheOrderListForCombinedInsertionAndDeletion(dataTemp,dataTemp1,dataTemp2,dataList1,dataList2,hdrNdTree1,hdrNdTree2,pq,finalOrderList);
  }
  else
  {
    processAffectedPointNeighbourhoods(dataTemp,dataTemp1,dataTemp2,pq, dataList1, dataList2);
  }
  dataTemp->isProcessed=TRUE;

}
void ProcessOtherFixedPointsForCombinedInsertionAndDeletion(Data dataTemp,DataHdr dataList1,DataHdr dataList2,HdrNd hdrNdTree1,HdrNd hdrNdTree2, PQueue* pq)
{
    int iCnt = 0, pts, pts1, i, j;
    Data dequeued, dataTemp1, dataTemp2;
    NB nbTemp,nb_temp, nb_next;
    double distance,new_r_dist;
    Data searchNd;
    if(dataTemp->toBeDeleted==FALSE)
    {
      if(dataTemp->affected_tag==TRUE && dataTemp->isProcessed==0)
      {
          processAffectedPoint(dataTemp,dataList1,dataList2, hdrNdTree1, hdrNdTree2,pq);
      }
      else
      {
          if(dataTemp->timestamp==0 && dataTemp->core_distance!=UNDEFINED)
          {
              //predecessor of moving objects
              if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
              {
                  if(dataList1->dataClstElem[dataTemp->iNumR-1].toBeDeleted==FALSE && dataList1->dataClstElem[dataTemp->iNumR-1].fixed==0)
                  {
                      distance=findDist(dataTemp->iData,dataList1->dataClstElem[dataTemp->iNumR-1].iData);
                      new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                      // printf("new reachability_distance for predecessor %d %d is %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                      if(dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                      {
                          dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                          dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                          dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                          // printf("enqueing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                          pqueue_enqueue(pq, dataList1->dataClstElem + dataTemp->iNumR-1);
                          //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                          //decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                          dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                      }
                      else
                      {
                          if(dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                          {
                              dataList1->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                              dataList1->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                              // printf("decreasing %d %d\n",dataList1->dataClstElem[dataTemp->iNumR-1].iNum,dataList1->dataClstElem[dataTemp->iNumR-1].timestamp );
                              decrease1(dataList1->dataClstElem + dataTemp->iNumR-1, pq);
                              dataList1->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                          }
                      }
                  }
              }
              //successor of moving objects
              processSuccessorsForCombinedInsertionAndDeletion(dataTemp,dataList1,pq);
          }
          else if(dataTemp->core_distance!=UNDEFINED)
          {
              //predecessor of moving objects
              if(dataTemp->reachability_distance!=UNDEFINED && dataTemp->iNumR!=-1)
              {
                  if(dataList2->dataClstElem[dataTemp->iNumR-1].toBeDeleted==FALSE && dataList2->dataClstElem[dataTemp->iNumR-1].fixed==0)
                  {
                      distance=findDist(dataTemp->iData,dataList2->dataClstElem[dataTemp->iNumR-1].iData);
                      new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
                      // printf("new reachability_distance for predecessor %d %d is %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp ,new_r_dist );
                      if(dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ==0)
                      {
                          dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                          dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                          dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                          //printf("enqueing %d %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                          pqueue_enqueue(pq, dataList2->dataClstElem + dataTemp->iNumR-1);
                          //decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                          dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                      }
                      else
                      {
                          if(dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance > new_r_dist)
                          {
                              dataList2->dataClstElem[dataTemp->iNumR-1].reachability_distance=new_r_dist;
                              dataList2->dataClstElem[dataTemp->iNumR-1].iNumRNew=dataTemp->iNum;
                              dataList2->dataClstElem[dataTemp->iNumR-1].iNumRefNew=dataTemp->timestamp;
                              //printf("decreasing %d\n",dataList2->dataClstElem[dataTemp->iNumR-1].iNum,dataList2->dataClstElem[dataTemp->iNumR-1].timestamp );
                              decrease1(dataList2->dataClstElem + dataTemp->iNumR-1, pq);
                              dataList2->dataClstElem[dataTemp->iNumR-1].addedToPQ=1;
                          }
                      }
                  }
                  

              }
              processSuccessorsForCombinedInsertionAndDeletion(dataTemp,dataList2,pq);
          }

      }
    }
}
void processSuccessorsForCombinedInsertionAndDeletion(Data dataTemp,DataHdr dataList1,PQueue * pq)
{
  int i;
  double new_r_dist,distance;
  SuccessorNode current=dataTemp->sListOld->First;
  
  while(current!=NULL)
  {
    //printf("%d\t",current->iNum );
    i=current->iNum-1;
    if(dataList1->dataClstElem[i].toBeDeleted==FALSE && dataList1->dataClstElem[i].fixed==0)
    {
      distance=findDist(dataTemp->iData,dataList1->dataClstElem[i].iData);
      new_r_dist= (dataTemp->core_distance > distance) ? dataTemp->core_distance : distance;
      //printf("new reachability_distance for successor %d %d is %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp ,new_r_dist );
      if(dataList1->dataClstElem[i].addedToPQ==0)
      {
        dataList1->dataClstElem[i].reachability_distance=new_r_dist;
        dataList1->dataClstElem[i].iNumRNew=dataTemp->iNum;
        dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
       // printf("enqueing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
        pqueue_enqueue(pq, dataList1->dataClstElem + i);
        //decrease1(dataList1->dataClstElem + i, pq);
        dataList1->dataClstElem[i].addedToPQ=1;
      }
      else
      {
        if(dataList1->dataClstElem[i].reachability_distance > new_r_dist)
        {
          dataList1->dataClstElem[i].reachability_distance=new_r_dist;
          dataList1->dataClstElem[i].iNumRNew=dataTemp->iNum;
          dataList1->dataClstElem[i].iNumRefNew=dataTemp->timestamp;
         // printf("decreasing %d %d\n",dataList1->dataClstElem[i].iNum,dataList1->dataClstElem[i].timestamp );
          decrease1(dataList1->dataClstElem + i, pq);
          dataList1->dataClstElem[i].addedToPQ=1;
        }
      }
    }
    current=current->next;
  }
  //printf("\n");
  //getchar();

}
void reorganizeForBothInsertionAndDeletion(DataHdr dataList1,DataHdr dataList2,HdrNd dataTree1,HdrNd dataTree2,PointHdr OrderList1,PointHdr OrderList2,PointHdr finalOrderList, int * clusterFlagArray)
{
  OrderedDataNd current=OrderList1->dataFirstNd;
  Data currentDataNd,dequeued;
  NB nb_temp,nb_next;
  int i;
  PQueue *pq = pqueue_new(&compare_pq, OrderList1->uiCnt + OrderList2->uiCnt );
  while(current!=NULL)
  {
    currentDataNd=dataList1->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE)
    {
      if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
      {
        
        processAffectedPoint(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq);
      }
    }
    current=current->dataNextNd;
    if(pq->size>0)
    {
      //printf("initial %d\n",currentDataNd->iNumNew );
      fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,1);
      break;
    }
  }
  while(current!=NULL)
  {
    currentDataNd=dataList1->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
    {
      if(pq->size>0)
      {
        if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && dataList1->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
        {
          printf("inumR fixed found %d %d %d %lf %d %lf %d\n",currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->iNumR,currentDataNd->reachability_distance,dataList1->dataClstElem[currentDataNd->iNumR-1].affected_tag,dataList1->dataClstElem[currentDataNd->iNumR-1].core_distance,dataList1->dataClstElem[currentDataNd->iNumR-1].rdUndef);
          ProcessOtherFixedPointsForCombinedInsertionAndDeletion(currentDataNd, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,2);
          current=current->dataNextNd;
        }
        else
        {
          dequeued=(Data)pqueue_dequeue(pq);
           ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,3); 
        }
      }
      else
      {
        while(current!=NULL)
        {
          currentDataNd=dataList1->dataClstElem+current->iNum-1;
          //if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0)
          if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
          {
            if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0 && clusterFlagArray[currentDataNd->clusterId-1]==0)
            //if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
            {
              // processAffectedPointModified(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq,finalOrderList);
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
               processAffectedPoint(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq);
            }
          }
          current=current->dataNextNd;
          if(pq->size>0 && currentDataNd->fixed==0)
          {
            //printf("between %d\n",currentDataNd->iNumNew );
            fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,4); 
            break;
          }

        }
        // processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq);
        // fixForDeletion(initialDataList,finalOrderList,currentDataNd,4);
        // current=current->dataNextNd;
      }
    }
    else
    {
      current=current->dataNextNd;
    }

  }
  while(pq->size>0)
  {
    
    dequeued=(Data)pqueue_dequeue(pq);
           ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,5); 
  }
  current = OrderList2->dataFirstNd;
  while(current!=NULL)
  {
    currentDataNd=dataList2->dataClstElem+current->iNum-1;
    if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
    {
      if(pq->size>0)
      {
        if(currentDataNd->reachability_distance<((Data)pqueue_top(pq))->reachability_distance && dataList2->dataClstElem[currentDataNd->iNumR-1].fixed==1 )
        {
          printf("inumR fixed found %d %d %d %lf %d %lf %d\n",currentDataNd->iNum,currentDataNd->iNumR,currentDataNd->iNumR,currentDataNd->reachability_distance,dataList2->dataClstElem[currentDataNd->iNumR-1].affected_tag,dataList2->dataClstElem[currentDataNd->iNumR-1].core_distance,dataList2->dataClstElem[currentDataNd->iNumR-1].rdUndef);
          ProcessOtherFixedPointsForCombinedInsertionAndDeletion(currentDataNd, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,6);
          current=current->dataNextNd;
        }
        else
        {
          dequeued=(Data)pqueue_dequeue(pq);
           ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,7); 
        }
      }
      else
      {
        while(current!=NULL)
        {
          currentDataNd=dataList2->dataClstElem+current->iNum-1;
          //if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0)
          if(currentDataNd->toBeDeleted==FALSE && currentDataNd->fixed==0 && currentDataNd->addedToPQ==0)
          {
            if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0 )
            //if(currentDataNd->affected_tag==TRUE && currentDataNd->isProcessed==0)
            {
              processAffectedPoint(currentDataNd,dataList1,dataList2, dataTree1, dataTree2,pq);
              //fixForDeletion(initialDataList,finalOrderList,currentDataNd,0);
            }
          }
          current=current->dataNextNd;
          if(pq->size>0 && currentDataNd->fixed==0)
          {
            //printf("between %d\n",currentDataNd->iNumNew );
            fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,8); 
            break;
          }

        }
        // processOtherPointsForDeletion(currentDataNd,initialDataList,DataTree,pq);
        // fixForDeletion(initialDataList,finalOrderList,currentDataNd,4);
        // current=current->dataNextNd;
      }
    }
    else
    {
      current=current->dataNextNd;
    }

  }
  while(pq->size>0)
  {
    
    dequeued=(Data)pqueue_dequeue(pq);
           ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,9); 
  }
  current=OrderList1->dataFirstNd;
  // printf("count3=%d\n",count3 );
  // printf("count4=%d\n",count4 );
 while(current!=NULL)
 {
  
    if(dataList1->dataClstElem[current->iNum-1].fixed==0 && dataList1->dataClstElem[current->iNum-1].toBeDeleted==FALSE && clusterFlagArray[dataList1->dataClstElem[current->iNum-1].clusterId-1]==0 )
        {
            
            currentDataNd=dataList1->dataClstElem+current->iNum-1;
                fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,10); 
            
        }
        current=current->dataNextNd;
  }
  current=OrderList2->dataFirstNd;
 while(current!=NULL)
 {
  
    if(dataList2->dataClstElem[current->iNum-1].fixed==0 && dataList2->dataClstElem[current->iNum-1].toBeDeleted==FALSE  )
        {
            
            currentDataNd=dataList2->dataClstElem+current->iNum-1;
                fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,11); 
            
        }
        current=current->dataNextNd;
  }
  // printf("hi 1\n");
  current=OrderList1->dataFirstNd;
  count1=0;
  while(current!=NULL)
  {
      if(dataList1->dataClstElem[current->iNum-1].fixed==0 && dataList1->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
      {
        currentDataNd = dataList1->dataClstElem + current->iNum-1;
        if(currentDataNd->rdUndef==TRUE)
        {
          count1++;
          recomputeRdForRdUndefForCombinedInsertionAndDeletion(currentDataNd,dataList1,dataTree1,dataList2,dataTree2,pq,finalOrderList);
          
        }
      }
      current=current->dataNextNd;
  }
  char tempString1[500];
  sprintf(tempString1,"rdUndefPoints_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  FILE *fp=fopen(tempString1,"a");
  fprintf(fp,"%d\n",count1 );
  fclose(fp);
  // printf("hi 2\n");
  current=OrderList1->dataFirstNd;
  count1=0;
  while(current!=NULL)
  {
      if(dataList1->dataClstElem[current->iNum-1].fixed==0 && dataList1->dataClstElem[current->iNum-1].toBeDeleted==FALSE )
      {
        currentDataNd = dataList1->dataClstElem + current->iNum-1;
        if(clusterFlagArray[currentDataNd->clusterId-1]==1 && currentDataNd->core_distance!=UNDEFINED && currentDataNd->rdUndef!=TRUE)
        {
          count1++;
          recomputeRdForRdUndefForCombinedInsertionAndDeletion(currentDataNd,dataList1,dataTree1,dataList2,dataTree2,pq,finalOrderList);
        }
      }
      
      current=current->dataNextNd;
  }
  sprintf(tempString1,"rdUndefClusterPoints_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  fp=fopen(tempString1,"a");
  fprintf(fp,"%d\n",count1 );
  fclose(fp);
  // printf("hi 3 count1=%d\n",count1);
  current = OrderList1->dataFirstNd;
  while(current!=NULL)
  {
     if(dataList1->dataClstElem[current->iNum-1].fixed==0 && dataList1->dataClstElem[current->iNum-1].toBeDeleted==FALSE)
      {
        currentDataNd=dataList1->dataClstElem+current->iNum-1;
        {
          ProcessOtherFixedPointsForCombinedInsertionAndDeletion(currentDataNd, dataList1, dataList2,dataTree1,dataTree2, pq);
          fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,currentDataNd,12); 
          while(pq->size>0)
          {
            
            dequeued=(Data)pqueue_dequeue(pq);
            // if(dequeued->affected_tag==TRUE && dequeued->NeighborhoodComputed==0)
            //       computeNeighborhood_CoreDistanceAndReachabilityDist(dequeued,initialDataList,DataTree);
            ProcessOtherFixedPointsForCombinedInsertionAndDeletion(dequeued, dataList1, dataList2,dataTree1,dataTree2, pq);
            fixForCombinedInsertionAndDeletion(dataList1,dataList2,finalOrderList,dequeued,14); 
          }
        //count2=1;
        
        }
      }
          // initialDataList->dataClstElem[current->iNum-1].fixed=1;
          // initialDataList->dataClstElem[current->iNum-1].reachability_distance=UNDEFINED;
          // initialDataList->dataClstElem[current->iNum-1].iNumR=-1;
      current=current->dataNextNd;
  }
  //printf("hi 4\n");
   current = OrderList1->dataFirstNd;
    while(current!=NULL)
    {
        if(dataList1->dataClstElem[current->iNum-1].fixed==0 &&dataList1->dataClstElem[current->iNum-1].toBeDeleted==FALSE  )
        {

           currentDataNd=dataList1->dataClstElem + current->iNum-1;
           AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,currentDataNd->timestamp,currentDataNd->iNum);
            //AddToNewSuccessorList(dataList1,dataList2,dataList2->dataClstElem + current->iNum-1);
            dataList1->dataClstElem[current->iNum-1].fixed=1;
            //countIndependent++;
            //printf("fixing rest of 2nd list %d %d %lf %lf\n", dataList2->dataClstElem[current->iNum-1].iNum, dataList2->dataClstElem[current->iNum-1].timestamp, dataList2->dataClstElem[current->iNum-1].core_distance, dataList2->dataClstElem[current->iNum-1].reachability_distance);
        }
        current=current->dataNextNd;
    }
    //printf("hi 5\n");
    current = OrderList2->dataFirstNd;
    while(current!=NULL)
    {
        if(dataList2->dataClstElem[current->iNum-1].fixed==0 )
        {

            currentDataNd=dataList2->dataClstElem + current->iNum-1;
           AddToOrderListForDeletion(finalOrderList,currentDataNd->iNumNew,currentDataNd->timestamp,currentDataNd->iNum);
            //AddToNewSuccessorList(dataList1,dataList2,dataList2->dataClstElem + current->iNum-1);
            dataList2->dataClstElem[current->iNum-1].fixed=1;
           // countIndependent++;
            //printf("fixing rest of 2nd list %d %d %lf %lf\n", dataList2->dataClstElem[current->iNum-1].iNum, dataList2->dataClstElem[current->iNum-1].timestamp, dataList2->dataClstElem[current->iNum-1].core_distance, dataList2->dataClstElem[current->iNum-1].reachability_distance);
        }
        current=current->dataNextNd;
    }
    // printf("hi 6\n");
    return;
}
void PrintOrderedListInToFileForCombinedInsertionAndDeletion(PointHdr UpdatedList,DataHdr dataList1,DataHdr dataList2, char * strFileName,int noOfObjects, int deletedCount )
{
    if(UpdatedList->dataFirstNd == NULL)
    {
        printf("Empty Ordered List\n");
        return;
    }
    int iCnt = 0,iNumR,iNum,globalTimestamp;
    double cD,rD;
    FILE *fp=fopen(strFileName,"w");
    OrderedDataNd dataTemp = UpdatedList->dataFirstNd;
    Data currentDataNd;
   // printf("hi 7\n");
    while(dataTemp != NULL)
    {
       
        if(dataTemp->timestamp==0)
        {
            currentDataNd=dataList1->dataClstElem + dataTemp->oldiNum -1;
            // if(currentDataNd->iNumNew==24542)
            //   printf("%d %d %lf %lf %d %d %d %d\n",currentDataNd->iNum,currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRef,currentDataNd->iNumRNew,currentDataNd->iNumRefNew );
            if(currentDataNd->iNumNew<=0)
            {
              printf("iNumNew zero %d %d\n",dataTemp->oldiNum,dataTemp->iNum );
            printf("1 %d %d %d %d %d %d %d %lf %lf %d %d\n",dataTemp->iNum,currentDataNd->iNum, currentDataNd->iNumNew, currentDataNd->iNumR, currentDataNd->iNumRef, currentDataNd->iNumRNew, currentDataNd->iNumRefNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->toBeDeleted, currentDataNd->rdUndef );
            getchar();
          }
            if(currentDataNd->iNumRefNew==1)
            {
              currentDataNd->iNumRNew=currentDataNd->iNumRNew + noOfObjects - deletedCount;
            }
            else if(currentDataNd->iNumRefNew==0)
            {
              currentDataNd->iNumRNew=dataList1->dataClstElem[currentDataNd->iNumRNew-1].iNumNew;
            }
            else if(currentDataNd->iNumRef==0)
            {
              currentDataNd->iNumR=dataList1->dataClstElem[currentDataNd->iNumR-1].iNumNew;
            }
            else if(currentDataNd->iNumRef==1)
            {
              currentDataNd->iNumR=currentDataNd->iNumR + noOfObjects - deletedCount;
            }
        }
        else
        {
            currentDataNd=dataList2->dataClstElem + dataTemp->oldiNum -1;
            // if(currentDataNd->iNumNew==24542)
            //   printf("%d %d %lf %lf %d %d %d %d\n",currentDataNd->iNum,currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRef,currentDataNd->iNumRNew,currentDataNd->iNumRefNew );
            if(currentDataNd->iNumNew<=0)
            {
              printf("iNumNew zero %d %d\n",dataTemp->oldiNum,dataTemp->iNum );
            printf("2 %d %d %d %d %d %d %lf %lf\n",currentDataNd->iNum, currentDataNd->iNumNew, currentDataNd->iNumR, currentDataNd->iNumRef, currentDataNd->iNumRNew, currentDataNd->iNumRefNew, currentDataNd->iNumRefNew,currentDataNd->core_distance,currentDataNd->reachability_distance );
          }
            if(currentDataNd->iNumRefNew==1)
            {
              currentDataNd->iNumRNew=currentDataNd->iNumRNew + noOfObjects - deletedCount;
            }
            else if(currentDataNd->iNumRefNew==0)
            {
              currentDataNd->iNumRNew=dataList1->dataClstElem[currentDataNd->iNumRNew-1].iNumNew;
            }
            else if(currentDataNd->iNumRef==1)
            {
              currentDataNd->iNumR=currentDataNd->iNumRNew + noOfObjects - deletedCount;
            }
            else if(currentDataNd->iNumRef==0)
            {
              currentDataNd->iNumR=dataList1->dataClstElem[currentDataNd->iNumR-1].iNumNew;
            }
            //currentDataNd->iNum+=noOfObjects- deletedCount;
        }

        globalTimestamp=currentDataNd->globalTimestamp;
        cD=currentDataNd->core_distance;
        rD=currentDataNd->reachability_distance;
        if(currentDataNd->iNumRefNew!=-1)
          iNumR=currentDataNd->iNumRNew;
        else
          iNumR=currentDataNd->iNumR;
        iNum=currentDataNd->iNumNew;
         // if(currentDataNd->iNumNew==24542)
         //      printf("%d %d %lf %lf %d %d %d %d\n",currentDataNd->iNum,currentDataNd->iNumNew,currentDataNd->core_distance,currentDataNd->reachability_distance,currentDataNd->iNumR,currentDataNd->iNumRef,currentDataNd->iNumRNew,currentDataNd->iNumRefNew );
        fprintf(fp,"%d %d %.100lf %.100lf %d %d\n",iNum,globalTimestamp,cD,rD,iNumR,currentDataNd->neighbors->nbhCnt);
        dataTemp = dataTemp->dataNextNd;
    }
    //printf("hi 8\n");
    fclose(fp);
    return;
}
void insertionAndDeletionCombined(char * initialFile,char *initialOrderListFile,char* newDataListFile,char *outputFile ,int timestampTobeDeleted, int currentTimestamp,char*finalInputFile)
{
  int i,j,deletedCount,affectedPoints=0,intersectionPoints=0;
  // printf("initial file=%s\n intitialOutputFile=%s\n newFile=%s\n outputFile=%s \n timestampTobeDeleted=%d\n currentTimestamp=%d\n",initialFile,initialOrderListFile,newDataListFile,outputFile ,timestampTobeDeleted, currentTimestamp );
  DataHdr initialDataList=readData(initialFile), newDataList=readData(newDataListFile);
  PointHdr initialOrderList=initPointHdr(),newOrderList=initPointHdr(),finalOrderList=initPointHdr();
  HdrNd newDataTree=buildRTree(newDataList);
  // printf("initial file=%s\n intitialOutputFile=%s\n newFile=%s\n outputFile=%s \n timestampTobeDeleted=%d\n currentTimestamp=%d\n",initialFile,initialOrderListFile,newDataListFile,outputFile ,timestampTobeDeleted, currentTimestamp );
  for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].globalTimestamp=currentTimestamp;
    newDataList->dataClstElem[i].timestamp=1;
  }
  Optics(newDataList,newDataTree,newOrderList );
  // printf("optics finished\n");
  for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].isProcessed=FALSE;
  }
  int noOfClusters=buildClusterOrdering(initialOrderListFile,initialOrderList,initialDataList);
  int *clusterFlagArray=(int*)malloc(sizeof(int)*noOfClusters);
  assert(clusterFlagArray!=NULL);
  for(i=0;i<noOfClusters;i++)
    clusterFlagArray[i]=0;
  // printf("cluster ordering finished\n");
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].globalTimestamp==timestampTobeDeleted)
      initialDataList->dataClstElem[i].toBeDeleted=TRUE;
  }
  HdrNd initialDataTree = buildRTreeForDeletion(initialDataList);
  deletedCount= MarkAffectedPointsForDeletion(initialDataList,initialDataTree,timestampTobeDeleted,clusterFlagArray);
  // printf("marking deleted points end\n");
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].toBeDeleted==TRUE)
    {
      deleteRecord(NULL,initialDataTree,initialDataList->dataClstElem+i);
    }
  }
  GetAffectedListUsingOverlapStrategy(initialDataTree,newDataTree,initialDataList,newDataList);
  // printf("affected list end\n");
  for(i=0;i<newDataList->uiCnt;i++)
  {
    if(newDataList->dataClstElem[i].affected_tag==TRUE)
      intersectionPoints++;
    newDataList->dataClstElem[i].iNumNew = newDataList->dataClstElem[i].iNum + initialDataList->uiCnt - deletedCount;
  }
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].affected_tag==TRUE)
      affectedPoints++;
  }
  //printf("affectedPoints=%d\n",affectedPoints );
  //neighborhoodQueryCount=0;
  reorganizeForBothInsertionAndDeletion(initialDataList,newDataList,initialDataTree,newDataTree,initialOrderList,newOrderList,finalOrderList,clusterFlagArray);
  PrintOrderedListInToFileForCombinedInsertionAndDeletion(finalOrderList,initialDataList,newDataList,outputFile,initialDataList->uiCnt,deletedCount);
  // currentTimestamp++;
  // fp=fopen(currentTimestampFile,"w");
  // fprintf(fp,"%d",currentTimestamp);
  // fclose(fp);
  writeBothInputFilesInToIntermediateInputFile(initialDataList,newDataList,finalInputFile);
  FILE *fp,*fp1,*fp2,*fp3,*fp4;
  char tempString1[500],tempString2[500],tempString3[500],tempString4[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"IntersectionPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString3,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString4,"neighborhoodQueryCountInTree2_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  if(currentTimestamp==1)
  {
    fp=fopen(tempString1,"w");
    fp1=fopen(tempString2,"w");
    fp2=fopen(tempString3,"w");
    fp3=fopen(tempString4,"w");
    fp4=fopen(tempString5,"w");
  }
  else
  {
    fp=fopen(tempString1,"a");
    fp1=fopen(tempString2,"a");
    fp2=fopen(tempString3,"a");
    fp3=fopen(tempString4,"a");
    fp4=fopen(tempString5,"a");
  }
  fprintf(fp,"%d\n",affectedPoints + intersectionPoints );
  fprintf(fp1,"%d\n", intersectionPoints );
  fprintf(fp2,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp3,"%d\n", neighborhoodQueryCountInTree2 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
}
void runClassicalOpticsForDeletion(char * initialFile,char *initialOrderListFile,char* newDataListFile,char *outputFile ,int timestampTobeDeleted, int currentTimestamp,char*finalInputFile)
{
  int i,j,deletedCount,affectedPoints=0,intersectionPoints=0;
  // printf("initial file=%s\n intitialOutputFile=%s\n newFile=%s\n outputFile=%s \n timestampTobeDeleted=%d\n currentTimestamp=%d\n",initialFile,initialOrderListFile,newDataListFile,outputFile ,timestampTobeDeleted, currentTimestamp );
  DataHdr initialDataList=readData(initialFile), newDataList=readData(newDataListFile);
  PointHdr initialOrderList=initPointHdr(),newOrderList=initPointHdr(),finalOrderList=initPointHdr();
  int noOfClusters=buildClusterOrderingForOPTICS(initialOrderListFile,initialOrderList,initialDataList);
  for(i=0;i<initialDataList->uiCnt;i++)
  {
    if(initialDataList->dataClstElem[i].globalTimestamp==timestampTobeDeleted)
      initialDataList->dataClstElem[i].toBeDeleted=TRUE;
    initialDataList->dataClstElem[i].timestamp=1;
  }
  for(i=0;i<newDataList->uiCnt;i++)
  {
    newDataList->dataClstElem[i].globalTimestamp=currentTimestamp;
    newDataList->dataClstElem[i].timestamp=1;
  }
  DataHdr tempDataList=mergeDataList(initialDataList,newDataList);
  DataHdr tempDataList2=initDataHdr(0);
  HdrNd newDataTree=buildRTree(tempDataList);
  // printf("initial file=%s\n intitialOutputFile=%s\n newFile=%s\n outputFile=%s \n timestampTobeDeleted=%d\n currentTimestamp=%d\n",initialFile,initialOrderListFile,newDataListFile,outputFile ,timestampTobeDeleted, currentTimestamp );
  
  Optics(tempDataList,newDataTree,newOrderList );
  // printf("optics finished\n");
  // for(i=0;i<newDataList->uiCnt;i++)
  // {
  //   newDataList->dataClstElem[i].isProcessed=FALSE;
  // }
  // int noOfClusters=buildClusterOrdering(initialOrderListFile,initialOrderList,initialDataList);
  // int *clusterFlagArray=(int*)malloc(sizeof(int)*noOfClusters);
  // assert(clusterFlagArray!=NULL);
  // for(i=0;i<noOfClusters;i++)
  //   clusterFlagArray[i]=0;
  // // printf("cluster ordering finished\n");
  // for(i=0;i<initialDataList->uiCnt;i++)
  // {
  //   if(initialDataList->dataClstElem[i].globalTimestamp==timestampTobeDeleted)
  //     initialDataList->dataClstElem[i].toBeDeleted=TRUE;
  // }
  // HdrNd initialDataTree = buildRTreeForDeletion(initialDataList);
  // deletedCount= MarkAffectedPointsForDeletion(initialDataList,initialDataTree,timestampTobeDeleted,clusterFlagArray);
  // printf("marking deleted points end\n");
  // for(i=0;i<initialDataList->uiCnt;i++)
  // {
  //   if(initialDataList->dataClstElem[i].toBeDeleted==TRUE)
  //   {
  //     deleteRecord(NULL,initialDataTree,initialDataList->dataClstElem+i);
  //   }
  // }
  // GetAffectedListUsingOverlapStrategy(initialDataTree,newDataTree,initialDataList,newDataList);
  // // printf("affected list end\n");
  // for(i=0;i<newDataList->uiCnt;i++)
  // {
  //   if(newDataList->dataClstElem[i].affected_tag==TRUE)
  //     intersectionPoints++;
  //   newDataList->dataClstElem[i].iNumNew = newDataList->dataClstElem[i].iNum + initialDataList->uiCnt - deletedCount;
  // }
  // for(i=0;i<initialDataList->uiCnt;i++)
  // {
  //   if(initialDataList->dataClstElem[i].affected_tag==TRUE)
  //     affectedPoints++;
  // }
  // //printf("affectedPoints=%d\n",affectedPoints );
  // //neighborhoodQueryCount=0;
  // reorganizeForBothInsertionAndDeletion(initialDataList,newDataList,initialDataTree,newDataTree,initialOrderList,newOrderList,finalOrderList,clusterFlagArray);
  // PrintOrderedListInToFileForCombinedInsertionAndDeletion(,initialDataList,newDataList,outputFile,initialDataList->uiCnt,deletedCount);
  // currentTimestamp++;
  // fp=fopen(currentTimestampFile,"w");
  // fprintf(fp,"%d",currentTimestamp);
  // fclose(fp);
  // writeBothInputFilesInToIntermediateInputFile(initialDataList,newDataList,finalInputFile);
  PrintOrderedListInToFileForAddtion(newOrderList,tempDataList2,tempDataList,outputFile,tempDataList2->uiCnt);
  // currentTimestamp++;
  // fp=fopen(currentTimestampFile,"w");
  // fprintf(fp,"%d",currentTimestamp);
  // fclose(fp);
  //printf("before\n");
  writeBothInputFilesInToIntermediateInputFile(initialDataList,newDataList,finalInputFile);
  FILE *fp,*fp1,*fp2,*fp3,*fp4;
  char tempString1[500],tempString2[500],tempString3[500],tempString4[500],tempString5[500];
  sprintf(tempString1,"AffectedPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString2,"IntersectionPointsCount_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString3,"neighborhoodQueryCountInTree1_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString4,"neighborhoodQueryCountInTree2_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  sprintf(tempString5,"nodesVisitedInRtree_%s_%d_%d.txt",originalDataFile,noOfPartitions,windowSize);
  if(currentTimestamp==1)
  {
    fp=fopen(tempString1,"w");
    fp1=fopen(tempString2,"w");
    fp2=fopen(tempString3,"w");
    fp3=fopen(tempString4,"w");
    fp4=fopen(tempString5,"w");
  }
  else
  {
    fp=fopen(tempString1,"a");
    fp1=fopen(tempString2,"a");
    fp2=fopen(tempString3,"a");
    fp3=fopen(tempString4,"a");
    fp4=fopen(tempString5,"a");
  }
  fprintf(fp,"%d\n",affectedPoints + intersectionPoints );
  fprintf(fp1,"%d\n", intersectionPoints );
  fprintf(fp2,"%d\n", neighborhoodQueryCountInTree1 );
  fprintf(fp3,"%d\n", neighborhoodQueryCountInTree2 );
  fprintf(fp4,"%lld\n", nodesVisitedInRtree );
  fclose(fp);
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
}
