output: Data.o  Optics.o List.o RTree.o dataDistribution.o file_io.o Main_Optics.o
	vtcc -vt:seq -fopenmp  -o output Data.o  Optics.o List.o RTree.o file_io.o dataDistribution.o Main_Optics.o -lm  
#output: Data.o IOptics.o Optics.o List.o RTree.o Main_rtree.o
#	gcc -g -o output Data.o IOptics.o Optics.o List.o RTree.o Main_rtree.o -lm

#output: Data.o IOptics.o Optics.o List.o RTree.o Main2.o
#	gcc -g -o output Data.o IOptics.o Optics.o List.o RTree.o Main2.o -lm

#output: Data.o IOptics.o Optics.o List.o RTree.o Main2.o
#	gcc -g -o output Data.o IOptics.o Optics.o List.o RTree.o Main2.o -lm

#Main1.o: Main_Batch.c
#	gcc -g -c Main_Batch.c

Main_Optics.o: Main_Optics.c
	vtcc -vt:seq -fopenmp  -c Main_Optics.c 



Data.o: Data.c
	vtcc -vt:seq -fopenmp  -c  Data.c  
	

Optics.o: Optics.c
	vtcc -vt:seq -fopenmp -c Optics.c 
	
List.o: List.c
	vtcc -vt:seq -fopenmp  -c List.c
	 
RTree.o : RTree.c
	vtcc -vt:seq -fopenmp -c RTree.c  
	
dataDistribution.o : dataDistribution.c
	vtcc -vt:seq -fopenmp  -c dataDistribution.c 
	
file_io.o : file_io.c
	vtcc -vt:seq -fopenmp -c file_io.c 
	
clean:
	rm *.o
	rm output
	#rm output1

