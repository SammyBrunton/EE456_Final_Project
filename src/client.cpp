// Part 1 : Include Libraries
#include <iostream>
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h> // For inet_pton()
#include <unistd.h> // For close()
#include <cstring>
using namespace std;

// Part 2 : Define Constants and Create a Socket
#define PORT 54321
int main() {
 int sock = socket(AF_INET, SOCK_STREAM, 0);
 if (sock < 0) {
 perror("Socket creation error");
 return -1;
 }
 cout << "Socket created successfully." << endl;

// Part 3 : Define Server Address and Connect
struct sockaddr_in serv_addr;
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_port = htons(PORT);
 // Convert IPv4 address from text to binary
 if (inet_pton(AF_INET, "10.40.120.82", &serv_addr.sin_addr) <= 0) {
 perror("Invalid address / Address not supported");
 return -1;
 }
 if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
{perror("Connection failed");
 return -1;
 }
 cout << "Connected to the server." << endl;
 
 // Part 4 : Send and Receive Data
 const char *hello = "Hello from client";
 send(sock, hello, strlen(hello), 0);
 cout << "Hello message sent." << endl;
 char buffer[1024] = {0};
 read(sock, buffer, 1024);
 cout << "Message from server: " << buffer << endl;
 close(sock);
 return 0;
}