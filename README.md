# Arduino Network Tester Interface

An Arduino-based graphical network tool featuring a TFT touchscreen interface and an Ethernet module. This project provides a handy standalone device to configure network settings, test connectivity (DHCP, Static IP), and perform basic diagnostics like Ping, all controlled via a touch interface.

## Features

- **Touchscreen Interface:** Intuitive menu-driven graphical user interface using an MCUFRIEND TFT display.
- **DHCP & Static IP Configuration:** Easily acquire an IP address automatically from a DHCP server or manually set a Static IP, Netmask, and Gateway using an on-screen numeric keypad.
- **Network Diagnostics (Ping):**
  - Ping external domains (e.g., Google).
  - Ping custom IP addresses.
  - Ping your Gateway.
  - Respond to incoming pings to verify your device's presence on the network.
- **Battery Monitoring:** Reads battery voltage (via analog pin 6) and displays the battery percentage directly on the screen.
- **Reset functionality:** Restart the device directly from the touch menu.

## Hardware Requirements

- **Arduino Board:** Compatible with Arduino Uno or Arduino Mega (depending on your TFT shield).
- **Display:** MCUFRIEND 320x480 TFT Touch Screen Shield.
- **Ethernet Module:** ENC28J60 Ethernet module (SPI connection).
- **Battery Circuit (Optional):** A voltage divider connected to Analog Pin 6 for battery monitoring.

## Software Dependencies

This project requires the following Arduino libraries to compile successfully. You can install them via the Arduino Library Manager or download them from their respective repositories:

- `MCUFRIEND_kbv` - For driving the TFT display.
- `Adafruit_GFX` - Core graphics library used by the display.
- `TouchScreen` - For reading touch coordinates from the TFT shield.
- `EtherCard` - For handling the ENC28J60 Ethernet module.

## File Structure

- **`InterfacciaV2.1.ino`**: The main Arduino sketch containing `setup()` and `loop()` functions. It orchestrates the flow of the application between the menu system and network events.
- **`DisplayModule.ino`**: Contains all the logic and graphical elements for the TFT touchscreen. It handles menu rendering, the numeric keypad, touch interactions, and battery monitoring.
- **`EthernetModule.ino`**: Encapsulates the network functionality using the EtherCard library. Manages DHCP setup, static IP configuration, and the ping/ICMP logic.

## Installation and Usage

1. **Wiring:**
   - Attach the TFT Touch Screen Shield to your Arduino.
   - Connect the ENC28J60 Ethernet module to the Arduino's SPI pins (usually pins 10, 11, 12, 13 on an Uno, with pin 10 as SS/CS. *Note: Modify the CS pin in `EthernetModule.ino` if you use a different pin setup*).
   - If utilizing the battery monitor, connect the output of your voltage divider to `A6`.

2. **Compilation:**
   - Open `InterfacciaV2.1.ino` in the Arduino IDE.
   - Make sure all required libraries are installed.
   - Select your specific Arduino board and port.
   - Compile and Upload the sketch to your board.

3. **Using the Device:**
   - Upon startup, the device will display the main menu.
   - **DHCP:** Tap to automatically obtain an IP address.
   - **STATIC:** Tap to use statically defined IP settings.
   - **SET IP:** Open the numeric keypad to define a Static IP, Netmask, Gateway, or Custom Ping IP.
   - **PING / GOOGLE / GW:** Once an IP is acquired, you can test connectivity to a custom IP, Google, or your Gateway.

## Customization

- **Battery Voltage Calibration:** In `DisplayModule.ino`, the `readVolt()` function maps the analog input. You may need to tweak the mapping values or resistor values (`R1`, `R2`) depending on your exact voltage divider circuit.
- **MAC Address:** The MAC address for the Ethernet module is defined in `EthernetModule.ino` as `mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x32}`. You can change this if you are using multiple devices on the same network.
