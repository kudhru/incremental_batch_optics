#include "Data.h"

DataHdr initDataHdr(int size)
{   DataHdr dataHdrInfo = (DataHdr)malloc(sizeof(struct dataHdr));
	if(dataHdrInfo == NULL)
		return NULL;

	dataHdrInfo->uiCnt = 0;
	dataHdrInfo->dataClstElem=(Data)malloc(sizeof(struct data)*size);
	assert(dataHdrInfo->dataClstElem!=NULL);
	return dataHdrInfo;
}
SuccessorListHeader initSuccessorList()
{
	SuccessorListHeader sList=(SuccessorListHeader)malloc(sizeof(struct successor_list_header));
	sList->successorCount=0;
	sList->First=NULL;
	return sList;
}
Data initData(DataPoint iData)
{   
 if(iData == NULL)
		return NULL;

	Data dataClstElem = (Data)malloc(sizeof(struct data));
	assert(dataClstElem!=NULL);
	if(dataClstElem == NULL)
		return NULL;
	dataClstElem->iData = iData;
	dataClstElem->core_distance = UNDEFINED;
	dataClstElem->reachability_distance = UNDEFINED;
	dataClstElem->iNum=0;
	dataClstElem->isProcessed = FALSE;
	dataClstElem->core_tag = FALSE;
	dataClstElem->neighbors = NULL;
	dataClstElem->iNumR = -1;
	dataClstElem->iNumRef = -1;
	dataClstElem->affected_tag = FALSE;
	dataClstElem->fixed = 0;
	dataClstElem->addedToPQ=0;
	dataClstElem->iNumRNew=-1;
	dataClstElem->iNumRefNew=-1;
	dataClstElem->sListOld=initSuccessorList();
	return dataClstElem;
}

PointHdr initPointHdr() //Sonal made
{   PointHdr dataHdrInfo = (PointHdr)malloc(sizeof(struct pointHdr));
	if(dataHdrInfo == NULL)
		return NULL;

	dataHdrInfo->uiCnt = 0;	
	dataHdrInfo->dataFirstNd = NULL;
	dataHdrInfo->dataLastNd = NULL; 
	//dataHdrInfo->dataClstElem=(Data)malloc(sizeof(struct data)*size);
	return dataHdrInfo;
}
void freePointHdr(PointHdr orderList)
{
	if(orderList==NULL || orderList->uiCnt==0 )
		return;
	OrderedDataNd current,tempNd;
	//OrderedData cur,temp;
	current=orderList->dataFirstNd;
	while(current!=NULL)
	{
		tempNd=current;
		//temp=tempNd->dataClstElem;
		current=current->dataNextNd;
		free(tempNd);
		//free(temp);
	}
	free(orderList);
	return;
}
Boolean isDataLstEmpty(DataHdr dataHdrInfo)
{    
	return(dataHdrInfo == NULL || dataHdrInfo->uiCnt == 0) ? TRUE : FALSE;
}

