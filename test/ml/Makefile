OCAMLLIB = `ocamlfind printconf stdlib`
CFLAGS = -Wall -ansi -I${OCAMLLIB}
OCAMLC = ocamlfind ocamlc -custom -package unix -linkpkg -I ../../
OCAMLOPT = ocamlfind ocamlopt -package unix -linkpkg -I ../../
FD_ARCHIVES = fd.cma
FD_XARCHIVES = fd.cmxa

all: bytecode
allopt: native

bytecode:
	${OCAMLC} -o test ${FD_ARCHIVES} test.ml
	${OCAMLC} -o mycat ${FD_ARCHIVES} mycat.ml
	${OCAMLC} -o openfile ${FD_ARCHIVES} openfile.ml
	${OCAMLC} -o fexecve ${FD_ARCHIVES} fexecve.ml

native:
	${OCAMLOPT} -o test ${FD_XARCHIVES} test.ml
	${OCAMLOPT} -o mycat ${FD_XARCHIVES} mycat.ml
	${OCAMLOPT} -o openfile ${FD_XARCHIVES} openfile.ml
	${OCAMLOPT} -o fexecve ${FD_XARCHIVES} fexecve.ml

clean:
	rm -f *.cmi *.cmo *.cmx *.o test mycat openfile fexecve
