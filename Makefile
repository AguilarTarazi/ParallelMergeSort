CHARMDIR = /media/Pedro/Elementary/Facultad/PPS/Charm++/charm-6.7.1/charm-6.7.1/mpi-linux-x86_64/
CHARMC = $(CHARMDIR)/bin/charmc $(OPTS)


default: all
all: merge


merge : main.o merge.o
	$(CHARMC) -language charm++ -o merge main.o merge.o -tracemode projections

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
