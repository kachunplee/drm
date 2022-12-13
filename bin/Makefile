BIN_DIR=/var/local/bin/

BIN_SCRIPTS= \
	$(BIN_DIR)genfilterlog \
	$(BIN_DIR)expandalias.pl \

SU_PROGS= \

usage:
	@echo 'Please use "make install" to install programs and files'

install: $(BIN_SCRIPTS) $(SU_PROGS)

$(BIN_SCRIPTS): ./${*F}
	install -c -o bin -g bin ${*F} $(BIN_DIR)

$(SU_PROGS): ${*F}
	install -c -s -o root -g wheel -m 04555 ${*F} $(BIN_DIR)
