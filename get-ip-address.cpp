/**
 * @file get-ip-address.cpp
 * @brief A program that retrieves the IP addresses and MAC addresses of all the network interfaces on the system and prints them to the console and/or writes them to a file.
 * @author Alexander La Barge
 * @date 7/19/2023
 * @license GNU v3
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <chrono>
#include <ctime>

/**
 * @brief Converts a MAC address to a string.
 * @param mac A pointer to an array of 6 bytes representing a MAC address.
 * @return A string representation of the MAC address in the format `xx:xx:xx:xx:xx:xx`.
 */
std::string macToString(const unsigned char* mac) {
    char buf[18];
    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(buf);
}

/**
 * @brief The entry point of the program.
 * @param argc The number of command-line arguments passed to the program.
 * @param argv An array of strings containing the command-line arguments.
 * @return 0 on success, 1 on failure.
 */
int main(int argc, char *argv[]) {
    struct ifaddrs *ifaddr, *ifa;
    char ip_address[INET_ADDRSTRLEN];
    unsigned char mac_address[6];

    // Get the list of network interfaces
    if (getifaddrs(&ifaddr) == -1) {
        std::cerr << "getifaddrs error" << std::endl;
        return 1;
    }

    // Check if the user specified a filename to write the IP addresses to
    std::ofstream outfile;
    if (argc > 1) {
        outfile.open(argv[1]);
        if (!outfile.is_open()) {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
    }

    // Loop through the list of network interfaces
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        // Check if the interface has an IPv4 address
        if ((ifa->ifa_flags & IFF_UP) &&
            (ifa->ifa_flags & IFF_RUNNING) &&
            (ifa->ifa_flags & IFF_BROADCAST) &&
            (ifa->ifa_addr->sa_family == AF_INET)) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN); // Convert the binary IP address to a human-readable string

            // Get the MAC address for the interface
            int fd = socket(AF_INET, SOCK_DGRAM, 0);
            struct ifreq ifr;
            ifr.ifr_addr.sa_family = AF_INET;
            strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ-1);
            ioctl(fd, SIOCGIFHWADDR, &ifr);
            close(fd);
            memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);

            // Get the current date and time
            auto now = std::chrono::system_clock::now();
            std::time_t time = std::chrono::system_clock::to_time_t(now);
            std::string date_time = std::ctime(&time);
            // date_time.pop_back(); // Remove the newline character from the end of the string
            // commenting just to get each data point on its own line - remove space in front of interface

            std::string data = date_time + "Interface: " + std::string(ifa->ifa_name) + "\n";
            data += "IP address: " + std::string(ip_address) + "\n";
            data += "MAC address: " + macToString(mac_address) + "\n\n";

            std::cout << data; // Print the data to the console

            if (outfile.is_open()) {
                outfile << data; // Write the data to the file
            }
        }
    }

    freeifaddrs(ifaddr); // Free the memory allocated by getifaddrs

    if (outfile.is_open()) {
        outfile.close(); // Close the output file
    }

    return 0;
}