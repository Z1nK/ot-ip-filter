#include <charconv>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <log-reader/log_reader.hpp>

namespace {

using Clock = std::chrono::steady_clock;
using Milliseconds = std::chrono::duration<double, std::milli>;

struct TimingStats {
  double minMs;
  double maxMs;
  double avgMs;
};

std::string makeInput(std::size_t lineCount) {
  const std::string line = "46.70.1.2\t100\t200\n";

  std::string input;
  input.reserve(line.size() * lineCount);
  for (std::size_t i = 0; i < lineCount; ++i) {
    input += line;
  }

  return input;
}

template <typename ReaderCall>
TimingStats measureStats(const std::string &inputData, ReaderCall &&readerCall,
                         int iterations, std::size_t expectedCount,
                         volatile std::uint64_t &sink) {
  double minMs = std::numeric_limits<double>::max();
  double maxMs = 0.0;
  double sumMs = 0.0;

  for (int i = 0; i < iterations; ++i) {
    std::istringstream input(inputData);

    const auto start = Clock::now();
    const std::vector<std::uint32_t> ips = readerCall(input);
    const auto end = Clock::now();

    if (ips.size() != expectedCount) {
      throw std::runtime_error("Unexpected parsed line count");
    }

    if (!ips.empty()) {
      sink += ips.front();
      sink += ips.back();
    }

    const double elapsedMs = Milliseconds(end - start).count();
    if (elapsedMs < minMs) {
      minMs = elapsedMs;
    }
    if (elapsedMs > maxMs) {
      maxMs = elapsedMs;
    }
    sumMs += elapsedMs;
  }

  return {minMs, maxMs, sumMs / static_cast<double>(iterations)};
}

} // namespace

int main(int argc, char **argv) {
  const std::vector<std::size_t> sizes = {
      1000, 10000, 1000000, 5000000, 10000000,
  };

  constexpr int kDefaultIterationsPerCase = 3;
  int iterationsPerCase = kDefaultIterationsPerCase;

  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [iterations]\n";
    return 1;
  }

  if (argc == 2) {
    int parsedIterations = 0;
    const std::string value = argv[1];
    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(),
                                     parsedIterations);

    if (ec != std::errc() || ptr != value.data() + value.size() ||
        parsedIterations <= 0) {
      std::cerr << "Invalid iterations value: " << value
                << ". Expected a positive integer.\n";
      std::cerr << "Usage: " << argv[0] << " [iterations]\n";
      return 1;
    }

    iterationsPerCase = parsedIterations;
  }

  volatile std::uint64_t sink = 0;

  std::cout << "Benchmark: LogReader::read vs LogReader::read_slow\n";
  std::cout << "Each case runs " << iterationsPerCase
            << " times, best time is reported.\n\n";

  std::cout << std::left << std::setw(12) << "lines" << std::setw(12)
            << "method" << std::right << std::setw(14) << "min (ms)"
            << std::setw(14) << "avg (ms)" << std::setw(14) << "max (ms)"
            << std::setw(14) << "speedup" << '\n';
  std::cout << std::string(80, '-') << '\n';

  LogReader reader;

  for (const std::size_t lineCount : sizes) {
    const std::string inputData = makeInput(lineCount);

    const TimingStats readStats = measureStats(
        inputData, [&](std::istream &in) { return reader.read(in); },
        iterationsPerCase, lineCount, sink);
    const TimingStats readSlowStats = measureStats(
        inputData, [&](std::istream &in) { return reader.read_slow(in); },
        iterationsPerCase, lineCount, sink);

    const double speedupByAvg = readSlowStats.avgMs / readStats.avgMs;

    std::cout << std::left << std::setw(12) << lineCount << std::setw(12)
              << "read" << std::right << std::setw(14) << std::fixed
              << std::setprecision(3) << readStats.minMs << std::setw(14)
              << readStats.avgMs << std::setw(14) << readStats.maxMs
              << std::setw(14) << "1.00x" << '\n';

    std::cout << std::left << std::setw(12) << "" << std::setw(12)
              << "read_slow" << std::right << std::setw(14) << std::fixed
              << std::setprecision(3) << readSlowStats.minMs << std::setw(14)
              << readSlowStats.avgMs << std::setw(14) << readSlowStats.maxMs
              << std::setw(13) << std::fixed << std::setprecision(2)
              << speedupByAvg << 'x' << '\n';
    std::cout << std::string(80, '-') << '\n';
  }

  std::cout << "\nChecksum (ignore): " << sink << '\n';

  return 0;
}
