#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include <log-reader/log_reader.hpp>

TEST(LogReaderTest, ReadParsesValidTsvAndSkipsEmptyLines) {
	std::istringstream input(
			"1.2.3.4\t10\t20\n"
			"\n"
			"46.70.1.2\t30\t40\n");

	LogReader reader;
	const std::vector<uint32_t> ips = reader.read(input);

	ASSERT_EQ(ips.size(), 2u);
	EXPECT_EQ(ips[0], 0x01020304u);
	EXPECT_EQ(ips[1], 0x2E460102u);
}

TEST(LogReaderTest, ReadAcceptsLineWithOnlyIpColumn) {
	std::istringstream input("1.2.3.4\n");

	LogReader reader;
	const std::vector<uint32_t> ips = reader.read(input);

	ASSERT_EQ(ips.size(), 1u);
	EXPECT_EQ(ips[0], 0x01020304u);
}

TEST(LogReaderTest, ReadThrowsOnInvalidIpAddress) {
	std::istringstream input("300.2.3.4\t10\t20\n");

	LogReader reader;
	EXPECT_THROW(reader.read(input), std::invalid_argument);
}

TEST(LogReaderTest, PrintOutputsDottedIpPerLine) {
	const std::vector<uint32_t> ips = {
			0x01020304u,
			0x2E460102u,
	};
	std::ostringstream output;

	LogReader reader;
	reader.print(ips, output);

	EXPECT_EQ(output.str(), "1.2.3.4\n46.70.1.2\n");
}
