set(SERVER_LIB_NAME log-reader)

add_library(${SERVER_LIB_NAME} STATIC "")

add_library(log-reader::log-reader ALIAS ${SERVER_LIB_NAME})

target_sources(${SERVER_LIB_NAME} PRIVATE
  log_reader.cpp
)

target_include_directories(${SERVER_LIB_NAME} PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
  )


