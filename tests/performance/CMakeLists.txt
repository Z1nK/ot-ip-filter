add_executable(performance_tests performance.cpp)

set_target_properties(performance_tests PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
)

target_link_libraries(performance_tests PRIVATE
	log-reader::log-reader
)
