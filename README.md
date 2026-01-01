# NeoBalsamFir 🎄

**NeoBalsamFir** is a USB-C powered, daisy-chainable LED board shaped like a Christmas tree, built around an **STM32G0B1** microcontroller and **WS2812B (NeoPixel)** LEDs.

Each tree features individually addressable RGB LEDs and can be linked together to form larger animated displays using a simple 3-wire interface.

## Features

* 🎄 Christmas tree–shaped PCB
* 🌈 Individually addressable WS2812B LEDs
* 🔗 Daisy-chainable (Power, Ground, Data)
* 🔌 USB-C input (5 V from any USB power brick or battery bank)
* 🎛 On-board power and toggle button to cycle LED modes

## Architecture

* **Host Tree**

  * Contains the STM32G0B1 microcontroller
  * Handles LED animation generation
  * Includes cycle and power off buttons.
  * Drives the LED data line for the entire chain

* **Follower Trees**

  * No microcontroller required
  * Pass-through boards with LEDs only
  * Connect to **5 V**, **GND**, and **Data**
  * Receive LED data from the host tree

Only the **first tree in the chain** requires a microcontroller. All additional trees operate as passive LED extensions.

## Power

* Input: **USB-C, 5 V**
* Compatible with standard USB power adapters and battery banks
* Power is distributed through the daisy chain

## Controls


* Right Button:

  * Cycle through LED animation modes
  * Toggle effects or patterns

* Left Button:

  * Toggle the microcontrollers power

## Status

This project is a fun, embedded-hardware LED display and firmware project used to learn about the WS2812B. 
No major updates are currently planned.
Feel free to copy/derive to build your own!

## Images

<img width="1484" height="1029" alt="image" src="https://github.com/user-attachments/assets/7d4d65c1-ca0a-4ac2-b741-347d83b0753a" />

<img width="1298" height="1100" alt="image" src="https://github.com/user-attachments/assets/b34a99af-436b-4283-8309-373ec52fba22" />

<img width="1298" height="1100" alt="image" src="https://github.com/user-attachments/assets/df301a68-5d8f-4115-9faa-1bd8d9876c93" />

<img width="1752" height="436" alt="image" src="https://github.com/user-attachments/assets/6365780e-bc3c-481c-ad91-d379a421a00f" />


## Schematic

<img width="1810" height="1300" alt="image" src="https://github.com/user-attachments/assets/2a65de29-82e1-42ae-bfea-0bb127cab5cc" />
