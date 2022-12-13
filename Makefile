TEST_SERVICES= \
	build-drm \

PROD_SERVICES= \
	build-mailserv \

OLD_SERVICES= \
	build-tdrm \
	build-ndrm \

build:
	( for d in \
		${PROD_SERVICES} \
	; do cd $$d ; echo "Building $$d"; make ; cd .. ; done )

build_test:
	( for d in \
		${TEST_SERVICES} \
	; do cd $$d ; echo "Building $$d"; make ; cd .. ; done )

depend:
	( for d in \
		${PROD_SERVICES} \
	; do cd $$d ; make depend ; cd .. ; done )

update:
	( for d in \
		${PROD_SERVICES} \
	; do cd $$d ; make update ; cd .. ; done )

depend_all:
	( for d in \
		${PROD_SERVICES} ${TEST_SERVICES} \
	; do cd $$d ; make depend ; cd .. ; done )

clean:
	( for d in \
		${TEST_SERVICES} ${PROD_SERVICES} \
	; do cd $$d ; make clean ; cd .. ; done )

