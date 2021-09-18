CXX = g++-7

CXXFLAGS = -ggdb -O0 -std=c++11 -fpermissive -fPIC -Ilibgringo4

HEADERS = stack.h  atomrule.h graphscc.h\
 program.h api.h \
 tree.h cmodels.h interpret.h param.h

LIB_SRCS =  stack.cc  atomrule.cc entry.cc \
program.cc api.cc \
tree.cc graphscc.cc cmodels.cc interpret.cc

LIB_OBJS = $(LIB_SRCS:.cc=.o)

libezsmt.so:   $(LIB_OBJS)
	$(CXX) $(CFLAGS) -shared -fPIC -o $@ $(LIB_OBJS) -Llibgringo4/src -lgringo

$(LIB_OBJS): $(HEADERS) Makefile

clean:
	rm -f *.o *.so example_main

example_main: example_main.c libezsmt.so
	gcc -o example_main -O0 -ggdb example_main.c -L. -lezsmt

d: example_main
	vim -c "Termdebug ./example_main" -c "Break main" -c 'Run'
