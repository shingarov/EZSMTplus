
OBJS = literal.o literals.o aggregates.o statements.o output.o

all: $(OBJS)

.cc.o:
	g++-7 -c -std=c++11 -O0 -ggdb -I../.. -o $@ $<

