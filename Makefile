terrapi: src/main.cpp
	g++ -Wall src/main.cpp -o terrapi -lwiringPi

clean:
	rm -rf terrapi
