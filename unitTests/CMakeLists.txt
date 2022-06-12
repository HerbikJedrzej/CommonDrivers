cmake_minimum_required(VERSION 3.10.0)

set(name unitTests)

find_package(GTest REQUIRED)

# Add local sources

list(APPEND TESTS_SRC
  main.cpp
  src/barometer/SPL06_007_test.cpp
  src/basic/GPIO_test.cpp
  src/basic/I2C_test.cpp
  src/basic/InterfacesConf_drivers_test.cpp
  src/basic/LED_test.cpp
  src/basic/SPI_test.cpp
  src/basic/Timer_test.cpp
  src/basic/UARTwithQueue_test.cpp
  src/basic/UART_test.cpp
  src/DriverIfc_test.cpp
  src/IMU/LSM6DS33_test.cpp
  src/IMU/MPU6050_test.cpp
  src/memory/AT24C_test.cpp
  src/PWM/BothChannelEngineDriver_test.cpp
  src/PWM/ESC_test.cpp
  src/PWM/PWM_test.cpp
  src/PWM/Servo_test.cpp
  src/UltrasonicDistanceSensor/US-015_test.cpp
  src/wirelessComunication/nRF24HalfDuplex_test.cpp
  src/wirelessComunication/nRF24SinglePlex_test.cpp
  src/wirelessComunication/RadioParser_test.cpp
)

# Add sources from src

function(prepend_list var prefix)
    set(temp "")
    foreach(f ${${var}})
        list(APPEND temp "${prefix}${f}")
    endforeach()
    set(${var} "${temp}" PARENT_SCOPE)
endfunction()

include(../src/src.cmake)

prepend_list(INCLUDE_PATHS "../src/")
prepend_list(SOURCE_PATHS  "../src/")

# define target

add_executable(${name} ${TESTS_SRC} ${SOURCE_PATHS})
target_link_libraries(${name} LINK_PRIVATE ${GTEST_LIBRARIES} pthread)
set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)

separate_arguments(ADDITIONAL_FLAGS)
target_compile_options(${name} PRIVATE ${ADDITIONAL_FLAGS})
target_compile_options(${name} PRIVATE ${COMMON_FLAGS})

# Add headers

target_include_directories(${name} PRIVATE .)
target_include_directories(${name} PRIVATE mocks)
target_include_directories(${name} PRIVATE src)
target_include_directories(${name} PRIVATE src/basic)
target_include_directories(${name} PRIVATE ${INCLUDE_PATHS})
target_include_directories(${name} PRIVATE ${GTEST_INCLUDE_DIRS})

# Execute after compile

add_custom_command(TARGET ${name} POST_BUILD
  COMMAND 
    ./${name}
COMMENT "Running unit tests."
)
