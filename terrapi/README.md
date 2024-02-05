# TerraPI

## Install
```shell
sudo apt update
sudo apt install cmake curl-dev wiringpi
```

## Configure
The main loop consists of a series of steps scheduled non-preemptively with specific timeouts in **milliseconds**.
Note that timeouts are not guaranteed to be exact, but they are guaranteed to be at least the specified value.
This is because the main loop is a single thread and all hardware access is blocking.
Specifically, sensor reads can be time-consuming.

All values in the configuration file are in **milliseconds**.

## Development

