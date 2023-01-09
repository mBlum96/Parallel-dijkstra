#include <iostream>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <random>

int main(int argc, char *argv[])
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error: Unable to create socket" << std::endl;
        return 1;
    }

    // Set up the server address
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080); // Replace 9876 with the port number of the server
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Connect to the server
    if (connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Error: Unable to connect to server" << std::endl;
        return 1;
    }

    // Send data to the server
    char data[] = "4 8 2 4";
    send(sockfd, data, sizeof(data), 0);

    // Receive data from the server
    char buffer[1024];
    int num_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    //send random edges to the server

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> src_dist(0, 199);
    std::uniform_int_distribution<> dest_dist(0, 199);
    std::uniform_int_distribution<> weight_dist(1, 1000);

    for (int i = 0; i < 8; i++)
    {
        // Generate random values for the source, destination, and weight
        int src = src_dist(gen);
        int dest = dest_dist(gen);
        int weight = weight_dist(gen);
        
        //make a string to send to the server made up of the random values
        //using array indexing to make a string
        char out[5];
        out[0] = src;
        out[1] = ' ';
        out[2] = dest;
        out[3] = ' ';
        out[4] = weight;
        //write this string to the server
        send(sockfd, out, sizeof(out), 0);

        // send(sockfd, out, sizeof(out), 0);

    }
    if (num_bytes < 0)
    {
        std::cerr << "Error: Unable to receive data from server" << std::endl;
        return 1;
    }
    std::cout << "Received " << num_bytes << " bytes: " << buffer << std::endl;

    // Close the socket
    close(sockfd);

    return 0;
}
