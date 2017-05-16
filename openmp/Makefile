default: all
all: pmerge

pmerge: main.o merge.o
	gcc -fopenmp -o pmerge pmerge.c
	ulimit -s unlimited

clean:
  rm -f pmerge
