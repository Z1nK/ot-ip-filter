#include "log_reader.hpp"

#include <charconv>
#include <ostream>
#include <sstream>
#include <stdexcept>

uint32_t LogReader::ipStringToUint32_slow(const std::string &ipStr) {
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
      ++octet;
    } catch (const std::exception &e) {
      throw std::invalid_argument("Invalid IP address: " + ipStr);
    }
  }

  if (octet != 4) {
    throw std::invalid_argument("Invalid IP address: " + ipStr);
  }

  return result;
}

uint32_t LogReader::ipStringToUint32(std::string_view ipStr) {
  uint32_t result = 0;
  int octet = 0;
  size_t start = 0;

  while (octet < 4) {
    size_t end = ipStr.find('.', start);
    if (end == std::string_view::npos) {
      end = ipStr.size();
    }

    std::string_view part = ipStr.substr(start, end - start);
    try {
      int value;
      auto [ptr, ec] =
          std::from_chars(part.data(), part.data() + part.size(), value);
      if (ec != std::errc() || value < 0 || value > 255) {
        throw std::out_of_range("Octet value out of range");
      }
      result = (result << 8) | static_cast<uint8_t>(value);
      ++octet;
    } catch (const std::exception &e) {
      throw std::invalid_argument("Invalid IP address: " + std::string(ipStr));
    }

    if (end == ipStr.size()) {
      break;
    }
    start = end + 1;
  }

  if (octet != 4) {
    throw std::invalid_argument("Invalid IP address: " + std::string(ipStr));
  }

  return result;
}

std::string LogReader::ipUint32ToString(uint32_t ip) {
  return std::to_string(ip >> 24) + "." + std::to_string((ip >> 16) & 0xFF) +
         "." + std::to_string((ip >> 8) & 0xFF) + "." +
         std::to_string(ip & 0xFF);
}

std::vector<uint32_t> LogReader::read_slow(std::istream &input) {
  std::vector<uint32_t> ips;

  std::string line;
  int lineNumber = 0;

  while (std::getline(input, line)) {
    ++lineNumber;

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
      ips.push_back(ipStringToUint32_slow(ipStr));
    } catch (const std::exception &e) {
      throw std::invalid_argument("Error parsing line " +
                                  std::to_string(lineNumber) + ": " + e.what());
    }
  }

  return ips;
}

std::vector<uint32_t> LogReader::read(std::istream &input) {
  std::vector<uint32_t> ips;
  ips.reserve(1000);

  std::string line;
  int lineNumber = 0;

  while (std::getline(input, line)) {
    ++lineNumber;

    if (line.empty()) {
      continue;
    }

    size_t tabPos = line.find('\t');

    // in case no IP, line starts from "\t"
    if (tabPos == 0) {
      throw std::invalid_argument("Invalid TSV format (empty IP) at line " +
                                  std::to_string(lineNumber));
    }

    // in case only IP, no "\t", line ends with IP
    size_t ipLength = (tabPos == std::string::npos) ? line.size() : tabPos;

    std::string_view ipStrView(line.data(), ipLength);

    try {
      ips.push_back(ipStringToUint32(ipStrView));
    } catch (const std::exception &e) {
      throw std::invalid_argument("Error parsing line " +
                                  std::to_string(lineNumber) + ": " + e.what());
    }
  }

  return ips;
}

void LogReader::print(const std::vector<uint32_t> &ips, std::ostream &output) {
  for (uint32_t ip : ips) {
    output << ipUint32ToString(ip) << '\n';
  }
}
