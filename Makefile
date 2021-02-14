

all: src SDL_Examples
	@echo Done!

src:
	cd src && $(MAKE) && cd ..


SDL_Examples:
	cd SDL_Examples && $(MAKE) && cd ..
clean:
	cd src && $(MAKE) clean && cd ..
	cd SDL_Examples && $(MAKE) clean && cd ..
#clean:
#	rm -f *~ *.o *.a
#	cd SDL_Examples && $(MAKE) clean && cd ..
