
%.preparsed : %.ez
	../tools/pre-parser $< > $@

%.ground : %.preparsed
	../tools/gringo $< > $@

all: ex1.ground

clean:
	rm *.preparsed *.ground
