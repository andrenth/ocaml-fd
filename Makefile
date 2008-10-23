SOURCES=fd_stubs.c fd.mli fd.ml
RESULT=fd

all: byte-code-library
opt: native-code-library
reallyall: byte-code-library native-code-library
install: libinstall
uninstall: libuninstall

-include OCamlMakefile
