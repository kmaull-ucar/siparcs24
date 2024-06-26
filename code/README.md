# Code

The platform code is split into two: receiver code and transmitter code.

Transmitters are implemented on the Pi via [CircuitPython](https://circuitpython.org) while receivers (at the moment) are Arduino boards.

## Receivers

See the code here for LoRA transceiver: [./PiLoRA](./PiLoRA/LoRA_bonnet_tranciever.py)

## Transmitters

| Vendor | Board | Frequency | Status | Code Link |
| :---: | :---: | :--- | :--- | :--- |
| Sparkfun | ExpLoRAble | 915mhz | implementation does not work with PiLoRA (Adafruit) board | [./PiLoRA](./PiLoRA/LoRA_bonnet_tranciever.py) |
| AdaFruit | ARM Adafruit Feather RP2040 with RFM95 LoRa Radio | 915mhz | in progress | -- |

