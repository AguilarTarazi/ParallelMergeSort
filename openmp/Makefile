
default: all
all: pmerge

pmerge: pmerge.c
	gcc -fopenmp -o pmerge pmerge.c
	ulimit -s unlimited

clean:
	rm -f pmerge
