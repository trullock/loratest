#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Arduino.h>
#include <Adafruit_GPS.h>

const long frequency = 433E6;  // LoRa Frequency

const int csPin = 26;          // LoRa radio chip select
const int resetPin = 14;        // LoRa radio reset
const int irqPin = 27;          // change for your board; must be a hardware interrupt pin

Adafruit_GPS GPS(&Serial2);

void onTxDone() {
	Serial.println("TxDone");
}

boolean runEvery(unsigned long interval)
{
	static unsigned long previousMillis = 0;
	unsigned long currentMillis = millis();
	if (currentMillis - previousMillis >= interval)
	{
		previousMillis = currentMillis;
		return true;
	}
	return false;
}

void setup() {
	Serial.begin(115200);                   // initialize serial

	Serial2.begin(9600, SERIAL_8N1, 12, 13);
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

	SPI.begin(32, 33, 25);
	LoRa.setPins(csPin, resetPin, irqPin);

	if (!LoRa.begin(frequency)) {
		Serial.println("LoRa init failed. Check your connections.");
		while (true);                       // if failed, do nothing
	}
}

void loop() {

	GPS.read();
	if (GPS.newNMEAreceived()) {
		if (!GPS.parse(GPS.lastNMEA())) 
			return;

		Serial.print("Sending packet: ");

		LoRa.beginPacket();

		LoRa.print("Fix: ");
		LoRa.print((int)GPS.fix);
    	LoRa.print(" quality: ");
		LoRa.println((int)GPS.fixquality);

		if (GPS.fix) {
			LoRa.print("Location: ");
			LoRa.print(GPS.latitude, 4);
			LoRa.print(GPS.lat);
			LoRa.print(", ");
			LoRa.print(GPS.longitude, 4);
			LoRa.println(GPS.lon);
		}

		LoRa.endPacket(true);
	}
}
