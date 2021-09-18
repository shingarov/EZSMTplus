all: libgringo.a

.cc.o:
	g++-7 -c -std=c++11 -fPIC -O0 -ggdb -I.. -o $@ $<

OBJS = python.o lua.o terms.o term.o scripts.o \
	input/literal.o \
	input/literals.o \
	input/programbuilder.o \
	input/statement.o \
	input/groundtermparser.o \
	input/aggregate.o \
	input/program.o \
	input/aggregates.o \
	input/groundtermgrammar.o \
	input/nongroundgrammar.o \
	input/nongroundparser.o \
	output/literal.o \
	output/literals.o \
	output/aggregates.o \
	output/statements.o \
	output/output.o \
	ground/literals.o \
	ground/program.o \
	ground/instantiation.o \
	ground/statements.o

libgringo.a: $(OBJS)
	ar rc $@ $(OBJS)
	ranlib $@
