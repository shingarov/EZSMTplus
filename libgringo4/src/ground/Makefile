
OBJS = instantiation.o literals.o program.o statements.o

all: $(OBJS)

.cc.o:
	g++-7 -c -std=c++11 -fPIC -O0 -ggdb -I../.. -o $@ $<

