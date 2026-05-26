#pragma once

#include <cstdint>
#include <iostream>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

class LogReader {
public:
  /**
   * Read TSV data from an input stream and parse log entries
   * @param input Input stream with TSV rows
   * @return Vector of IP addresses as uint32_t
   */
  static std::vector<uint32_t> read_slow(std::istream &input);

  /**
   * Read TSV data from an input stream and parse log entries
   * @param input Input stream with TSV rows
   * @return Vector of IP addresses as uint32_t
   */
  static std::vector<uint32_t> read(std::istream &input);

  /**
   * Print IP addresses as dotted IPv4 format to an output stream
   * @param ips Vector of IP addresses as uint32_t
   * @param output Output stream, defaults to stdout
   */
  static void print(const std::vector<uint32_t> &ips,
                    std::ostream &output = std::cout);

private:
  /**
   * Convert IPv4 address string to uint32_t
   * @param ipStr IP address string (e.g., "192.168.1.1")
   * @return IP address as uint32_t in host byte order
   */
  static uint32_t ipStringToUint32_slow(const std::string &ipStr);

  /**
   * Convert IPv4 address string to uint32_t
   * @param ipStr IP address string (e.g., "192.168.1.1")
   * @return IP address as uint32_t in host byte order
   */
  static uint32_t ipStringToUint32(std::string_view ipStr);

  /**
   * Convert uint32_t IPv4 address to dotted-decimal string
   * @param ip IP address in host byte order
   * @return Dotted-decimal IPv4 string
   */
  static std::string ipUint32ToString(uint32_t ip);
};
