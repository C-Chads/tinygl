

all:
#	( for f in $(DIRS); do ( cd $$f ; make all ) || exit 1 ; done )
	cd src && $(MAKE) && cd ..
clean:
	cd src && $(MAKE) clean && cd ..