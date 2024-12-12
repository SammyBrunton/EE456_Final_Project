# EE456  
# LoRa 1262 Project  

**Class:** EE456  
**Project Name:** LoRa 1262  

This project utilizes the SX1262 chip and Raspberry Pi to transmit and receive data using LoRa and FSK modulation.

***********VERY IMPORTANT***********  
- Enable SPI before attempting to transmit or receive.  
Run the following commands:  
`sudo raspi-config`  
Navigate to `Interfacing Options` -> `SPI` and enable it.  
Reboot your Raspberry Pi.  
**********************************************************  

## Installation Instructions  

--Run  `sudo apt update && sudo apt install -y cmake git`  

1. Clone the GitHub Repository for EE456:  
`git clone https://github.com/BenDuval/EE456.git`  
Navigate into the 'EE456' directory.  

2. Navigate to 'WiringPi' directory:  
Run:  
`./build`  
  

3. Navigate to 'RadioLib/examples/NonAudrino/Raspberry/'  
Run:  
`mkdir build`  
`cd build`  
`cmake ..`  
`make`    

4. Make and build the Project:  
From EE456 directory run:  
`mkdir build`  
Navigate to the `build` directory:    
`cd build`  
Run:  
`cmake ../src`  **(cmake must be ran after any changes to CMakeLists.txt such as adding a new executable!)**  
Then run:  
`make` **(make must be ran after all changes to .cpp files)**  

## Wiring Diagram for WiringPi Library & SX1262 MBED Shield  

- NSS: GPIO 8, WPI# 10  
- Reset: GPIO 5, WPI# 21  
- DI01: GPIO 27, WPI# 2  
- Busy: GPIO 17, WPI# 0  
- Pwr: Pin 1 3.3V   
- Gnd: Pick one   
  
Ensure your wiring matches the above configuration for proper operation with the RPI and MBED shield.  

## Wiring Diagram for WiringPi Library & SX1262 LORAWAN Pi Hat  

- NSS: GPIO 21, WPI# 29  
- Reset: GPIO 18, WPI# 1  
- DI01: GPIO 16, WPI# 27  
- Busy: GPIO 20, WPI# 28  

Ensure your wiring matches the above configuration for proper operation with the RPI and SX1262 LORAWAN Pi Hat.  

## Running the LoRa TX and RX  

1. **Transmit Code**:  
Run the `lora_gps_tx.cpp` example to confirm transmission without errors using:  
`sudo ./lora_rx`  

2. **Receive Code**:  
Run the `lora_rx_client.cpp` example to confirm initialization without any errors using:  
`sudo ./lora_rx`  
Check for errors and verify successful reception.  

The above two scrips will only TX and RX via lora, to transmit and recieve via TCP/IP continue to the next section.

## Communicating via TCP/IP
Navigate to the `src` folder and locate the following scripts:
- `server.cpp`: This script needs to run on the TCP/IP receiving computer. 
- `client.cpp`: This script is integrated with in the Lora_rx and gets executed automatically.

## Configuring the scripts
The above scripts might need to be configured incase the server's IP address changes. Within the `Lora_rx_client.cpp` line _57_ specifies an IP address as a string, this needs to match the specific server IP address.

### Running the scripts
This step applies to all C++ scripts. To run the scripts an executable must be generated, run `g++ script_name.cpp -0 executable_name`.
Adjust `script_name` to refelct either of the above scripts names and `executable_name`to the same name as `script_name` however this name does not matter as long as it is tracked.
To actually execute the scripts run `./executable_name`. 

## Helpful Tips
Since the GPS module outpus stright string NEMA messages, it is possible to simply look at it by reading the terminal. A tool called `picocom` is great for such as task. This tool might not be installed on your device by defult but can be installed via `sudo apt install picocom`
Run `picocom -b 9600 /dev/serial0` to stright read the NEMA messages from the GPS module, this can be helpful if there are some issues where the output of the GPS module is uncertain.
**NOTE:** to exit from picocom hit `ctrl + a` then `ctrl + x`


## Additional Resources  

- [RadioLib Documentation](https://jgromes.github.io/RadioLib/index.html)  
- [RadioLib GitHub Repository](https://github.com/jgromes/RadioLib)  
- [WiringPi Pinout](https://pinout.xyz/pinout/wiringpi)  
- [WiringPi GitHub Repository](https://github.com/WiringPi/WiringPi)  
- [Waveshare SX1262 LoRaWAN Hat](https://www.waveshare.com/sx1262-lorawan-hat.htm)  
- [Semtech SX1262 Product Page](https://www.semtech.com/products/wireless-rf/lora-connect/sx1262)  
