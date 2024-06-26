# SiPaRCS 2024 Research

2024 Research.

# Project Introduction

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod
tempor incididunt ut labore et dolore magna aliqua. Est ante in nibh
mauris cursus mattis molestie a iaculis. Feugiat pretium nibh ipsum
consequat nisl vel. Sed adipiscing diam donec adipiscing tristique risus
nec. Sit amet risus nullam eget felis eget nunc lobortis mattis. Lectus
nulla at volutpat diam. Morbi tincidunt ornare massa eget. Sit amet
volutpat consequat mauris. Tempor orci eu lobortis elementum nibh tellus
molestie. Montes nascetur ridiculus mus mauris vitae ultricies leo. Quam
adipiscing vitae proin sagittis nisl rhoncus mattis rhoncus. Placerat in
egestas erat imperdiet sed. Ultricies tristique nulla aliquet enim
tortor at auctor urna.


Integer malesuada nunc vel risus commodo viverra maecenas. Fermentum
posuere urna nec tincidunt praesent semper. Massa sed elementum tempus
egestas sed sed risus. Tortor at auctor urna nunc id cursus metus
aliquam. Sit amet dictum sit amet justo donec. Risus nullam eget felis
eget nunc lobortis mattis aliquam. Ornare arcu odio ut sem nulla
pharetra. Leo duis ut diam quam. Elit sed vulputate mi sit amet mauris
commodo quis. Et sollicitudin ac orci phasellus egestas tellus rutrum
tellus. Gravida rutrum quisque non tellus orci ac auctor. Tincidunt dui
ut ornare lectus sit amet est placerat. Mauris in aliquam sem fringilla
ut morbi tincidunt augue. Volutpat sed cras ornare arcu dui vivamus.

-   [a rando link
    (https://loremipsum.io/generator/?n=5&t=p)](https://loremipsum.io/generator/?n=5&t=p)

![LoRA Firenet](images/LoRA_firenet.png)



# Research Questions

LoRa is a low cost efficient addition to the openIoTwx platform which can be implemented with minimal effort
Can we do some remote inference of ML models on th Pis?


|  RQ |   Summary | Notebook |
| :---: | ----------|----------|
| 1 | How reliable is the data collected with these low cost devices compared to standard ones? | [./notebooks/00_extract.ipynb](./notebooks/00_extract.ipynb) |
| 2 | Are the data communications between the weather stations done reliably? | [./notebooks/01_explore.ipynb](./notebooks/01_explore.ipynb) |
| 3 | How reliable is the data collected with these low cost devices compared to standard ones? | [./notebooks/00_extract.ipynb](./notebooks/00_extract.ipynb) |
| 2 | Can we do some remote inference of ML models on th Pis?  | [./notebooks/01_explore.ipynb](./notebooks/01_explore.ipynb) |

# Code

The source code for the different implementations can be found as follows:

|  Data |   Summary | Location |
| :---: | ----------|----------|
| PiLoRa | This folder contains source code for PiLoRA tranceiver | [./cpde/](./code/PiLoRa) |
| Sparkfun | This folder contains source code for PiLoRA tranceiver | [./cpde/](./code/Sprakfun Explorable) |

# Data 


Data to be compared with the NEON station


|  Data |   Summary | Location |
| :---: | ----------|----------|
| EPA source | This folder contains EPA source data. | [./data/epa/](./data/epa) |

# Devices
1. [Raspberry Pi 3 Model B
    (https://www.raspberrypi.com/products/raspberry-pi-3-model-b/)](https://www.raspberrypi.com/products/raspberry-pi-3-model-b/) 
2. [Adafruit Radio Bonnets with OLED - RFM69 or LoRa - RadioFruit
    (https://www.adafruit.com/product/4087)](https://www.adafruit.com/product/4087) 
3. BME 680 (and others to be added later)


# PiLoRa setup and installations

1. Create venv
2. Activate venv
3. Active SPI (raspi-config)
4. Install required libraries



# Manuscript

The manuscript for this project can be found:

* (pdf) draft [./manuscript/manuscript.pdf](./manuscript/manuscript.pdf)

# Citing This Work

If you use this work, please cite it using the following citation:

> Hossain, A.m et al SiPARCS Research Repository. [DOI coming soon]

# License

CC-BY-4.0

&copy; Abrar Hossain, Keith E. Maull, Agbeli Ameko
