
default: all
all: pmerge

pmerge: pmerge.c
	gcc -fopenmp -o pmerge pmerge.c
	gcc -fopenmp -o pmerge2 pmergev2.c
	ulimit -s unlimited

clean:
	rm -f pmerge
	rm -f pmerge2

go:
	make clean
	make
