#include <iostream>
#include <unistd.h> // For close()
#include <netinet/in.h> // For sockaddr_in
#include <cstring> // For memset
#include <fcntl.h> // For opening files or serial ports
#include <termios.h> // For configuring serial ports
#include <sstream> // For parsing NMEA data
#include "RadioLib.h" // LoRa library for radio communication
#include "PiHal.h" // HAL for Raspberry Pi

using namespace std;

// LoRa Radio Configuration
PiHal* hal = new PiHal(0); // SPI interface 0
SX1262 radio = new Module(hal, 29, 27, 1, 28); // GPIO pins for SX1262 module

#define PORT 8080 // Port for the server to listen on

// Function to parse and process GPS data
void parseNMEAAndProcess(const std::string &line) {
    if (line.rfind("$GNGGA", 0) == 0) {  // Check if the line starts with "$GNGGA"
        std::istringstream ss(line);
        std::string token;
        int index = 0;
        std::string latitude, longitude, lat_dir, lon_dir;

        // Extract relevant tokens from the NMEA sentence
        while (std::getline(ss, token, ',')) {
            if (index == 2) latitude = token;        // Latitude
            else if (index == 3) lat_dir = token;    // Latitude Direction (N/S)
            else if (index == 4) longitude = token;  // Longitude
            else if (index == 5) lon_dir = token;    // Longitude Direction (E/W)
            index++;
        }

        // If valid GPS data is parsed
        if (!latitude.empty() && !longitude.empty()) {
            char gps_data[256];
            snprintf(gps_data, sizeof(gps_data), "Lat: %s %s, Lon: %s %s", 
                     latitude.c_str(), lat_dir.c_str(), longitude.c_str(), lon_dir.c_str());

            // Transmit the GPS data over LoRa
            int state = radio.transmit(gps_data);
            if (state == RADIOLIB_ERR_NONE) {
                printf("LoRa Transmission success!\n");
            } else {
                printf("LoRa Transmission failed, code %d\n", state);
            }

            // Log GPS data
            std::cout << "Parsed GPS Data: " << gps_data << std::endl;
        }
    }
}

int main() {
    // Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    cout << "Socket created successfully." << endl;

    // Configure server address
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); // Zero out the address structure
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Listen on any interface
    address.sin_port = htons(PORT); // Convert port to network byte order

    // Bind socket to the specified port and address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    cout << "Binding successful." << endl;

    // Put server in listening mode
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    cout << "Listening for connections..." << endl;

    // Accept a connection
    socklen_t addrlen = sizeof(address);
    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    cout << "Connection accepted." << endl;

    // Initialize LoRa module
    printf("[SX1262] Initializing ... ");
    int state = radio.begin(915.0, 125.0, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0.0, false);
    if (state != RADIOLIB_ERR_NONE) {
        printf("LoRa Initialization failed, code %d\n", state);
        return 1;
    }
    printf("LoRa Initialization success!\n");

    // Open the serial port for GNSS data
    int serial_port = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        cerr << "Error opening serial port\n";
        return 1;
    }

    // Configure serial port
    struct termios tty;
    tcgetattr(serial_port, &tty);
    tty.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    tty.c_iflag = IGNPAR;
    tcsetattr(serial_port, TCSANOW, &tty);

    string buffer;
    char c;

    // Server main loop
    while (true) {
        // Read GPS data from GNSS module
        if (read(serial_port, &c, 1) > 0) {
            if (c == '\n') { // End of line detected
                parseNMEAAndProcess(buffer); // Process GPS data
                buffer.clear(); // Clear buffer for the next message
            } else {
                buffer += c; // Append character to buffer
            }
        }

        // Receive and respond to messages from the client
        char recv_buffer[1024] = {0};
        read(new_socket, recv_buffer, 1024);
        cout << "Message from client: " << recv_buffer << endl;

        const char *response = "GPS data being processed.";
        send(new_socket, response, strlen(response), 0);
        cout << "Response sent to client." << endl;
    }

    // Cleanup
    close(serial_port);
    close(new_socket);
    close(server_fd);

    return 0;
}