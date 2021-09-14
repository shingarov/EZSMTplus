

OBJS = literal.o literals.o programbuilder.o statement.o groundtermparser.o aggregate.o program.o aggregates.o nongroundparser.o \
	nongroundgrammar.o groundtermgrammar.o


all: nongroundgrammar.cc groundtermgrammar.cc groundtermlexer.hh nongroundlexer.hh $(OBJS)

.cc.o:
	g++-7 -c -std=c++11 -O0 -ggdb -I../.. -o $@ $<


nongroundgrammar.cc: nongroundgrammar.yy
	bison -r all -o $@ $<

groundtermgrammar.cc: groundtermgrammar.yy
	bison -r all -o $@ $<

groundtermlexer.hh: groundtermlexer.xh
	re2c -o $@ $<

nongroundlexer.hh: nongroundlexer.xh
	re2c -o $@ $<

clean:
	rm *.o nongroundgrammar.cc groundtermgrammar.cc groundtermlexer.hh nongroundlexer.hh
