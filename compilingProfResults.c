#include <stdio.h>
#include <string.h>
char *cutTheLastCharacter(char *inclusiveTimePerCall)
{
	int i,len=0;
	for(len=0;inclusiveTimePerCall[len]!='\0';len++);
	char * string=(char *)malloc(len*sizeof(char));
	for(i=0;i<len-1;i++)
	{
		if(inclusiveTimePerCall[i]!='s')
			string[i]=inclusiveTimePerCall[i];
		else 
			break;
	}
	string[i]='\0';
	return string;

}
int main(int argc, char * argv[])
{
   FILE *fp=fopen(argv[1],"r");
   FILE *foutput;
   if(atoi(argv[3])==0)
   	foutput=fopen(argv[2],"w");
   else
   	foutput=fopen(argv[2],"a");
   char *str2=(char*)malloc(sizeof(char)*600), *str1, *token, *subtoken="main\n";
   char *saveptr1, *saveptr2;
   char *totalExclusiveTime, *totalInclusiveTime, *noOfCalls, *exclusiveTimePerCall, *inclusiveTimePerCall,*functionName;
   int j;
   //printf("%s\n",argv[1] );
   fgets(str2,500,fp);
   //printf("%s\n",str1 );
   fgets(str2,500,fp);
   //printf("%s\n",str1 );
   while(!feof(fp))
   {
	   //str2=(char*)malloc(sizeof(char)*600);
	   fgets(str2,500,fp);
	   //printf("%s\n",str2 );
	   	str1=str2;
	       totalExclusiveTime = strtok_r(str1, "      ", &saveptr1);
	       if (totalExclusiveTime == NULL)
	       {
	       	printf("error 1 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("totalExclusiveTime=%s\n",totalExclusiveTime );
	       str1 = NULL;
	       totalInclusiveTime = strtok_r(str1, "      ", &saveptr1);
	       if (totalInclusiveTime == NULL)
	       {
	       	printf("error 2 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("totalInclusiveTime=%s\n",totalInclusiveTime );
	       noOfCalls = strtok_r(str1, "      ", &saveptr1);
	       if (noOfCalls == NULL)
	       {
	       	printf("error 3 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("noOfCalls=%s\n",noOfCalls );
	       exclusiveTimePerCall = strtok_r(str1, "      ", &saveptr1);
	       if (exclusiveTimePerCall == NULL)
	       {
	       	printf("error 4 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("exclusiveTimePerCall=%s\n",exclusiveTimePerCall );
	       inclusiveTimePerCall = strtok_r(str1, "      ", &saveptr1);
	       if (inclusiveTimePerCall == NULL)
	       {
	       	printf("error 5 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("inclusiveTimePerCall=%s\n",inclusiveTimePerCall );
	       functionName = strtok_r(str1, "      ", &saveptr1);
	       if (functionName == NULL)
	       {
	       	printf("error 6 in reading file \n");
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("functionName=%s %d\n",functionName,strcmp(functionName,subtoken) );

	       if(!strcmp(functionName,subtoken))
	       {
	       	
	       	fprintf(foutput,"%s\n",cutTheLastCharacter(inclusiveTimePerCall) );
	       	fclose(fp);
	       	exit(0);
	       }
	       //printf("end of iteration %s\n",functionName );
	       //printf("%d: %s\n", j, token);
	   
   }
}