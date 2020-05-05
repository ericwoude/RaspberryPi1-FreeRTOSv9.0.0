# RaspberryPi1-FreeRTOSv9.0.0
> FreeRTOS v9.0.0 port for Raspberry Pi 1

## Includes
- FreeRTOS v9.0.0 and the required modification to run on Raspberry Pi 1
- BCM2835 library to interface most of the devices present in the BCM2835 chip (I2C, SPI, GPIOS...). This library was originally written for Linux, I adapted it to run in bare metal
- A driver to use the miniuart on GPIO pins 14 and 15

## Credit to the following people:
- James Walmsley who made the original FreeRTOS v7.2.0 - https://github.com/jameswalmsley/RaspberryPi-FreeRTOS
- David Welch who wrote a lot of bare metal related stuff - https://github.com/dwelch67/raspberrypi
- Mike McCauley who wrote the BCM2835 library for Linux - http://www.airspayce.com/mikem/bcm2835/
- Léonard Bise and lbise who made this version of FreeRTOS compatible with Raspberry 1 - https://github.com/leodido99/RaspberryPi1-FreeRTOSv9.0.0

## How to use
1. Create the docker image by running the following command  
`docker build -t [name] --build-arg USER_ID=$(id -u) --build-arg GROUP_ID=$(id -g) .`
2. Run the container's console and bind the project folder  
Use the following command in the project's root folder; `docker run -it --rm --mount "type=bind,src=$(pwd),dst=/opt/shared" --workdir /opt/shared [name] bash`
3. Compile the kernel by running `make` in the following folder: _/FreeRTOS/Demo/ARM6_BCM2835_
