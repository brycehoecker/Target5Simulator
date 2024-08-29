#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <limits>
#include <unistd.h>
#include <cstring> // Provides strerror() for error descriptions.
#include <iomanip>
#include <sys/socket.h> // Necessary for socket creation and manipulation.
#include <netinet/in.h> // Defines sockaddr_in structure for IP-based operations.
#include <arpa/inet.h> // Provides utility functions like inet_pton().
#include "SimpleT5Register.cpp"
#include <array>
#include "config.h"
#include <fcntl.h> // for fcntl()
#include <sys/select.h> // for select()
//typedef uint32_t registerArray[84];
//const int PACKET_SIZE = 8192;  // Size of each UDP packet in bytes
//const int MAX_GB = 8192;       // Maximum allowable buffer size in gigabytes
using namespace std;

#define UDP_PACKET_SIZE 8192	// Size of each UDP packet in bytes
#define MAX_GB 100				// Maximum allowable buffer size in gigabytes
#define PACKET_SIZE_BYTES 1400
//#define SINGLE_REGISTER_ARRAY_SIZE = 84;	//Number of 32-bit Registers in one


const uint32_t REGISTER_ARRAY_SIZE = 84;
const uint32_t REGISTER_ARRAY_BYTES = REGISTER_ARRAY_SIZE * sizeof(uint32_t);

//vector<uint32_t[REGISTER_ARRAY_SIZE]> sct_buffer;
vector<array<uint32_t, REGISTER_ARRAY_SIZE>> sct_buffer;

void udpReceiver() {
    int sockfd;
    sockaddr_in serverAddr, clientAddr;
    char packetBuffer[PACKET_SIZE_BYTES];
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Set the socket to non-blocking mode.
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    addr_size = sizeof(clientAddr);

    cout << "Receiver: Listening for incoming UDP packets " << endl;

    fd_set readfds;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        // Use select to wait for data.
        int activity = select(sockfd + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity < 0) {
            perror("select error");
            close(sockfd);
            return;
        }

        if (FD_ISSET(sockfd, &readfds)) {
            int bytesRead = recvfrom(sockfd, packetBuffer, sizeof(packetBuffer), 0, (struct sockaddr *) &clientAddr, &addr_size);
            /*if (bytesRead > 0) {
                cout << "Receiver: Received packet of size: " << bytesRead << endl;
            }// if (bytesRead > 0)*/
        }// (FD_ISSET(sockfd, &readfds))
    }// while(true)

    //Close the UDP Receiver Socket
    close(sockfd);
}

void udpSender() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr);

    // Set the socket to non-blocking mode.
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    uint32_t totalPackets = (sct_buffer.size() * sizeof(sct_buffer[0])) / PACKET_SIZE_BYTES + 1;

    fd_set writefds;
    for (uint32_t i = 0; i < totalPackets; ++i) {
        int bytesToSend = PACKET_SIZE_BYTES < (sct_buffer.size() * sizeof(sct_buffer[0])) - i * PACKET_SIZE_BYTES ? PACKET_SIZE_BYTES : (sct_buffer.size() * sizeof(sct_buffer[0])) - i * PACKET_SIZE_BYTES;
        FD_ZERO(&writefds);
        FD_SET(sockfd, &writefds);

        // Use select to check if the socket is ready for sending data.
        int activity = select(sockfd + 1, nullptr, &writefds, nullptr, nullptr);

        if (activity < 0) {
            perror("select error");
            close(sockfd);
            return;
        }

        if (FD_ISSET(sockfd, &writefds)) {
            int sentBytes = sendto(sockfd, (char*)sct_buffer.data() + i * PACKET_SIZE_BYTES, bytesToSend, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

            if (sentBytes == -1) {
                cout << "Error sending packet " << i << "." << endl;
                close(sockfd);
                return;
            } else {
                //cout << "Packet sent: " << i << endl;
            }
        }
    }

    cout << "All packets sent!" << endl;
    close(sockfd);
}

int main(int argc, char **argv) {
	// Greeting and version information.
	cout << "This is the Beginning of the Simple SCT Simulator Program, Here we go!" << endl;
	cout << "Version " << Simple_SCT_Simulator_VERSION_MAJOR << "." << Simple_SCT_Simulator_VERSION_MINOR << endl;

	// Ask the user for buffer size.
	cout << "Enter the desired buffer size in gigabytes: ";
	uint16_t buffer_size_gigabytes;
	cin >> buffer_size_gigabytes;

	// Check buffer size against the defined maximum.
	if (buffer_size_gigabytes > MAX_GB) {
		cout << "Max amount of RAM is limited to " << MAX_GB << "GB" << endl;
		return 1;
	}

	// Calculate the required size and number of arrays.
	uint64_t totalBytesNeeded = static_cast<uint64_t>(buffer_size_gigabytes) * 1024ULL * 1024ULL * 1024ULL;
	uint64_t num_arrays = totalBytesNeeded / REGISTER_ARRAY_BYTES;

	// Resize and initialize the buffer.
	sct_buffer.resize(num_arrays);
	cout << "Buffer of " << buffer_size_gigabytes << "GB created with " << num_arrays << " arrays." << endl;

	/*
	// Ask the user to check system resources before continuing.
	cout << "Press enter to continue after checking system resources...";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Clear the previous newline left in the buffer.
	cin.get();  // Wait for user to press Enter.
	cout << "After get" << endl;
	*/

	// Start the UDP transmission process.
	// Launch the receiver thread first to ensure it's listening when the sender starts transmitting.
	thread receiverThread(udpReceiver);

	// A small delay to ensure the receiver is set up before we start the sender.
	this_thread::sleep_for(chrono::seconds(2));

	// Now launch the sender thread.
	thread senderThread(udpSender);

	// Wait for the sender to finish. For simplicity, we're not stopping the receiver in this example.
	senderThread.join();

	cout << "Exiting Simple SCT Simulator Program. Have a nice day!" << endl;
	return 0;
}// main

