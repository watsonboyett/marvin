
# Revision History

## v2A

* Major redesign of hardware!
* Changed MCU to ESP32-WROOM
* Consolidated all parts to one PCB
* Changed mic circuit to use digital mic (no amplifier needed)

## v1A

* Initial design

## v1B

* Moved PIR output to D8 pin
* Fixed floating CSB pin on MS5607 chip 
* Added pull-down to PIR output

## v1C

* Changed pressure sensor to LPS22 chip
* Added microphone and amp
* Added 3v3 regulator (fed from 5v rail)
* Added current limiting resistor to PIR output

## v1D

* Added ADC chip for mic (ESP's ADC does not seem to meet noise specs when WiFi is used)
* Replaced 3v3 regulator with precision reference

## v1E

* Added Pi filter to create analog voltage rail
* Changed mic to SPH1642 part
* Increased mic amp gain


# Alternate Parts 

These are not intended to be drop-in replacements. These are parts that may provide similar performance and similar cost (or were found while performing initial component selection).

## MCU

* D1 Mini Pro - $5, 11 DIO, 1 AIO [web](https://www.wemos.cc/product/d1-mini-pro.html)


## Temp/Humidity

* SHT35 - $12, I2C, 0.1°C/1.5%RH accuracy
* SHT21 - $9, I2C, 0.5°C/2%RH accuracy
* SHT25 - $14, I2C, 0.2°C/1.8%RH accuracy
* Si7013 - $4, I2C, 0.4°C/3%RH accuracy

## Light

* APDS-9250 - $3, I2C, Separate R/G/B/Vis/IR channels
* LTR-329ALS-01 - $1, I2C, Separate Visible/IR channels
* TSL45315CL - $2, I2C, 
* PDV-P9203 - $2, Analog, 


## PIR

* EKMC1601112 - $12, Standard range/angle
* EKMC1603112 - $13, Long range, standard angle
* AMN34111 - $23, Wall range/angle


## Pressure

* LPS22HBTR - $4, I2C, high accuracy, decent range
* ICM-20789 - $9, I2C, 1hPa, 300hPa - 1100hPa (includes gyroscope and accelerometer)
* DPS422XTSA1 - $3, I2C, 0.06hPa/0.4°C, 300–1200 hPa


## Sound 

* (MIC) SPM0404HE5H-PB - $4
* (MIC) CMA-4544PF-W - $2
* (MIC) SPU0410HR5H-PB - $1
* (AMP) MCP6022T-I/ST - $2
* http://electronics.stackexchange.com/a/16596/83463
* http://electronics.stackexchange.com/a/58703/83463


## Air Quality

* AS-MLV-P2 - $25, CO/
* SGP30 - $14, MO Gas (CO2 Equivalent)
* CCS811B - $14, I2C, TVOC + eCO2


## Vibration/Acceleration

* D7S-A0001 - $30, 3 Axis

