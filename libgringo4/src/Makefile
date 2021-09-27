all: libgringo.a

libgringo.a: nongroundgrammar.cc groundtermgrammar.cc groundtermlexer.hh nongroundlexer.hh $(OBJS)

grammar: nongroundgrammar.cc groundtermgrammar.cc groundtermlexer.hh nongroundlexer.hh

.cc.o:
	g++-7 -c -std=c++11 -fPIC -O0 -ggdb -I.. -o $@ $<

OBJS = python.o lua.o terms.o term.o scripts.o \
	iliteral.o iliterals.o programbuilder.o statement.o groundtermparser.o \
	aggregate.o program.o iaggregates.o nongroundparser.o \
	nongroundgrammar.o groundtermgrammar.o \
	oliteral.o oliterals.o oaggregates.o ostatements.o output.o \
	instantiation.o gliterals.o gprogram.o statements.o

libgringo.a: $(OBJS)
	ar rc $@ $(OBJS)
	ranlib $@

nongroundgrammar.cc: nongroundgrammar.yy
	bison -r all -o $@ $<

groundtermgrammar.cc: groundtermgrammar.yy
	bison -r all -o $@ $<

groundtermlexer.hh: groundtermlexer.xh
	re2c -o $@ $<

nongroundlexer.hh: nongroundlexer.xh
	re2c -o $@ $<

clean:
	rm -f *.o nongroundgrammar.cc groundtermgrammar.cc groundtermlexer.hh nongroundlexer.hh *.a
