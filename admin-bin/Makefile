BIN_DIR=/var/local/etc/httpd/admin-bin/

BIN_SCRIPTS= \
	$(BIN_DIR)filterdetail \
	$(BIN_DIR)procmaillog \
	$(BIN_DIR)procmaillogsum \

usage:
	@echo 'Please use "make install" to install programs and files'

install: $(BIN_SCRIPTS)

$(BIN_SCRIPTS): ./${*F}
	install -c -o bin -g bin ${*F} $(BIN_DIR)
