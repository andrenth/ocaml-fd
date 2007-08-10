# Adapted from ocaml-event's Makefile.

NAME=fd
OBJECTS=fd.cmo
XOBJECTS=$(OBJECTS:.cmo=.cmx)
C_OBJECTS=fd.o

ARCHIVE=$(NAME).cma
XARCHIVE=$(ARCHIVE:.cma=.cmxa)
CARCHIVE_NAME=fd
CARCHIVE=lib$(CARCHIVE_NAME).a

# Flags for the C compiler.
CFLAGS=-O2 -Wall -ansi

OCAMLC=ocamlc
OCAMLOPT=ocamlopt
OCAMLDEP=ocamldep
OCAMLMKLIB=ocamlmklib 
OCAMLDOC=ocamldoc
OCAMLFIND=ocamlfind

.PHONY: all
all: $(ARCHIVE)
.PHONY: allopt
allopt:  $(XARCHIVE)

depend: *.c *.ml *.mli
	gcc -MM *.c > depend	
	$(OCAMLDEP) *.mli *.ml >> depend

## Library creation
$(CARCHIVE): $(C_OBJECTS)
	$(OCAMLMKLIB) -oc $(CARCHIVE_NAME) $(C_OBJECTS)
$(ARCHIVE): $(CARCHIVE) $(OBJECTS)
	$(OCAMLMKLIB) -o $(NAME) $(OBJECTS) -oc $(CARCHIVE_NAME)
$(XARCHIVE): $(CARCHIVE) $(XOBJECTS)
	$(OCAMLMKLIB) -o $(NAME) $(XOBJECTS) -oc $(CARCHIVE_NAME)

## Installation
.PHONY: install
install: all
	{ test ! -f $(XARCHIVE) || extra="$(XARCHIVE) $(NAME).a"; }; \
	$(OCAMLFIND) install $(NAME) META $(NAME).cmi $(NAME).mli $(ARCHIVE) \
	dll$(CARCHIVE_NAME).so lib$(CARCHIVE_NAME).a $$extra

.PHONY: uninstall
uninstall:
	$(OCAMLFIND) remove $(NAME)

## Documentation
.PHONY: doc
doc::
	mkdir doc/html; cd doc/html; $(OCAMLDOC) -html -I .. ../$(NAME).mli

## Testing
.PHONY: test
test: all
	cd test/ml; make

.PHONY: testopt
testopt: allopt
	cd test/ml; make allopt

## Cleaning up
.PHONY: clean
clean::
	rm -f *~ *.cm* *.o *.a *.so depend doc/*
	cd test/ml; make clean

FORCE:

.SUFFIXES: .ml .mli .cmo .cmi .cmx

.mli.cmi:
	$(OCAMLC) -c $(COMPFLAGS) $<
.ml.cmo:
	$(OCAMLC) -c $(COMPLAGS) -nolabels $<
.ml.cmx:
	$(OCAMLOPT) -c $(COMPFLAGS) -nolabels $<
.c.o:
	$(OCAMLC) -c -ccopt "$(CFLAGS)" $<

-include depend
