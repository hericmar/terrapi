# TerraPi
Simple manager of environments such as terrariums, aquariums, or greenhouses.
Measure temperature, humidity, and other physical quantities, 
and control devices such as fans, heaters, and lights,
and collect the data and plot them into graphs.

> **Note** This is hobby project and is not intended for production use. It is provided as-is without any warranty.

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
| Name   | Type | Description                     |
|--------|------|---------------------------------|
| DHT11  |      | Temperature and humidity sensor |
| HTU21D | I2C  | Temperature and humidity sensor |

### Value types (Physical quantities)
Using only SI units.
Supported both in client and broker.

| Name        | Unit | Description       |
|-------------|------|-------------------|
| Humidity    | %    | Relative humidity |
| Signal      | bool | Digital signal    |
| Temperature | °C   | Temperature       |

### Expressions
Expressions are used to define conditions for controlling devices.
You can refer to value types by their names as **variables**,
as you defined them in the configuration.
You can use **comparison** and **logical operators**, and **parentheses**
to group expressions.
You can also use **constants** in expressions.

Example expressions:
```toml
[sensor."dht11"]
type = "DHT11"
gpio = 7

[device."lights"]
gpio = 3
rule = "(10:59 < time) & (time < 21:57)"
power = 60.0

[device."heater"]
gpio = 2
rule = "dht11.temperature < 22"
power = 50.0
```

Comparison: 
| Operator | Description          |
|----------|----------------------|
| `=`      | Equal                |
| `<`      | Less than            |
| `>`      | Greater than         |

Logical:
| Operator | Description          |
|----------|----------------------|
| `&`      | Logical AND          |
| `\|`     | Logical OR           |

Parentheses:
| Operator | Description          |
|----------|----------------------|
| `(`      | Open parenthesis     |
| `)`      | Close parenthesis    |

Special variables:
| Name | Description                  |
|------|------------------------------|
| time | Current time in HH:MM format |

Logical operators have higher precedence than comparison operators.
You have to use parentheses to group expressions.

## Future plans
- Add support for more sensors and devices.
- Add support for InfluxDB and Grafana for data visualization.
