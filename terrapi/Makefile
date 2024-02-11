# To force rebuild on these kind of not-file-related targets.
.PHONY: all build install clean

build:
	@echo "Building WiringPi..."
	cd lib/WiringPi && ./build && cd ../..
	@echo "Building terrapi..."
	cmake -B build && make --file=build/Makefile terrapi

install:
	@echo "Installing terrapi..."
	./install.sh

clean:
	@echo "Cleaning terrapi..."
	rm -rf build
