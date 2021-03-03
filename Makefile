
LIB = lib/libTinyGL.a

all: $(LIB) SDL_Examples
	@echo Done!

$(LIB):
	cd src && $(MAKE) && cd ..
	cp src/*.a ./lib/


SDL_Examples: $(LIB)
	cd SDL_Examples && $(MAKE) && cd ..
	cd Raw_Demos && $(MAKE) && cd ..
clean:
	cd src && $(MAKE) clean && cd ..
	cd SDL_Examples && $(MAKE) clean && cd ..
	cd Raw_Demos && $(MAKE) clean && cd ..
	cd lib && rm -f *.a && cd ..
#clean:
#	rm -f *~ *.o *.a
#	cd SDL_Examples && $(MAKE) clean && cd ..