/*
cout << "This is the Beginning of the Simple SCT Simulator Program, Here we go!" << endl;
cout << "Version " << Simple_SCT_Simulator_VERSION_MAJOR << "." << Simple_SCT_Simulator_VERSION_MINOR << endl;

// Define the size of the hardware register array and its byte size
const uint32_t REGISTER_ARRAY_SIZE = 84;
const uint32_t REGISTER_ARRAY_BYTES = REGISTER_ARRAY_SIZE * sizeof(uint32_t);

cout << "Enter the desired buffer size in Gigabytes: ";			    // Ask user for the desired buffer size in gigabytes
uint32_t buffer_size_gigabytes;
cin >> buffer_size_gigabytes;
if (buffer_size_gigabytes > MAX_GB){
	cout << "Max amount of RAM is limited to " << MAX_GB << endl;
	return 1;
}
uint64_t totalBytesNeeded = static_cast<uint64_t>(buffer_size_gigabytes) * 1024ULL * 1024ULL * 1024ULL;     // Calculate the number of register arrays
uint64_t num_arrays = totalBytesNeeded / REGISTER_ARRAY_BYTES;												// needed to fill the specified gigabytes

cout << "Creating an " << buffer_size_gigabytes << "GB buffer and filling it." << endl;		// Create a vector of register arrays to represent the buffer
vector<uint32_t[REGISTER_ARRAY_SIZE]> buffer(num_arrays);    								// Note: This can consume a lot of memory and may cause the program to crash if there isn't enough memory available.
cout << "Buffer filled with " << num_arrays << " register arrays of size " << REGISTER_ARRAY_SIZE << " each.\n";

cout << "Press enter to continue after checking system resources...";
cin.ignore(numeric_limits<streamsize>::max(), '\n'); // To clear the previous newline left in the buffer
cin.get();  // Wait for user to press Enter
cout << "After get" << endl;

// START UDP SOCKET TRANSMISSION

// Create a UDP socket.
cout << "Creating UDP Socket" << endl;
int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
if (sockfd == -1) {
    // If socket creation failed, print an error message and exit.
    cout << "Error creating socket." << endl;
    return 1;
}
else{
	cout << "Socket Successfully Completed" << endl;
}

// Define a structure to hold the destination address information.
sockaddr_in destAddr;
destAddr.sin_family = AF_INET;
destAddr.sin_port = htons(12345);
inet_pton(AF_INET, "127.0.0.1", &destAddr.sin_addr);
uint32_t totalPackets = (buffer.size() * sizeof(buffer[0])) / PACKET_SIZE_BYTES + 1;

for (uint32_t i = 0; i < totalPackets; ++i) {
    int bytesToSend = PACKET_SIZE_BYTES < (buffer.size() * sizeof(buffer[0])) - i * PACKET_SIZE_BYTES ? PACKET_SIZE_BYTES : (buffer.size() * sizeof(buffer[0])) - i * PACKET_SIZE_BYTES;
    int sentBytes = sendto(sockfd, (char*)buffer.data() + i * PACKET_SIZE_BYTES, bytesToSend, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

    if (sentBytes == -1) {
        cout << "Error sending packet " << i << "." << endl;
        close(sockfd);
        return 1;
    }
    else{
    	//cout << "Packet turret goes, BBBBBBBBBRRRRRRRRRRRRTTTTTTTTTTTTTTTTTTT   " << right << setw(20) << i << endl;
    	cout << " Sending Packet #" << right << setw(20) << i << endl;
    }
}
cout << "Closing Socket" << endl;
close(sockfd);
cout << "Socket Closed" << endl;
cout << endl;

// Create the receiver thread
thread receiverThread(udpReceiver);

// Allow the receiver to start up
this_thread::sleep_for(chrono::seconds(2));

// Create the sender thread
thread senderThread(udpSender);

// Wait for the threads to finish (in this example they won't, but in real scenarios, you might have conditions to end these threads)
receiverThread.join();
senderThread.join();

// Your buffer creation logic here:
cout << "Enter the desired buffer size in gigabytes: ";
uint32_t buffer_size_gigabytes;
cin >> buffer_size_gigabytes;
if (buffer_size_gigabytes > MAX_GB) {
    cout << "Max amount of RAM is limited to " << MAX_GB << "GB" << endl;
    return 1;
}

uint64_t totalBytesNeeded = static_cast<uint64_t>(buffer_size_gigabytes) * 1024ULL * 1024ULL * 1024ULL;
uint64_t num_arrays = totalBytesNeeded / REGISTER_ARRAY_BYTES;
sct_buffer.resize(num_arrays); // Adjust the buffer size based on the input
cout << "Buffer of " << buffer_size_gigabytes << "GB created with " << num_arrays << " arrays." << endl;

// ... Rest of your initialization ...

thread receiverThread(udpReceiver);
this_thread::sleep_for(chrono::seconds(2));  // Giving some time for the receiver to set up
thread senderThread(udpSender);

senderThread.join();  // We should join the sender thread, but not the receiver in this simple example, because the receiver has an infinite loop.

*/
