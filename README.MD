# VRC-USB Adapter for Raspberry Pi Pico



This project allows you to use your RC Car transmitter and receiver as a USB Gamepad. It is designed for [**VRC Pro**](https://vrcworld.com/), but works with any simulator that supports standard USB joysticks.



It converts the PWM signals from an RC Receiver into a USB HID Joystick device using a **Raspberry Pi Pico (RP2040)**.



This is a port of the original ATmega32U4 project by [kekse23.de](http://kekse23.de). Most of this port was completed by AI.



## Features

* **Plug & Play:** Identifies as "VRC-USB" in Windows.

* **High Performance:** Uses the RP2040's PIO/Interrupt capabilities for low-latency signal reading.

* **4 Channels:** Supports Steering, Throttle, and two AUX channels.

* **No Drivers Needed:** Uses standard HID Gamepad drivers.



## Hardware Required

* **Raspberry Pi Pico/Waveshare RP2040-Zero** (or any RP2040 based board).

* **RC Receiver** (Any standard PWM receiver bound to your transmitter).

* **Male Futaba style connectors** (Required to connect to your receiver).



## Wiring



**⚠️ IMPORTANT:** The Raspberry Pi Pico GPIO pins are **NOT 5V tolerant**. You must power your RC Receiver using the Pico's **3.3V (3V3)** pin. Do **NOT** use the 5V (VBUS) pin to power the receiver, as the 5V signal return can damage the Pico.



**Raspberry Pi Pico**

| Function | RC Receiver | Pico Pin | GPIO Pin |
| :--- | :--- | :--- | :--- |
| **Power** | Red* | 3V3 (OUT) | Pin 36 |
| **Ground** | Black/Brown* | GND | Any GND |
| **Steering** | Signal Ch 1 | Pin 34 | GPIO 28 |
| **Throttle** | Signal Ch 2 | Pin 5 | GPIO 3 |
| Aux 1 | Signal Ch 3 | Pin 6 | GPIO 4 |
| Aux 2 | Signal Ch 4 | Pin 7 | GPIO 5 |


**RP2040-Zero**

| Function | RC Receiver | Pico Pin | GPIO Pin |
| :--- | :--- | :--- | :--- |
| **Power** | Red* | 3V3 (OUT) | Pin 3V3 |
| **Ground** | Black/Brown* | GND | Any GND |
| **Steering** | Signal Ch 1 | Pin 28 | GPIO 28 |
| **Throttle** | Signal Ch 2 | Pin 3 | GPIO 3 |
| Aux 1 | Signal Ch 3 | Pin 4 | GPIO 4 |
| Aux 2 | Signal Ch 4 | Pin 5 | GPIO 5 |

Any Pico/RP2040 should also work as long as you connect to the correct pins. If those pins dont exist on your board, you can amend the .ino file and recompile. Info below.

**Note:** You only need to connect power and ground once, either channel 1 or channel 2 can supply the power/ground. The other channel only needs to be signal.

**Note:** Most modern receivers operate perfectly on 3.3V. If your receiver strictly requires 5V to turn on, you must use a voltage divider or logic level shifter on the signal wires to protect the Pico or use the original project based on a ATmega32U4 which is 5V tolerant.






## Installation



### Option 1: The Easy Way (UF2 File)

1.  Download the `VRC_Pico.uf2` file from the [Releases page](releases/latest).

2.  Hold the **BOOTSEL** button on your Pico while plugging it into your computer.

3.  A drive named `RPI-RP2` will appear.

4.  Drag and drop the `.uf2` file into that drive.

5.  The Pico will reboot and appear as a Game controller named **VRC-USB**.



### Option 2: Compile from Source

If you want to modify pinouts or logic:



1.  Install the **Arduino IDE**.

2.  Add the RP2040 board manager URL in `Preferences`:

&nbsp;   `https://github.com/earlephilhower/arduino-pico/releases/download/global/package\_rp2040\_index.json`

3.  Install **"Raspberry Pi Pico/RP2040"** by Earle F. Philhower via Boards Manager.

4.  Select your board: **Raspberry Pi Pico**.

5.  **Important Settings:**

&nbsp;   * **USB Stack:** Select **"Adafruit TinyUSB"**. May need to install the library first.

6.  Upload the `VRC_Pico.ino` sketch.



## Configuration \& Calibration



### In-Game (VRC Pro)

1.  Open VRC Pro.

2.  Go to **Options -> Control Options**.

3.  Click **Calibrate**.

4.  This will load the Windows Game Controllers screen. Click Properties, then Settings and finally Calibrate

5.  Following the Calibration Wizard to ensure all motion ranges are at 100%, then save.

6.  Once back in VRC Pro, check that ranges use 100%. If any are reversed, you can use the 'reverse steering/throttle' option to correct, or adjust on your transmitter.
