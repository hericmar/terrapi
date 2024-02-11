# To force rebuild on these kind of not-file-related targets.
.PHONY: all build install clean

build:
	@echo "Building WiringPi..."
	cd lib/WiringPi && ./build && cd ../..
	@echo "Building terrapi..."
	mkdir -p build && cd build && cmake .. && make terrapi

install:
	@echo "Installing terrapi..."
	./install.sh

clean:
	@echo "Cleaning terrapi..."
	rm -rf build
