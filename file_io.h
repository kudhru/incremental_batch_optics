#ifndef __FILE_IO_H
#define __FILE_IO_H
#include "Def.h"
#include "Data.h"
void  file_read(  char *filename,      /* input file name */
                  int  *numObjs,       /* no. data objects (local) */
                  int  *numCoords,double*** objects) ;    /* no. coordinates */
DataHdr readData(char *strFileName);
DataHdr readDataModified(char *strFileName,int dataList2_Size);
#endif