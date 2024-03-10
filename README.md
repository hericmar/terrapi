# TerraPi
Simple manager of environments such as terrariums, aquariums, or greenhouses.
Measure temperature, humidity, and other physical quantities, 
and control devices such as fans, heaters, and lights,
and collect the data and plot them into graphs.

> **Note**: This is hobby project and is not intended for production use.

The project consists of Raspberry Pi client implementation, 
a broker server that collects data, 
and a web interface that plots the data into graphs. 

## Requirements
- Raspberry Pi computer for client control unit (tested on Raspberry Pi Zero).
  Please note that Raspberry Pi 5 is not supported, because WiringPi 2.x library is not compatible with it.
- Optional: PostgreSQL database server for the broker.

## Instalation
See [terrapi](./terrapi/README.md) and [broker](./broker/README.md) for installation instructions.

## Configuration
For I2C sensors, you need to enable I2C interface on Raspberry Pi.
```shell
sudo raspi-config
```
Navigate to `Interfacing Options` and enable `I2C`.

### Supported sensors
| Name | Type | Description                     |
| ---- |-----|---------------------------------|
| DHT11 |     | Temperature and humidity sensor |
| HTU21D | I2C | Temperature and humidity sensor |

### Value types (Physical quantities)
Using only SI units.
Supported both in client and broker.

| Name | Unit | Description |
| ---- | ---- | ----------- |
| Humidity | % | Relative humidity |
| Signal | bool | Digital signal |
| Temperature | °C | Temperature |

## Future plans
- Add support for more sensors and devices.
- Add support for InfluxDB and Grafana for data visualization.
