add_executable(ip_filter ip_filter.cpp)

target_compile_features(ip_filter PUBLIC cxx_std_17)

target_link_libraries(ip_filter PRIVATE
                        log-reader::log-reader   
)

if (MSVC)
    target_compile_options(ip_filter PRIVATE /W4)
else()
    target_compile_options(ip_filter PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Copy *.tsv files to build directory

copy_tsv_files(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Installation

install(TARGETS ip_filter RUNTIME DESTINATION bin)