void insertDataLstElem(DataHdr dataHdrInfo, DataPoint iData)
{    
	
	Data dataClstElem=dataHdrInfo->dataClstElem + dataHdrInfo->uiCnt;
	dataClstElem->iData = iData;
	dataClstElem->core_distance = UNDEFINED;
	dataClstElem->reachability_distance = UNDEFINED;
	dataClstElem->iNum=dataHdrInfo->uiCnt+1;
	dataClstElem->isProcessed = FALSE;
	dataClstElem->core_tag = FALSE;
	dataClstElem->neighbors = NULL;
	dataClstElem->iNumR = -1;
	dataClstElem->iNumRef = -1;
	dataClstElem->affected_tag = FALSE;
	dataClstElem->fixed = 0;
	dataClstElem->addedToPQ=0;
	dataClstElem->iNumRNew=-1;
	dataClstElem->iNumRefNew=-1;
	dataClstElem->sListOld=initSuccessorList();
	dataClstElem->clusterId=-1;
	dataClstElem->addedToNeighborHoodIndex=0;
	dataClstElem->timestamp=0;
	dataClstElem->globalTimestamp=0;
	dataClstElem->rdUndef=FALSE;
	dataClstElem->toBeDeleted=FALSE;
	 omp_init_lock(&dataClstElem->lock);
	 dataHdrInfo->uiCnt++;
	return;
	//return dataClstElem;
}
Data insertDataLstNd(DataHdr dataHdrInfo, Data dataClstElem)
{   
 if(dataClstElem == NULL || dataHdrInfo == NULL)
		return NULL;
	
	//DataNd dataClstNd = initDataNd(dataClstElem);

	/*if(isDataLstEmpty(dataHdrInfo))
    {   dataHdrInfo->dataFirstNd = dataClstNd;
		dataHdrInfo->dataLastNd = dataClstNd;
    }
	else
    {   dataHdrInfo->dataLastNd->dataNextNd = dataClstNd;
		dataHdrInfo->dataLastNd = dataClstNd;
    }*/
	dataHdrInfo->dataClstElem[dataHdrInfo->uiCnt]=*dataClstElem;	
    dataHdrInfo->uiCnt++;
	
	return dataClstElem;
}
void printDataLst(DataHdr dataHdrInfo)
{    //printf("++++++++++CALL TO PRINT THE LIST__________+++++++++++++++++++\n");
	if(isDataLstEmpty(dataHdrInfo))
		return;
	//printf("++++++++++CALL TO PRINTing THE representative  LIST +++++++++++++++++++\n");
	int iCnt = 0,j;
	//DataNd dataTemp = dataHdrInfo->dataFirstNd;
	for(j=0;j<dataHdrInfo->uiCnt;j++)
	{
		printf("RecNum %d\t",dataHdrInfo->dataClstElem[j].iNum);
		for(iCnt = 0; iCnt < DIMENSION; iCnt++ )
			printf("%lf ", dataHdrInfo->dataClstElem[j].iData[iCnt] );
		printf("\n");
		
	}
	
  return;
}

NbHdr initNbHdr()
{   
	NbHdr nbHdrInfo = (NbHdr)malloc(sizeof(struct nbHdr));
	if(nbHdrInfo == NULL)
		return NULL;

	nbHdrInfo->nbhCnt = 0;
	nbHdrInfo->nbFirst =NULL;
	nbHdrInfo->nbLast = NULL;
	
	return nbHdrInfo;
}

Boolean isNbLstEmpty(NbHdr nbHdrInfo)
{    return(nbHdrInfo == NULL || nbHdrInfo->nbhCnt == 0) ? TRUE : FALSE;
}
void insertNeighbors(Data dataTemp, Data dataClstElem, double dist)
{
   NbHdr nbHdrInfo = dataTemp->neighbors;
    
	NB newNb;
	if(isNbLstEmpty(nbHdrInfo))
    {   newNb = (NB)malloc(sizeof(struct nb));
	
	    newNb->n = dataClstElem->iNum;
	    newNb->dist = dist;
	    newNb->nbNext = NULL;
	    
        nbHdrInfo->nbFirst = newNb;
		nbHdrInfo->nbLast = newNb;
	}
	else
    {   newNb = (NB)malloc(sizeof(struct nb));
        newNb->n = dataClstElem->iNum;
	    newNb->dist = dist;
	    newNb->nbNext = NULL;
        
        nbHdrInfo->nbLast->nbNext = newNb;
		nbHdrInfo->nbLast = newNb;
    }
    dataTemp->neighbors->nbhCnt++;
    
}
void insertNeighbors_1(Data dataTemp, Data dataClstElem, double dist)
{   
   NbHdr nbHdrInfo = dataTemp->neighbors;
    
	NB newNb;

	if(isNbLstEmpty(nbHdrInfo))
    {   newNb = (NB)malloc(sizeof(struct nb));
	
	    newNb->n = dataClstElem->iNum;
	    newNb->dist = dist;
	    newNb->nbNext = NULL;
	    
        nbHdrInfo->nbFirst = newNb;
		nbHdrInfo->nbLast = newNb;
    }
	else
    {   newNb = (NB)malloc(sizeof(struct nb));
        newNb->n = dataClstElem->iNum;
	    newNb->dist = dist;
	    newNb->nbNext = NULL;
        
        nbHdrInfo->nbLast->nbNext = newNb;
		nbHdrInfo->nbLast = newNb;
    }
    dataTemp->neighbors->nbhCnt++;
    
}

