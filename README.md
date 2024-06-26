# SiPaRCS 2024 Research

2024 Research.

# Project Introduction  LoRa Mesonet Weather Monitoring System
This project aims to develop a mesonet of weather monitoring sensors that collect various environmental data, including temperature, humidity, altitude, air quality, UV levels, and rain data. The collected data will be transmitted via a LoRa (Long Range) network to intermediary Raspberry Pi gateways. These Raspberry Pi devices will then forward the aggregated data over the internet to a network server, which distributes the information to application servers for further processing and analysis.

Key Components:
Sensor Devices: These devices measure and collect weather data. Each sensor node is equipped with LoRa communication capabilities to transmit data wirelessly.
Raspberry Pi Gateways: The Raspberry Pi devices act as local gateways, receiving data from multiple sensor devices via the LoRa network. They then transmit this data over the internet to central servers.
Network Server: This server acts as a central hub, receiving data from various Raspberry Pi gateways and distributing it to multiple application servers.
Application Servers: These servers process the weather data for various applications, including weather modeling and forecasting, real-time alert systems, and environmental monitoring.
Join Server: This server handles the network join requests from the sensor devices, ensuring secure and authenticated communication.
Use Cases:
Weather Modeling: The collected data can be used to create detailed weather models that help in predicting local weather patterns with greater accuracy.
Real-Time Alert Systems: The system can generate real-time alerts for severe weather conditions such as storms, heavy rainfall, or extreme temperatures, helping communities to prepare and respond promptly.
Environmental Monitoring: Continuous monitoring of air quality, UV levels, and other environmental factors can help in assessing the impact of pollution and climate change, facilitating environmental protection efforts.
Agricultural Applications: Farmers can use the data to optimize irrigation schedules, protect crops from adverse weather conditions, and improve overall agricultural productivity.
Urban Planning: City planners can utilize the data for designing resilient infrastructure that can withstand local weather conditions and mitigate the impact of natural disasters.

-   [a rando link
    (https://loremipsum.io/generator/?n=5&t=p)](https://loremipsum.io/generator/?n=5&t=p)

![LoRa Mesonet Weather Monitoring System](images/LoRA_firenet.png)


## Mathematical Formulation

The project tries to solve:

$$
\int^\infty f(x,y) \frac{dx}{dy} \times \int\int^{-\infty} g(x,y,z)\frac{dy}{dz}
$$

# Research Questions

Leo duis ut diam quam. Elit sed vulputate mi sit amet mauris commodo
quis. Et sollicitudin ac orci phasellus egestas tellus rutrum tellus.
Gravida rutrum quisque non tellus orci ac auctor. Tincidunt dui ut
ornare lectus sit amet est placerat. Mauris in aliquam sem fringilla ut
morbi tincidunt augue. Volutpat sed cras ornare arcu dui vivamus.

|  RQ |   Summary | Notebook |
| :---: | ----------|----------|
| 1 | How can we detect  the mass of dark matter? | [./notebooks/00_extract.ipynb](./notebooks/00_extract.ipynb) |
| 2 | How can we detect  the mass of subspace? | [./notebooks/01_explore.ipynb](./notebooks/01_explore.ipynb) |

# Code

Leo duis ut diam quam. Elit sed vulputate mi sit amet mauris commodo
quis. Et sollicitudin ac orci phasellus egestas tellus rutrum tellus.
Gravida rutrum quisque non tellus orci ac auctor. Tincidunt dui ut
ornare lectus sit amet est placerat. Mauris in aliquam sem fringilla ut
morbi tincidunt augue. Volutpat sed cras ornare arcu dui vivamus.

|  Data |   Summary | Location |
| :---: | ----------|----------|
| Arduino | This folder contains source code. | [./cpde/](./code) |
| CircuitPython | This folder contains source code. | [./cpde/](./code) |

# Data 


Leo duis ut diam quam. Elit sed vulputate mi sit amet mauris commodo
quis. Et sollicitudin ac orci phasellus egestas tellus rutrum tellus.
Gravida rutrum quisque non tellus orci ac auctor. Tincidunt dui ut
ornare lectus sit amet est placerat. Mauris in aliquam sem fringilla ut
morbi tincidunt augue. Volutpat sed cras ornare arcu dui vivamus.


|  Data |   Summary | Location |
| :---: | ----------|----------|
| EPA source | This folder contains EPA source data. | [./data/epa/](./data/epa) |

# Manuscript

The manuscript for this project can be found:

* (pdf) draft [./manuscript/manuscript.pdf](./manuscript/manuscript.pdf)

# Citing This Work

If you use this work, please cite it using the following citation:

> Hossain, A.m et al SiPARCS Research Repository. [DOI coming soon]

# License

CC-BY-4.0

&copy; Abrar Hossain, Keith E. Maull, Agbeli Ameko
