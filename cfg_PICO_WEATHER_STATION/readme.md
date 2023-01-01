# Pico Weather Station

#### Location

[Projects](../README.md) / Pico-Weather-Station

#### Navigation

[Features](#features) | [Context](#context) | [Usage](#usage) | [Changelog](#changelog)

## Brief
[[TOP](#location)]

This project realizes a simple weather-station to be usable on a Raspberry Pico.

## Features
[[TOP](#location)]

- provide ambient temperature information
- provide ambient humidity information
- display information on a LCD
- provide maximum and minimum values of temperatur and humidity of the last 24 hours
- support for an keypad

## Context
[[TOP](#location)]

![Context](uml/img/pico_weather_station_diagram_component_context.svg )

| Node      | Description |
|-----------|-------------|
| DCF       |             |
| Sensor    | Different sensor to measure current ambient status.<br>E.g. temperature, humidity, air-pressure |
| LCD       | Display to show current status of ambient sensors. |
| Keyboard  | A simple keypad to control the weather-station.<br>Including changing inforamtion thats currently shown on the LCD . |
| Framework | Set of SW-modules to realize logic-functions and to control external HW-components |
| USB       | The system includes the possibility to connect an external computer to exchange data. |

## Usage
[[TOP](#location)]

## Changelog
[[TOP](#location)]

### Version 1.0

- Date: NOT RELEASED
- Author: Sebastian Lesse

#### New Features

- none

#### Bug Fixes

- none

#### Misc

- none

#### Known Bugs

- none