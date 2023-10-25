#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Arduino.h>
#include <ringbuf.h>

const long frequency = 433E6;  // LoRa Frequency

const int csPin = 18;          // LoRa radio chip select
const int resetPin = 19;        // LoRa radio reset
const int irqPin = 21;          // change for your board; must be a hardware interrupt pin

RingBuf<uint8_t, 128> myBuffer;

void onReceive(int packetSize) {
	for (int i = 0; i < packetSize; i++)
		myBuffer.pushOverwrite(LoRa.read());
}

void setup() {
	Serial.begin(115200);
	while (!Serial);

	SPI.begin(2, 4, 5);
	LoRa.setPins(csPin, resetPin, irqPin);

	if (!LoRa.begin(frequency)) {
		Serial.println("LoRa init failed. Check your connections.");
		while (true);
	}
	
	LoRa.onReceive(onReceive);

	LoRa.receive();
}

void loop()
{
	if(!myBuffer.isEmpty())
	{
		Serial.println("Received: ");

		uint8_t datum;
		while(myBuffer.lockedPop(datum))
			Serial.print((char)datum);

		Serial.println();
	}
}
