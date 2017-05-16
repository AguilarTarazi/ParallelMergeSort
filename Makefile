CHARMDIR = /home/mauri/pps/charm-6.7.1
CHARMC = $(CHARMDIR)/bin/charmc $(OPTS)


default: all
all: merge

merge : main.o merge.o
	$(CHARMC) -language charm++ -o merge main.o merge.o -tracemode projections
	ulimit -s unlimited

main.o : main.C main.h main.decl.h main.def.h merge.decl.h
	$(CHARMC) -o main.o main.C -O3

main.decl.h main.def.h : main.ci
	$(CHARMC) main.ci

main.h : merge.decl.h

merge.o : merge.C merge.h merge.decl.h merge.def.h main.decl.h
	$(CHARMC) -o merge.o merge.C -O3

merge.decl.h merge.def.h : merge.ci
	$(CHARMC) merge.ci


clean:
	rm -f main.decl.h main.def.h main.o
	rm -f merge.decl.h merge.def.h merge.o
	rm -f merge charmrun

cleanall:
		rm -f main.decl.h main.def.h main.o
		rm -f merge.decl.h merge.def.h merge.o
		rm -f merge charmrun
		rm -f *.log
		rm -f tachyonShifts.dat
		rm -f *.projrc
		rm -f .fuse_hidden*
		rm -f *.sts
		rm -f *.topo
		rm -f *.fuse*
		rm -f *cachegrind*

go:
		make clean
		make
