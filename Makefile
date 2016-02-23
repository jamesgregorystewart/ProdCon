###########################################################
#
# Simple Makefile for Operating Systems Project 1
# faulty
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

faulty: faulty.o
	$(CPP) $(CPPOPTS) faulty.o -o faulty

faulty.o: faulty.cpp
	 $(CPP) $(CPPOPTS) -c faulty.cpp

clean:
	$(RM) $(RMOPTS) *.o *~ core faulty
