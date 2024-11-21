// Part 1 : Include Libraries
#include <iostream>
#include <sys/socket.h> // For socket-related functions
#include <arpa/inet.h> // For inet_pton() and sockaddr_in
#include <unistd.h> // For close() and other system calls
#include <cstring> // For string handling
#include <string>
#include <fcntl.h> // For opening files or serial ports
#include <termios.h> // For configuring serial port
#include <sstream> // For parsing NMEA data
#include "RadioLib.h" // LoRa library for handling radio communication
#include "PiHal.h" // HAL for Raspberry Pi hardware abstraction

using namespace std;

// Create a new instance of the HAL class for SPI communication
PiHal* hal = new PiHal(0); // SPI interface 0

// Create a radio object for LoRa communication using the SX1262 chip
SX1262 radio = new Module(hal, 29, 27, 1, 28); // GPIO pins configured for SX1262 module

// Constants for the server connection
#define PORT 54321 // Port number for the TCP server
#define SERVER_IP "10.40.120.82" // IP address of the TCP server

// Function to parse NMEA GPS data and transmit it
void parseNMEAAndTransmit(const std::string &line, int sock) {
    // Check if the line starts with "$GNGGA" (a key GPS data message format)
    if (line.rfind("$GNGGA", 0) == 0) {
        std::istringstream ss(line); // Create a string stream for parsing
        std::string token; // Variable to hold tokens from the string
        int index = 0; // Index for token position
        std::string latitude, longitude, lat_dir, lon_dir; // Variables for GPS data

        // Parse the line by splitting it at commas
        while (std::getline(ss, token, ',')) {
            if (index == 2) latitude = token;        // Latitude value
            else if (index == 3) lat_dir = token;    // Latitude direction (N/S)
            else if (index == 4) longitude = token;  // Longitude value
            else if (index == 5) lon_dir = token;    // Longitude direction (E/W)
            index++;
        }

        // Check if latitude and longitude were successfully extracted
        if (!latitude.empty() && !longitude.empty()) {
            // Create a formatted GPS data string
            char gps_data[256];
            snprintf(gps_data, sizeof(gps_data), "Lat: %s %s, Lon: %s %s", 
                     latitude.c_str(), lat_dir.c_str(), longitude.c_str(), lon_dir.c_str());

            // Transmit the GPS data over LoRa
            int state = radio.transmit(gps_data);
            if (state == RADIOLIB_ERR_NONE) {
                printf("LoRa Transmission success!\n");

                // Send the same GPS data to the TCP server
                send(sock, gps_data, strlen(gps_data), 0);
                cout << "GPS data sent to server: " << gps_data << endl;
            } else {
                printf("LoRa Transmission failed, code %d\n", state);
            }

            // Print the parsed GPS data to the console
            std::cout << "Latitude: " << latitude << " " << lat_dir
                      << ", Longitude: " << longitude << " " << lon_dir << std::endl;
        }
    }
}

int main() {
    // Part 2 : Create a TCP Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0); // Create a socket for IPv4 and TCP
    if (sock < 0) {
        perror("Socket creation error"); // Error if socket creation fails
        return -1;
    }
    cout << "Socket created successfully." << endl;

    // Part 3 : Define Server Address and Connect
    struct sockaddr_in serv_addr; // Structure for server address
    serv_addr.sin_family = AF_INET; // IPv4 address family
    serv_addr.sin_port = htons(PORT); // Convert port number to network byte order

    // Convert the server IP address from text to binary format
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    cout << "Connected to the server." << endl;

    // Initialize the LoRa radio module
    printf("[SX1262] Initializing ... ");
    int state = radio.begin(915.0, 125.0, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0.0, false);
    if (state != RADIOLIB_ERR_NONE) {
        printf("LoRa Initialization failed, code %d\n", state);
        return 1;
    }
    printf("LoRa Initialization success!\n");

    // Open the serial port to read GNSS data
    int serial_port = open("/dev/serial0", O_RDWR | O_NOCTTY); // Serial port for GPS
    if (serial_port < 0) {
        cerr << "Error opening serial port\n";
        return 1;
    }

    // Configure the serial port for 9600 baud rate
    struct termios tty;
    tcgetattr(serial_port, &tty); // Get current serial port settings
    tty.c_cflag = B9600 | CS8 | CLOCAL | CREAD; // Set baud rate, 8-bit data, local mode
    tty.c_iflag = IGNPAR; // Ignore parity errors
    tcsetattr(serial_port, TCSANOW, &tty); // Apply settings immediately

    string buffer; // String buffer to store incoming GNSS data
    char c; // Temporary variable to hold each character read from the serial port

    // Part 4 : Read GNSS Data and Transmit
    while (true) {
        // Read one character at a time from the serial port
        if (read(serial_port, &c, 1) > 0) {
            if (c == '\n') {  // If newline character detected, we have a complete line
                parseNMEAAndTransmit(buffer, sock);  // Parse and transmit the data
                buffer.clear();     // Clear the buffer for the next line
            } else {
                buffer += c;  // Append character to the buffer
            }
        }
    }

    // Close the serial port and socket before exiting
    close(serial_port);
    close(sock);

    return 0;
}
