
default: all
all: pmerge

pmerge: pmerge.c pmerge1.c pmergev2.c
	gcc -fopenmp -o pmerge pmerge.c
	gcc -fopenmp -o pmerge1 pmerge1.c
	gcc -fopenmp -o pmerge2 pmergev2.c
	ulimit -s unlimited

clean:
	rm -f pmerge
	rm -f pmerge1
	rm -f pmerge2

go:
	make clean
	make
