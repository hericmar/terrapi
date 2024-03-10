# To force rebuild on these kind of not-file-related targets.
.PHONY: all build install test clean

build:
	@echo "Building WiringPi..."
	cd lib/WiringPi && ./build && cd ../..
	@echo "Building terrapi..."
	cmake -B build && cmake --build build --target terrapi -j

install:
	@echo "Installing terrapi..."
	./install.sh

test:
	@echo "Testing terrapi..."
	cmake -B build && cmake --build build --target terrapi_test -j
	./target/bin/terrapi_test


clean:
	@echo "Cleaning terrapi..."
	rm -rf build
