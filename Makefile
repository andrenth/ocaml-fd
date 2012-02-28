all:
	@#ocaml setup.ml -build -classic-display
	ocaml setup.ml -build

configure:
	oasis setup && ocaml setup.ml -configure

install:
	ocaml setup.ml -install

clean:
	ocaml setup.ml -clean
