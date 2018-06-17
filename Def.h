#ifndef __DEF_H
#define __DEF_H
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<assert.h>
#include<omp.h>
//#define PRINT 1
#define LEFT(x) (2 * (x) + 1)
#define RIGHT(x) (2 * (x) + 2)
#define PARENT(x) ((x-1) / 2)
double EPS;
double EPS1;
int MINPOINTS;
int UNDEFINED;
int DIMENSION;		//value for the number of dimensions
int MINENTRIES;  //Minimum entries for node in RTree
int MAXENTRIES;        //Maximum entries for node in RTree
long int nodesVisited;
long long int nodesVisitedInRtree;
int neighborhoodQueryCountInTree1;
int neighborhoodQueryCountInTree2;
char* originalDataFile;
int noOfPartitions;
int windowSize;
typedef struct hdrNd *HdrNd;	// Head Node for a list of children of RTree
typedef struct lstNd *LstNd;	// Nodes in the list of children of RTree node
typedef struct data *Data;	// Structure holding the data of each element
typedef struct orderedDataNd * OrderedDataNd;
typedef struct dataHdr *DataHdr;// Head node for the list of data elements
typedef struct pointHdr *PointHdr;
typedef double *DataPoint;		// type of pointer to data point
typedef double dataPoint;		// type of data point
typedef struct region *Region;	// type of pointer to structure for rectangle
typedef union treeData *TreeData;	// type of pointer to Data in tree node
typedef struct treeNode *TreeNode;	// type of pointer to tree node
typedef double *Dimension;	//type of pointer to one corner of rectangle
typedef double dimension;	//type of corner of rectangle
typedef double *Double;	//type of pointer to double
typedef int RecNum;
typedef struct nbHdr *NbHdr;
typedef struct nb *NB; 
typedef struct cluster_order *Cluster_order;
typedef struct noise *Noise;
typedef struct cluster *Cluster;
typedef struct clstHd *ClstHd;
typedef struct node* Elem; 
typedef struct stack *Stack; 
typedef struct successor_node * SuccessorNode; 
typedef struct successor_list_header * SuccessorListHeader; 

struct node
{
  int pid1;
  int pid2;
  Elem next;
};
struct stack
{
  Elem top;
  int size; 
};

typedef enum{
    INTNODE,
	EXTNODE
}NODETYPE;	// used to mark a node of RTree as internal or external

typedef enum{
	FALSE,			// 0
	TRUE			// 1
} Boolean;		// used for declaring boolean variables

struct nb{
       int n;
       int timestamp;
       double dist;
       NB nbNext;
};

struct nbHdr{
       int nbhCnt;
       NB nbFirst;
       NB nbLast;
};

struct successor_node
{
  int iNum;
  SuccessorNode next;
};

struct successor_list_header
{
  int successorCount;
  SuccessorNode First;
};

struct data
{
	int timestamp;	//added by RBK..
  int globalTimestamp;
	DataPoint iData;	// data point
	double core_distance;
	double reachability_distance;
	RecNum iNum;
  RecNum iNumNew;
	Boolean isProcessed; //Added by Mayank and Aman
	Boolean core_tag;
	Boolean affected_tag;
	RecNum iNumR; 
	RecNum iNumRef;
  RecNum iNumRNew; 
  RecNum iNumRefNew;
  int fixed;
  int addedToPQ;
  NbHdr neighbors;
  int clusterId;
  int addedToNeighborHoodIndex;//added by Dhruv Kumar 21 april 2013
  omp_lock_t lock;// added by Dhruv Kumar 23 may 2013
  SuccessorListHeader sListOld;
  Boolean toBeDeleted;
  Boolean rdUndef;
};			// structure holding the information about data point

struct orderedDataNd{
    int iNum;
    int timestamp;
    int oldiNum;
  OrderedDataNd dataNextNd;  // pointer to next data node
};


struct region{
    Dimension iBottomLeft;	// bottom left corner of rectangle
	Dimension iTopRight;	// top right corner of rectanngle
};

union treeData{
    Region rgnRect;		// pointer to rectangle incase of internal node
	Data dataClstElem;	//pointer to a Data in case of external node
};

struct treeNode{
    NODETYPE ndType;	//type of tree node (internal or external)
	TreeData tdInfo;	//pointer to treedata structure
};

struct pointHdr{	
	unsigned int uiCnt;
	OrderedDataNd dataFirstNd;
	OrderedDataNd dataLastNd;
};
struct dataHdr{			//header for data list
	unsigned int uiCnt;
	Data dataClstElem;
};

struct hdrNd{			//node in data list
	unsigned int uiCnt;	// Number of elements in the list
	LstNd ptrFirstNd;	// First node of the list
	LstNd ptrParentNd;	// Parent node of the list
};

struct lstNd{			//node of child list
	TreeNode tnInfo;	// Data
	HdrNd ptrChildLst;	// List of child nodes
	LstNd ptrNextNd;	// Next node in the list
};
typedef struct mylist *myList;
struct mylist {
  LstNd listNode;
  myList next;
};
struct cluster_order{
       int index;
       int ClstId;
       double core_distance;
       double reachability_distance;
       Cluster_order next;
       int timestamp;
       RecNum iNumR; 
       RecNum iNumRef;
};

struct noise{
        int index;
        Noise next;
	int timestamp;
};

struct cluster{
        int index;
        int count;
        ClstHd first;
        Cluster next;
	int timestamp;
}; 

struct clstHd{
        int index;
	int timestamp;
        ClstHd next;
};

#endif
#ifndef __PQUEUE__H__
#define __PQUEUE__H__

/**
* Debugging macro .
*
* Checks for a NULL pointer, and prints the error message, source file and
* line via 'stderr' .
* If the check fails the program exits with error code (-1) .
*/
#define NP_CHECK(ptr) \
    { \
        if (NULL == (ptr)) { \
            fprintf(stderr, "%s:%d NULL POINTER: %s n", \
                __FILE__, __LINE__, #ptr); \
            exit(-1); \
        } \
    } \

#define DEBUG(msg) fprintf(stderr, "%s:%d %s", __FILE__, __LINE__, (msg))

/**
* Priority Queue Structure
*/
typedef struct PQueue_s {
    /* The actual size of heap at a certain time */
    size_t size;
    /* The amount of allocated memory for the heap */
    size_t capacity;
    /* An array of (void*), the actual max-heap */
    void **data;
    /* A pointer to a comparator function, used to prioritize elements */
    int (*cmp)(Data, Data);
} PQueue;

/** Allocates memory for a new Priority Queue .
Needs a pointer to a comparator function, thus establishing priorities .
*/
PQueue *pqueue_new(int (*cmp)(Data, Data),
                   size_t capacity);

/** De-allocates memory for a given Priority Queue */
void pqueue_delete(PQueue *q);

/** Add an element inside the Priority Queue */
void pqueue_enqueue(PQueue *q, const void *data);

/** Removes the element with the greatest priority from within the Queue */
void *pqueue_dequeue(PQueue *q);

// ordinary queue data structure by Dhruv Kumar 24 april 2012
typedef struct queue * Queue;
struct queue
{
  int iNum;
  Queue next;
};
struct queue_header
{
  int size;
  Queue first;
  Queue last;
};
typedef struct queue_header * QueueHeader;

#endif

























