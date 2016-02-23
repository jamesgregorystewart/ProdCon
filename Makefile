###########################################################
#
# Simple Makefile for Operating Systems Project 3
# osp3
#
###########################################################
.SUFFIXES: .h .c .cpp .l .o

CPP = g++
CCOPTS =
CPPOPTS =
LEX = flex
LIBS = -ll		#lfl on linux
RM = /bin/rm
RMOPTS = -f

osp3: osp3.o
	$(CPP) $(CPPOPTS) osp3.o -o osp3

osp3.o: osp3.cpp
	 $(CPP) $(CPPOPTS) -c osp3.cpp

clean:
	$(RM) $(RMOPTS) *.o *~ core osp3
