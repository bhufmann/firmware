/*
 ******************************************************************************
 *  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#include "application.h"

#include "Serial2/Serial2.h"
#include "MQTT.h"

SYSTEM_MODE(AUTOMATIC);

// ------------
// Blink an LED
// ------------

/*-------------

We've heavily commented this code for you. If you're a pro, feel free to ignore it.

Comments start with two slashes or are blocked off by a slash and a star.
You can read them, but your device can't.
It's like a secret message just for you.

Every program based on Wiring (programming language used by Arduino, and Particle devices) has two essential parts:
setup - runs once at the beginning of your program
loop - runs continuously over and over

You'll see how we use these in a second.

This program will blink an led on and off every second.
It blinks the D7 LED on your Particle device. If you have an LED wired to D0, it will blink that LED as well.

-------------*/


// First, we're going to make some variables.
// This is our "shorthand" that we'll use throughout the program:

int _SER_Pin = D0; // Instead of writing D0 over and over again, we'll write led1
// You'll need to wire an LED to this one to see it blink.

int _SRCLK_Pin = D1; // Instead of writing D7 over and over again, we'll write led2

int _RST_Pin = D2;

// This one is the little blue LED on your board. On the Photon it is next to D7, and on the Core it is next to the USB jack.

// Having declared these variables, let's move on to the setup function.
// The setup function is a standard part of any microcontroller program.
// It runs only once when the device boots up or is reset.

PinState state = HIGH;

int numbers_def[] = {
	0x7E,
	0x0C,
	0xB6,
	0x9E,
	0xCC,
	0xDA,
	0xFA,
	0x0E,
};

void setNumber(byte val) {
	digitalWrite(_SRCLK_Pin, LOW);
	digitalWrite(_RST_Pin, LOW);
	digitalWrite(_RST_Pin, HIGH);
	for (int i = 7; i >= 0; i--) {
		state = ((val >> i) & 0x1) ? HIGH : LOW;

        digitalWrite(_SER_Pin, state);
        digitalWrite(_SRCLK_Pin, HIGH);
        digitalWrite(_SRCLK_Pin, LOW);
	}
	digitalWrite(_SER_Pin, LOW);
    digitalWrite(_SRCLK_Pin, HIGH);
    digitalWrite(_SRCLK_Pin, LOW);
	digitalWrite(_RST_Pin, LOW);
	digitalWrite(_RST_Pin, HIGH);
}

bool player = false;

void reset0() {
	setNumber(0x0);
	delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    if (message.equals("9")) {
    	reset0();
    	player = true;
    } else { int l = message.toInt();
    	Serial.printlnf("connected: %d", l);
    	setNumber(numbers_def[l]);
    	if (player) {
    		delay(1000);
    		player = false;
    	}
    }
}

byte server[] = { 192, 168, 0, 101 };
MQTT client(server, 1883, callback);

void setup() {

  // We are going to tell our device that D0 and D7 (which we named led1 and led2 respectively) are going to be output
  // (That means that we will be sending voltage to them, rather than monitoring voltage that comes from them)

  // It's important you do this here, inside the setup() function rather than outside it or in the loop function.

  pinMode(_SER_Pin, OUTPUT);
  pinMode(_SRCLK_Pin, OUTPUT);
  pinMode(_RST_Pin, OUTPUT);
  pinMode(D7, OUTPUT);

  Serial.begin(9600);
  if (!client.isConnected()) {
	  Serial.printlnf("connected: %d", client.connect("Bernd"));
  }
  client.subscribe("gameserver");

}

// Next we have the loop function, the other essential part of a microcontroller program.
// This routine gets repeated over and over, as quickly as possible and as many times as possible, after the setup function is called.
// Note: Code that blocks for too long (like more than 5 seconds), can make weird things happen (like dropping the network connection).  The built-in delay function shown below safely interleaves required background activity, so arbitrarily long delays can safely be done if you need them.

void loop() {
//	state = HIGH;
//	int val = 0x1;
//	for (int i = 0; i < 8; i++) {
//		setNumber(numbers_def[i]);
//		delay(300);
//	}
//	setNumber(0x7e);

	if (client.isConnected()) {
		client.loop();
	}
}



