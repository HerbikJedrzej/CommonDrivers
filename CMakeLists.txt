cmake_minimum_required(VERSION 3.10.0)
project(CommonDrivers VERSION 1.0.0 LANGUAGES CXX)

set(name unitTests)

find_package(GTest REQUIRED)

macro(addOptionFlag flag ONorOFF)
  option(Flag_${flag} "Flag ${Flag_${flag}}:" ${ONorOFF})
  message(STATUS "Flag_${flag}: ${Flag_${flag}}")
  if(Flag_${flag})
    list(APPEND COMMON_FLAGS -${flag})
  endif()
endmacro()

addOptionFlag(Wall ON)
addOptionFlag(pedantic ON)
addOptionFlag(Waddress ON)
addOptionFlag(zdefs ON)
addOptionFlag(Wno-psabi ON)
addOptionFlag(Wunused-parameter ON)
addOptionFlag(Wextra ON)
addOptionFlag(Werror OFF)

# Add local sources

list(APPEND TESTS_SRC
  ./unitTests/main.cpp
  ./unitTests/src/barometer/SPL06_007_test.cpp
  ./unitTests/src/basic/GPIO_test.cpp
  ./unitTests/src/basic/I2C_test.cpp
  ./unitTests/src/basic/InterfacesConf_drivers_test.cpp
  ./unitTests/src/basic/LED_test.cpp
  ./unitTests/src/basic/SPI_test.cpp
  ./unitTests/src/basic/Timer_test.cpp
  ./unitTests/src/basic/UARTwithQueue_test.cpp
  ./unitTests/src/basic/UART_test.cpp
  ./unitTests/src/DriverIfc_test.cpp
  ./unitTests/src/IMU/LSM6DS33_test.cpp
  ./unitTests/src/IMU/MPU6050_test.cpp
  ./unitTests/src/memory/AT24C_test.cpp
  ./unitTests/src/PWM/BothChannelEngineDriver_test.cpp
  ./unitTests/src/PWM/ESC_test.cpp
  ./unitTests/src/PWM/PWM_test.cpp
  ./unitTests/src/PWM/Servo_test.cpp
  ./unitTests/src/UltrasonicDistanceSensor/US-015_test.cpp
  ./unitTests/src/wirelessComunication/nRF24HalfDuplex_test.cpp
  ./unitTests/src/wirelessComunication/nRF24SinglePlex_test.cpp
  ./unitTests/src/wirelessComunication/RadioParser_test.cpp
)

# Add sources from src

function(prepend_list var prefix)
    set(temp "")
    foreach(f ${${var}})
        list(APPEND temp "${prefix}${f}")
    endforeach()
    set(${var} "${temp}" PARENT_SCOPE)
endfunction()

include(src/src.cmake)

prepend_list(INCLUDE_PATHS "src/")
prepend_list(SOURCE_PATHS  "src/")

# define target

add_executable(${name} ${TESTS_SRC} ${SOURCE_PATHS})
target_link_libraries(${name} LINK_PRIVATE ${GTEST_LIBRARIES})
set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)

separate_arguments(ADDITIONAL_FLAGS)
target_compile_options(${name} PRIVATE ${ADDITIONAL_FLAGS})
target_compile_options(${name} PRIVATE ${COMMON_FLAGS})

# Add headers

target_include_directories(${name} PRIVATE ${INCLUDE_PATHS})
target_include_directories(${name} PRIVATE ${GTEST_INCLUDE_DIRS})
target_include_directories(${name} PRIVATE unitTests)
target_include_directories(${name} PRIVATE unitTests/mocks)
target_include_directories(${name} PRIVATE unitTests/src)
target_include_directories(${name} PRIVATE unitTests/src/basic)

# Execute after compile

add_custom_command(TARGET ${name} POST_BUILD
  COMMAND 
    ./${name}
COMMENT "Running unit tests."
)