void printNbhLst(NbHdr nbHdrInfo,DataHdr dataList)
{   
	
   if(isNbLstEmpty(nbHdrInfo))
		return;
	//printf("++++++++++CALL TO PRINTing THE representative  LIST +++++++++++++++++++\n");
	int iCnt = 0;
	NB nbTemp = nbHdrInfo->nbFirst;

	while(nbTemp != NULL)
    {   //printf("++++++++++CALL TO PRINT THE LIST__________+++++++++++++++++++\n");
	    printf("\nN = %d \t dist = %lf\t ", nbTemp->n, nbTemp->dist);
	    for(iCnt=0;iCnt<DIMENSION;iCnt++)
	    	printf("%lf\t",dataList->dataClstElem[nbTemp->n-1].iData[iCnt] );
	    printf("\n");
        nbTemp = nbTemp->nbNext;
   }
   return;
}

int compare_pq(Data data1,Data data2)
{
	if(data1->reachability_distance < data2->reachability_distance)
		return 1;
	else
		return 0;	
}

PQueue *pqueue_new(int (*cmp)(Data, Data), size_t capacity) 
{
    PQueue *res = NULL;
    NP_CHECK(cmp);
    res = malloc(sizeof(*res));
    NP_CHECK(res);
    res->cmp = cmp;
    /* The inner representation of data inside the queue is an array of void* */
    res->data = malloc(capacity * sizeof(*(res->data)));
    NP_CHECK(res->data);
    res->size = 0;
    res->capacity=capacity;
    return (res);
}

/**
* De-allocates memory for a given Priority Queue structure .
*/
void pqueue_delete(PQueue *q) 
{
    if (NULL != q) {
        DEBUG("Priority Queue is already NULL. Nothing to free.");
        return;
    }
    free(q->data);
    free(q);
}

