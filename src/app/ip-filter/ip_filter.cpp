#include <algorithm>
#include <iostream>
#include <vector>

#include <log-reader/log_reader.hpp>

int main() {
  try {
    std::vector<uint32_t> ips = LogReader().read(std::cin);

    // Reverse lexicographically sort
    std::sort(ips.begin(), ips.end(), std::greater<uint32_t>());
    LogReader().print(ips, std::cout);

    // Filter by first byte and output (filter(1))
    auto filtered = std::vector<uint32_t>();
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(filtered),
                 [](uint32_t ip) { return (ip >> 24) == 1; });
    LogReader().print(filtered, std::cout);

    // Filter by first and second bytes and output (filter(46, 70))
    auto filtered2 = std::vector<uint32_t>();
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(filtered2),
                 [](uint32_t ip) { return (ip >> 24) == 46 && ((ip >> 16) & 0xFF) == 70; });
    LogReader().print(filtered2, std::cout);

    // Filter by any byte and output (e.g., filter(46))
    auto filtered3 = std::vector<uint32_t>();
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(filtered3),
                 [](uint32_t ip) { 
                   return (ip >> 24) == 46 || ((ip >> 16) & 0xFF) == 46 ||
                          ((ip >> 8) & 0xFF) == 46 || (ip & 0xFF) == 46;
                 });
    LogReader().print(filtered3, std::cout);
        
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}