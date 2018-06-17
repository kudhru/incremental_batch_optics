#include "Def.h"
#include<stdio.h>
void extractClusters(Cluster_order ord)
{    //Cluster_order clustO = getClusterOrder(strFileName);

     Cluster_order clustO = ord;
     Cluster_order tempCO;
     int a=-1, m;
     int nCnt = 0, cCnt = 0;
     Noise newNP, tempN, firstN = NULL;
     Cluster newC, tempC, firstC = NULL;
     ClstHd newHd, tempHd;
     
     //assign cluster IDs
     for(tempCO = clustO; tempCO != NULL; tempCO = tempCO->next)
     {    
      if(tempCO->reachability_distance > EPS1)
          {    
          // printf("\nIndex1 : %d", tempCO->index);
          //      printf("reachability distance : %f", tempCO->reachability_distance);
          //      printf("core distance : %f", tempCO->core_distance);
              
               if(tempCO->core_distance <= EPS1)
		         {    
              // printf("new cluster %d\n",a );
              a= a+1;
		              tempCO->ClstId = a;
                 }
                 else
		         {    tempCO->ClstId = -1;
		         }
           }
           else
           {    tempCO->ClstId = a;
            // printf("\nIndex2 : %d", tempCO->index);
            //    printf("reachability distance : %f", tempCO->reachability_distance);
            //    printf("core distance : %f", tempCO->core_distance);
           }
     }
     
     //view assigned clusterIDs
     /*printf("\nAssigned clusterIDs : ");
     for(tempCO = clustO; tempCO != NULL; tempCO = tempCO->next)
     {     printf("\n%d: %d", tempCO->index, tempCO->ClstId); 
     }*/

     //extract noise
     for(tempCO = clustO; tempCO != NULL; tempCO = tempCO->next)
     {   if(tempCO->ClstId == -1)
	       {      nCnt++;
                  if(nCnt == 1)
                  {   newNP = (Noise)malloc(sizeof(struct noise));
                      newNP->index = tempCO->index;
                      newNP->next = NULL;
                  
                      tempN = newNP;
                      firstN = newNP;
                  }
                  else
                  {   newNP = (Noise)malloc(sizeof(struct noise));
                      newNP->index = tempCO->index;
                      newNP->next = NULL;
                  
                      tempN->next = newNP;
                      tempN = newNP;
                  }
           }
     }
     
     //extract clusters
     //should be there: printf("\nThe number of clusters : %d", a+1);
     
     for(m = 0; m <= a; m++)
     {   cCnt = 0;
         if(m == 0)
         {    newC = (Cluster)malloc(sizeof(struct cluster));
              newC->index = m+1;
              newC->count = cCnt;
              newC->first = NULL;
              newC->next = NULL;
              
              tempC = newC;
              firstC = newC;
         }
         
         else
         {    newC = (Cluster)malloc(sizeof(struct cluster));
              newC->index = m+1;
              newC->count = cCnt;
              newC->first = NULL;
              newC->next = NULL;
              
              tempC->next = newC;
              tempC = newC;     
         }
         for(tempCO = clustO; tempCO != NULL; tempCO = tempCO->next)
         {    if(tempCO->ClstId == m)
              {	  if(newC->count == 0)
                  {   newHd = (ClstHd)malloc(sizeof(struct clstHd));
                      newHd->index = tempCO->index;
                      newHd->next = NULL;
                      
                      newC->first = newHd;
                      tempHd = newHd;
                      
                      newC->count = (newC->count)+1;
                  }
                  else
                  {   newHd = (ClstHd)malloc(sizeof(struct clstHd));
                      newHd->index = tempCO->index;
                      newHd->next = NULL;
                    
                      tempHd->next = newHd;
                      tempHd = newHd;
                      newC->count = (newC->count)+1;
                  }
              }
         }
      }
     
     //view
    //should be there: 
      
     //should be there:
      viewClusters(firstC);
	viewNoise(firstN);
}

Cluster_order getClusterOrder(char *strFileName)
{    
  if(strFileName == NULL)
         return NULL;
     FILE *f1;
     int x,u,w,a;
     float y, z;
     int iCnt = 0;
     
     Cluster_order newClust;
     Cluster_order head, temp;
     
     f1 = fopen(strFileName, "r");
     //printf("The file is : %s", strFileName);
     
     while(!feof(f1))
     {    fscanf(f1, "%d %d %f %f %d %d\n", &x,&u, &y, &z,&w,&a);
          //should be there:
            //printf("\n get cluster order %d %f %f", x, y, z);
          iCnt++;
          if(iCnt == 1)
          {   newClust = (Cluster_order)malloc(sizeof(struct cluster_order));
              newClust->index = x;
              newClust->core_distance = y;
              newClust->reachability_distance = z;
              newClust->next = NULL;
              
              head = newClust;
              temp = newClust;
          }
          
          else
          {   newClust = (Cluster_order)malloc(sizeof(struct cluster_order));
              newClust->index = x;
              newClust->core_distance = y;
              newClust->reachability_distance = z;
              newClust->next = NULL;
              
              temp->next = newClust;
              temp = newClust;
          }        
      }
     fclose(f1);   
     /* should be there:
     printf("\nCluster Info :");
     for(temp = head; temp != NULL; temp = temp->next)
     {    printf("\n%d %f %f",temp->index, temp->core_distance, temp->reachability_distance);
     }*/
     return(head);
}

void viewNoise(Noise noiseHd)
{    Noise tempN; int noisecount=0;
     if(noiseHd == NULL)
          return;
     else
     {  

        printf("\nThe noise points are : ");
         for(tempN = noiseHd; tempN != NULL; tempN = tempN->next)
         {    //printf("%d \t",tempN->index);
              noisecount++;
         }
         printf("the no of points in noise are %d\n", noisecount);  
     } 
}

void viewClusters(Cluster clust)
{    Cluster tempC;
     ClstHd tempHd;
     
     if(clust == NULL)
          return;
     else
     {
     tempC = clust;
     while(tempC != NULL)
     {    printf("\nThe number of points in Cluster %d is : %d", tempC->index, tempC->count);
          tempHd = tempC->first;
          //printf("\nThe data points in Cluster %d are : \n", tempC->index);
          while(tempHd != NULL)
          {   //printf("%d \t", tempHd->index);
              tempHd = tempHd->next;
          }
          tempC = tempC->next;
     }    
     }
}
int main(int argc, char * argv[])
{
  EPS1=atof(argv[2]);
  printf("eps=%f\n",EPS1 );
  //printf("m=%d M=%d eps=%f eps1=%f min=%d un=%d\n",MINENTRIES,MAXENTRIES,EPS,EPS1,MINPOINTS,UNDEFINED );
	Cluster_order ord = getClusterOrder(argv[1]);
	extractClusters(ord);
}