/**
* Adds a new element to the Priority Queue .
*/
void pqueue_enqueue(PQueue *q, const void *data) 
{
    size_t i;
    void *tmp = NULL;
    NP_CHECK(q);
    if (q->size >= q->capacity) {
        DEBUG("Priority Queue is full. Cannot add another element .");
        printf("q->size=%d\t q->capacity=%d data=%d\n",(int)q->size,(int)q->capacity,((Data)data)->iNum);
        return;
    }
    /* Adds element last */
    q->data[q->size] = (void*) data;
    i = q->size;
    q->size++;
	//     printf("q->size : %d\n", q->size);
    /* The new element is swapped with its parent as long as its
    precedence is higher */
    while(i > 0 && q->cmp(q->data[i], q->data[PARENT(i)]) > 0) {
        tmp = q->data[i];
        q->data[i] = q->data[PARENT(i)];
        q->data[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

/**
* Returns the element with the biggest priority from the queue .
*/
void *pqueue_dequeue(PQueue *q)
 {
    void *data = NULL;
    NP_CHECK(q);
    if (q->size < 1) {         /* Priority Queue is empty */         DEBUG("Priority Queue underflow . Cannot remove another element .");         return NULL;     }     data = q->data[0];
    q->data[0] = q->data[q->size-1];
    q->size--;
    /* Restore heap property */
    pqueue_heapify(q, 0);
    return (data);
}
void *pqueue_top(PQueue *q) {
    void *data = NULL;
    NP_CHECK(q);
    if (q->size < 1) {         /* Priority Queue is empty */         DEBUG("Priority Queue underflow . Cannot remove another element .");         return NULL;     }   
    data = q->data[0];
    //q->data[0] = q->data[q->size-1];
    //q->size--;
    /* Restore heap property */
    //pqueue_heapify(q, 0);
    return (data);
}
/**
* Turn an "almost-heap" into a heap .
*/
void pqueue_heapify(PQueue *q, size_t idx)
 {
    /* left index, right index, largest */
    void *tmp = NULL;
    size_t l_idx, r_idx, lrg_idx;
    NP_CHECK(q);

    l_idx = LEFT(idx);
    r_idx = RIGHT(idx);

    /* Left child exists, compare left child with its parent */
    if (l_idx < q->size && q->cmp(q->data[l_idx], q->data[idx]) > 0) {
        lrg_idx = l_idx;
    } else {
        lrg_idx = idx;
    }

    /* Right child exists, compare right child with the largest element */
    if (r_idx < q->size && q->cmp(q->data[r_idx], q->data[lrg_idx]) > 0) {
        lrg_idx = r_idx;
    }

    /* At this point largest element was determined */
    if (lrg_idx != idx) {
        /* Swap between the index at the largest element */
        tmp = q->data[lrg_idx];
        q->data[lrg_idx] = q->data[idx];
        q->data[idx] = tmp;
        /* Heapify again */
        pqueue_heapify(q, lrg_idx);
    }
}
Stack newStack()
{
	Stack st = (Stack) malloc (sizeof (struct stack)); 
	st->top=NULL;
	st->size=0; 
	
	return st; 
}

Boolean pushStack (Stack st, int pid1,int pid2)
{
	
	if (st==NULL)
		{
			printf("\n Error-Push: Stack Non Existent");
			return FALSE;
		}
	
	Elem el = (Elem) malloc (sizeof (struct node)); 
	el->next=NULL;
	el->pid1= pid1;
	el->pid2=pid2;
			
			if(st->top==NULL)
			{
				st->top=el;
				st->size=1;
			}
			else {
				el->next=st->top;
				st->top=el;
				st->size++;
			}
			return TRUE; 
	
	
}
Boolean popStack (Stack st, int* pid1,int* pid2)
{
	Elem el=NULL;
	if (st==NULL)
		{
			printf("\n Error-Pop: Stack Non Existent");
			return FALSE;
		}
	if (st->top==NULL)
		{
			printf("\n Error-Pop: Empty Stack");
			return FALSE;
		}
	
	el=st->top;
	*pid1=el->pid1;
	*pid2=el->pid2;
	st->top=el->next;
	free(el);
	st->size--;
	return TRUE; 	
}


Boolean topOfStack (Stack st, int* pid1,int* pid2)
{
	Elem el=NULL;
	if (st==NULL)
		{
			printf("\n Error-Top: Stack Non Existent");
			return FALSE;
		}
	if (st->top==NULL)
		{
			printf("\n Error-Top: Empty Stack");
			return FALSE;
		}
	
	el=st->top;
	*pid1=el->pid1;
	*pid2=el->pid2;
	return TRUE; 	
}




int sizeOfStack(Stack st)
{
if (st==NULL)
		{
			printf("\n Error-Pop: Stack Non Existent");
			return -1;
		}
return st->size; 
}




void displayStack (Stack st)
{

	if (st==NULL)
		{
			printf("\n Error-Display: Stack Non Existent");
			
		}
	else{
		printf("\n Stack-Display : ");
		Elem curr=st->top;
	
		while(curr!=NULL)
		{
			printf("\n %d %d", curr->pid1,curr->pid2);
			curr=curr->next;
		}
		
		printf("\n Stack-Display ends \n");
	
	}
	
}


