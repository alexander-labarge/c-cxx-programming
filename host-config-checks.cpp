#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>

int main() {
    // Get the hostname
    char hostname[1024];
    gethostname(hostname, 1024);
    std::cout << "Hostname: " << hostname << std::endl;

    // Get the current time of the host
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::cout << "Current time: " << std::ctime(&current_time);

    // Get the current network sync time
    std::ifstream ifs("/etc/timesyncd.conf");
    std::string line;
    std::time_t network_time = 0;
    while (std::getline(ifs, line)) {
        if (line.find("NTP") != std::string::npos) {
            std::getline(ifs, line);
            network_time = std::stoi(line);
            break;
        }
    }
    std::cout << "Network sync time: " << std::ctime(&network_time);

    // Display the difference
    std::cout << "Time difference: " << std::difftime(current_time, network_time) << " seconds" << std::endl;

    // Get the return of configuration of the /etc/hosts file
    std::ifstream hosts_file("/etc/hosts");
    std::string hosts_contents((std::istreambuf_iterator<char>(hosts_file)), std::istreambuf_iterator<char>());
    std::cout << "Contents of /etc/hosts:\n" << hosts_contents << std::endl;

    return 0;
}
