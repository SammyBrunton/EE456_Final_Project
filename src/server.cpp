#include <iostream>
#include <unistd.h> // For close()
#include <netinet/in.h> // For sockaddr_in
#include <cstring> // For memset


using namespace std;


#define PORT 54321 // Define the port number to be used for the server
int main() {
 // Create a socket using the socket() function
 // AF_INET: Specifies the address family (IPv4)
 // SOCK_STREAM: Specifies the socket type (TCP)
 // 0: Specifies the protocol (default for SOCK_STREAM is TCP)
 int server_fd = socket(AF_INET, SOCK_STREAM, 0);
 // Check if the socket creation was successful
 // If socket() returns -1, error occurred
 if (server_fd == -1) {
 perror("socket failed"); // Print an error message
 exit(EXIT_FAILURE); // Exit the program with a failure status
 }

 // If the socket is successfully created, print a success message
 cout << "Socket created successfully." << endl;

 // Continue with rest of the code
}



// Declare a sockaddr_in structure to hold the address information
struct sockaddr_in address;
// Initialize the address structure to zero
memset(&address, 0, sizeof(address)); // Zero out the address structure
// Set the address family to AF_INET (IPv4)
address.sin_family = AF_INET;
// Set the IP address to listen on any available interface
address.sin_addr.s_addr = INADDR_ANY;
// Set the port number and convert it to network byte order using htons()
// This ensures the correct byte order for network communication
address.sin_port = htons(PORT);
// Bind the socket to the specified address and port
// If bind() returns a value less than 0, an error occurred
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
 perror("bind failed"); // Print an error message if the binding fails
 exit(EXIT_FAILURE); // Exit the program with a failure status
}
// If binding is successful, print a success message
cout << "Binding successful." << endl;


// Put the server socket in a passive mode & waiting for client connection
// The second argument specifies the maximum length of the queue for pending
//connections (3 in this case)
if (listen(server_fd, 3) < 0) {
 perror("listen"); // Print an error message if the listen call fails
 exit(EXIT_FAILURE); // Exit the program with a failure status
}
// If the server successfully starts listening, print a message indicating it
//is ready
cout << "Listening for connections..." << endl;


int new_socket; // Variable to hold the file descriptor for the new
//connection
socklen_t addrlen = sizeof(address); // Length of the address structure
// Accept an incoming connection from the client
// accept() returns a new socket file descriptor for the connection
new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
// Check if accept() was successful
if (new_socket < 0) {
 perror("accept"); // Print an error message if the accept call fails
 exit(EXIT_FAILURE); // Exit the program with a failure status
}
// If the connection is successful, print a message
cout << "Connection accepted." << endl;
// Buffer to store the message received from the client
char buffer[1024] = {0}; // Initialize the buffer to zero
// Read the message from the client into the buffer
// The read() function reads up to 1024 bytes from the socket
read(new_socket, buffer, 1024);
cout << "Message from client: " << buffer << endl; // Print the received
//message
// Message to send back to the client
const char *hello = "Hello from server";
// Send the response message to the client
send(new_socket, hello, strlen(hello), 0);
cout << "Hello message sent." << endl; // Confirm that the message was sent
// Close the socket for the current connection
close(new_socket);
// Close the original server socket
close(server_fd);
return 0; // Exit the program successfully
