#include "log_reader.hpp"

#include <ostream>
#include <sstream>
#include <stdexcept>

uint32_t LogReader::ipStringToUint32(const std::string& ipStr) {
  uint32_t result = 0;
  int octet = 0;
  std::istringstream iss(ipStr);
  std::string part;

  while (std::getline(iss, part, '.') && octet < 4) {
    try {
      int value = std::stoi(part);
      if (value < 0 || value > 255) {
        throw std::out_of_range("Octet value out of range");
      }
      result = (result << 8) | static_cast<uint8_t>(value);
      octet++;
    } catch (const std::exception& e) {
      throw std::invalid_argument("Invalid IP address: " + ipStr);
    }
  }

  if (octet != 4) {
    throw std::invalid_argument("Invalid IP address: " + ipStr);
  }

  return result;
}

std::string LogReader::ipUint32ToString(uint32_t ip) {
  return std::to_string(ip >> 24) + "." +
         std::to_string((ip >> 16) & 0xFF) + "." +
         std::to_string((ip >> 8) & 0xFF) + "." +
         std::to_string(ip & 0xFF);
}

std::vector<uint32_t> LogReader::read(std::istream& input) {
  std::vector<uint32_t> ips;

  std::string line;
  int lineNumber = 0;

  while (std::getline(input, line)) {
    lineNumber++;
    
    if (line.empty()) {
      continue;
    }
    
    std::istringstream iss(line);
    std::string ipStr;

    if (!std::getline(iss, ipStr, '\t')) {
      throw std::invalid_argument("Invalid TSV format at line " +
                                  std::to_string(lineNumber));
    }

    try {
      ips.push_back(ipStringToUint32(ipStr));
    } catch (const std::exception& e) {
      throw std::invalid_argument("Error parsing line " +
                                  std::to_string(lineNumber) + ": " + e.what());
    }
  }

  return ips;
}

void LogReader::print(const std::vector<uint32_t>& ips, std::ostream& output) {
  for (uint32_t ip : ips) {
    output << ipUint32ToString(ip) << '\n';
  }
}
