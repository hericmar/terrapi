int wiringPiSetup() { return 0; }
void pinMode(int pin, int mode) {}

/// Test mock used by signal sensor.
int digitalRead(int pin) { return 1; }
void digitalWrite(int pin, int value) {}

void delay(unsigned int howLong) {}
void delayMicroseconds(unsigned int howLong) {}
