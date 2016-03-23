// For use with the ControlEverything.com relay board (or other board that uses

// the MCP23008 I2C expander)

// NOTE:  You will need to also import the MCP23008-I2C library as well.

#include "relay-lib.h"
#include "MCP23008-I2C/MCP23008-I2C.h"

Adafruit_MCP23008 mcp;
RelayLib relay[5]; // init should always be called in setup() to set pinMode

int  relayControl(String command);

void mcpDigitalWrite(pin_t p, uint8_t d) {
  mcp.digitalWrite(p, d);
}

// void mcpPinMode(pin_t p, PinMode mode) {
//     mcp.pinMode(p, mode);
// }

void setup()
{
  mcp.begin(1);

  for (int i = 0; i < 4; i++) {
    // call with either a normal function, or a lambda wrapper around a class
    // member function
    // relay[i].init(mcpDigitalWrite, mcpPinMode, i);
    relay[i].init(mcpDigitalWrite,
                  [&mcp](pin_t p, PinMode m) {
      mcp.pinMode(p, m);
    },
                  i);
  }
  relay[4].init(D7); // using onboard GPIO
  Particle.function("relay", relayControl);
}

void loop()
{}

// command format r1,ON or r1,OFF or r1,SWITCH or r1,PULSE
int relayControl(String command)
{
  // parse the relay number
  int relayNumber = command.charAt(1) - '0';

  // do a sanity check
  if ((relayNumber < 1) || (relayNumber > 5)) return -1;

  // find out the state of the relay
  if (command.substring(3, 5) == "ON") relay[relayNumber - 1].on();
  else if (command.substring(3, 6) == "OFF") relay[relayNumber - 1].off();
  else if (command.substring(3, 9) == "SWITCH")  relay[relayNumber - 1].toggle();
  else if (command.substring(3, 9) == "PULSE")  relay[relayNumber - 1].pulse();
  else return -1;

  return 1;
}
