# SiPaRCS 2024 Research



## Project Description: Mesonet Weather Monitoring System

This project aims to develop a mesonet of weather monitoring sensors that collect various environmental data, including temperature, humidity, altitude, air quality, UV levels, and rain data. The collected data will be transmitted via a LoRa (Long Range) network to intermediary Raspberry Pi gateways. These Raspberry Pi devices will then forward the aggregated data over the internet to a network server, which distributes the information to application servers for further processing and analysis.

#### Key Components:
1. **Sensor Devices:** These devices measure and collect weather data. Each sensor node is equipped with LoRa communication capabilities to transmit data wirelessly.
2. **Raspberry Pi Gateways:** The Raspberry Pi devices act as local gateways, receiving data from multiple sensor devices via the LoRa network. They then transmit this data over the internet to central servers.
3. **Network Server:** This server acts as a central hub, receiving data from various Raspberry Pi gateways and distributing it to multiple application servers.
4. **Application Servers:** These servers process the weather data for various applications, including weather modeling and forecasting, real-time alert systems, and environmental monitoring.
5. **Join Server:** This server handles the network join requests from the sensor devices, ensuring secure and authenticated communication.

#### Use Cases:
1. **Weather Modeling:** The collected data can be used to create detailed weather models that help in predicting local weather patterns with greater accuracy.
2. **Real-Time Alert Systems:** The system can generate real-time alerts for severe weather conditions such as storms, heavy rainfall, or extreme temperatures, helping communities to prepare and respond promptly.
3. **Environmental Monitoring:** Continuous monitoring of air quality, UV levels, and other environmental factors can help in assessing the impact of pollution and climate change, facilitating environmental protection efforts.
4. **Agricultural Applications:** Farmers can use the data to optimize irrigation schedules, protect crops from adverse weather conditions, and improve overall agricultural productivity.
5. **Urban Planning:** City planners can utilize the data for designing resilient infrastructure that can withstand local weather conditions and mitigate the impact of natural disasters.

-   [This project is an extension of the existing OpenIoTwx project
    (https://ncar.github.io/openiotwx/)](https://ncar.github.io/openiotwx/)

![LoRA Firenet](images/LoRA_firenet.png)


# Research Questions

LoRa is a low cost efficient addition to the openIoTwx platform which can be implemented with minimal effort.  **We acknowledge** that there are a number of excellent IoT platforms for deploying and managing IoT devices such [ResIoT](https://www.resiot.io/en/home/), [KaaIoT](https://kaaiot.com), [The Things Network](https://thethingsnetwork.org), [ThingsBoard](https://thingsboard.io), [ThingsSpeak](https://thingspeak.com), among others, as well as LoRa networks such as [LoRa Alliance](https://lora-alliance.org), [Azure IoT](), [SenseCAP](), [Meshtastic](https://meshtastic.org) and others.  

The primary goal here is to _develop a locally-operated network of LoRa IoT devices to experiment and understand costs, design details, implementation details and use cases_.

Can we do some remote inference of ML models on th Pis?


|  RQ |   Summary | Notebook |
| :---: | ----------|----------|
| 1 | How reliable is the data collected with these low cost devices compared to standard ones? | [./notebooks/00_extract.ipynb](./notebooks/00_extract.ipynb) |
| 2 | Are the data communications between the weather stations done reliably? | [./notebooks/01_explore.ipynb](./notebooks/01_explore.ipynb) |
| 3 | How reliable is the data collected with these low cost devices compared to standard ones? | [./notebooks/00_extract.ipynb](./notebooks/00_extract.ipynb) |
| 2 | Can we do some remote inference of ML models on th Pis?  | [./notebooks/01_explore.ipynb](./notebooks/01_explore.ipynb) |

# Code

The source code for the different implementations can be found as follows:

## Transceivers

| Vendor | Board | Frequency | Status | Code Link |
| :---: | :---: | :--- | :--- | :--- |
| AdaFruit | Adafruit LoRa Radio Bonnet with OLED - RFM95W | 915mhz (GPIO Hat) | working | This folder contains source code for PiLoRA tranceiver: [./code/PiLoRA/](./code/PiLoRA/) |

## Receivers

| Vendor | Board | Frequency | Status | Code Link |
| :---: | :---: | :--- | :--- | :--- |
| Sparkfun | ExpLoRAble | 915mhz | implementation **does not work with PiLoRA** (Adafruit) board | [./code/Sparkfun_ExpLoRAble](./code/Sparkfun_ExpLoRAble) |
| AdaFruit | ARM Adafruit Feather RP2040 with RFM95 LoRa Radio | 915mhz | in progress | -- |

<!-- # Data 

Data to be compared with the NEON station


|  Data |   Summary | Location |
| :---: | ----------|----------|
| EPA source | This folder contains EPA source data. | [./data/epa/](./data/epa) | -->

# Hardware 

| Type | Device/Instrument | Purpose | Setup Information |
| :-- | :-- | :-- | :-- |
| SBC | [Raspberry Pi 3 Model B](https://www.raspberrypi.com/products/raspberry-pi-3-model-b/) | provides main  hardware and OS platform for tranceiver | -- | 
| Sensor  | [Adafruit Qwiic BME 680](https://www.adafruit.com/product/3660) | Atmospheric measurements | -- |
| MCU | [Adafruit Feather RP2040 LoRA board](https://learn.adafruit.com/adafruit-feather-rp2040-pico/overview) | transmit data at 915mhz over LoRA | -- |
| MCU | [Sparkfun ExpLoRaBLE WRL17506 Thing Plus LoRA Board](https://www.sparkfun.com/products/17506) | transmit data at 915mhz over LoRA | -- |


# PiLoRa Setup and Installations

Follow these steps to set up and install the necessary components for your PiLoRa project:

1. **Create a Virtual Environment:**
   - On your Raspberry Pi with the bonnet connected, create a virtual environment named `adafruit`:
     ```
     python -m venv adafruit
     ```

2. **Install Required Libraries:**
   - Install the necessary libraries using the following commands:
     ```
     pip3 install adafruit-circuitpython-ssd1306
     pip3 install adafruit-circuitpython-framebuf
     pip3 install adafruit-circuitpython-rfm69
     ```

3. **Activate the Virtual Environment:**
   - Navigate to your virtual environment's bin directory and activate it:
     ```
     cd adafruit/bin
     source activate
     ```

4. **Enable SPI:**
   - Enable SPI by executing the Raspberry Pi configuration tool:
     ```
     sudo raspi-config
     ```
   - Navigate to `Interface Options` and enable SPI.

5. **Run the Transceiver Script:**
   - Execute the Python script to see your Raspberry Pi with the bonnet act as a transceiver for LoRa packets:
     ```
     python3 LoRA_bonnet_transceiver.py
     ```




# Manuscript

The manuscript for this project can be found:

* (pdf) draft [./manuscript/manuscript.pdf](./manuscript/manuscript.pdf)

# Citing This Work

If you use this work, please cite it using the following citation:

> Hossain, A.m et al SiPARCS Research Repository. [DOI coming soon]

# License

CC-BY-4.0

&copy; Abrar Hossain, Keith E. Maull, Agbeli Ameko
