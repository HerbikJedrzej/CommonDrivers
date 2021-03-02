macro(addFlag flag)
  if(NOT ADDITIONAL_FLAGS MATCHES ${flag})
    set(ADDITIONAL_FLAGS "${ADDITIONAL_FLAGS} ${flag}")
  endif()
endmacro()

addFlag("-lm")

list(APPEND INCLUDE_PATHS
  barometer
  basic
  IMU
  memory
  PWM
  UltrasonicDistanceSensor
  wirelessComunication
  .
)

list(APPEND SOURCE_PATHS
  barometer/SPL06_007.cpp
  basic/GPIO.cpp
  basic/I2C.cpp
  basic/LED.cpp
  basic/SPI.cpp
  basic/Timer.cpp
  basic/UART.cpp
  IMU/LSM6DS33.cpp
  IMU/MPU6050.cpp
  memory/AT24C.cpp
  UltrasonicDistanceSensor/US-015.cpp
  wirelessComunication/nRF24HalfDuplex.cpp
  wirelessComunication/nRF24SinglePlex.cpp
  wirelessComunication/RadioParser.cpp
  DriverIfc.cpp
)