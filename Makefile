AR = /usr/bin/ar
CC = gcc
CPP = @CPP@
CXX = g++
CXXCPP = @CXXCPP@
RANLIB = ranlib

CPPFLAGS = 
LDFLAGS = 
LIBS = 
CXXFLAGS = -g -O0 -fpermissive
CFLAGS = -g -O0

MFLAGS = 

ifeq (solaris, $(OSTYPE))
  MFLAGS = -D_NEED_REDEFINE_RAND_MAX_
endif

#RANLIB = ranlib
#AR = ar

.SUFFIXES: .o .cc 

HEADERS = Solver.h SimpSolver.h SolverTypes.h Vec.h Queue.h Alg.h BasicHeap.h BoxedVec.h Map.h Heap.h Sort.h \
stack.h  atomrule.h read.h graphscc.h\
 timer.h  program.h api.h ctable.h \
tree.h  simo.h cmodels.h interpret.h param.h

SOLVER_SRCS = main.cc 
SOLVER_OBJS = $(SOLVER_SRCS:.cc=.o)

LIB_SRCS =  stack.cc  atomrule.cc read.cc \
timer.cc  program.cc api.cc ctable.cc \
tree.cc  simo.cc graphscc.cc cmodels.cc interpret.cc

LIB_OBJS = $(LIB_SRCS:.cc=.o)


cmodels:   $(SOLVER_OBJS) libsat.a
	  $(CXX) $(LINKFLAGS) $(CFLAGS) $(MFLAGS) $(SOLVER_OBJS) libsat.a -o ezsmtPlus

zverify_bf: zverify_bf.cc	
	  $(CXX) $(LINKFLAGS) $(CFLAGS) $(MFLAGS) zverify_bf.cc -o zverify_bf

zverify_df: zverify_df.cc
	  $(CXX) $(LINKFLAGS) $(CFLAGS) $(MFLAGS) zverify_df.cc -o zverify_df

zcore: zcore_extract.cc
	  $(CXX) $(LINKFLAGS) $(CFLAGS) $(MFLAGS) zcore_extract.cc -o zcore

cnf_stats: cnf_stats.cc
	  $(CXX) $(LINKFLAGS) $(CFLAGS) $(MFLAGS) cnf_stats.cc -o cnf_stats

$(LIB_OBJS): $(HEADERS) Makefile

libsat.a:   $(LIB_OBJS)
	@rm -f libsat.a
	$(AR) cr libsat.a $(LIB_OBJS)
	$(RANLIB) libsat.a

#.cc.o:
#	$(CC) $(CFLAGS) $(MFLAGS) -c  $< 

clean:	
	rm -f *.o libsat.a zcore cnf_stats Model* dimacs-completion*.out SMT*

all: cmodels 
	 	  

d: ezsmtPlus
	vim -c "Termdebug ./ezsmtPlus" -c "Break main" -c 'Run < INPUT.ground'

dt: ezsmtPlus
	vim -c "Termdebug ./ezsmtPlus" -c "Break main" -c 'Run < TIGHT.ground'

