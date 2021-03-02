#pragma once

#include <gtest/gtest.h>
#include <GPIOconf.hh>
#include <GPIO.hh>
#include <UART.hh>
#include <UARTwithQueue.hh>
#include <SPI.hh>
#include <I2C.hh>

namespace drivers_test{
    struct GPIO_TypeDef{
        uint16_t ODR = 0;
        uint16_t IDR = 0;
    };
	enum HAL_UART_StateTypeDef{HAL_UART_STATE_READY, errUART};
	enum HAL_SPI_StateTypeDef{HAL_SPI_STATE_READY, errSPI};
	enum HAL_StatusTypeDef{HAL_OK, HAL_BUSY, HAL_TIMEOUT};
	enum HAL_I2C_StateTypeDef{HAL_I2C_STATE_READY, errI2C};

    struct SPI_HandleTypeDef{
		bool spiIsBussy = false;
		bool pinState;
		std::vector<uint8_t> dataIn;
		std::vector<uint8_t> dataOut;
		uint16_t size;
		uint32_t Timeout;
		bool dma;
		void (*testFuncIn)(void) = {nullptr};
		void (*testFuncOut)(void) = {nullptr};
		void (*testErrFunc)(void) = {nullptr};
		void clear();
    };

	struct UART_HandleTypeDef{
		bool uartIsBussy = false;
		std::vector<uint8_t> dataIn;
		std::vector<uint8_t> dataOut;
		uint16_t size;
		uint32_t Timeout;
		bool dma;
		void (*testFuncIn)(void) = {nullptr};
		void (*testFuncOut)(void) = {nullptr};
		void (*testErrFunc)(void) = {nullptr};
		void clear();
	};

    struct I2C_HandleTypeDef{
		bool i2cIsBussy = false;
		std::vector<uint8_t> data;
		uint16_t DevAddress;
		uint16_t MemAddSize;
		uint16_t MemAddress;
		uint32_t Timeout;
		bool dma;
		void clear();
    };

	class GpioIrqTest : public Drivers::DriverIfc{
		bool willBeCalled = false;
		bool wasCalled = true;
	public:
		GpioIrqTest();
		bool getWill();
		bool getWas();
		void callMe();
	private:
	    void handleFinish(DriverIfc*) override;
	};
	class SPIIrqTest : public Drivers::DriverIfc{
		bool willBeCalled = false;
		bool wasCalled = true;
	public:
		SPIIrqTest();
		bool getWill();
		bool getWas();
		void callMe();
	private:
	    void handleFinish(DriverIfc*) override;
	};

	class UARTIrqTest : public Drivers::DriverIfc{
		bool willBeCalled = false;
		bool wasCalled = true;
	public:
		UARTIrqTest();
		bool getWill();
		bool getWas();
		void callMe();
	private:
	    void handleFinish(DriverIfc*) override;
	};

	class I2CIrqTest : public Drivers::DriverIfc{
		bool willBeCalled = false;
		bool wasCalled = true;
	public:
		I2CIrqTest();
		bool getWill();
		bool getWas();
		void callMe();
	private:
	    void handleFinish(DriverIfc*) override;
	};

	extern uint32_t delayPeriod;
    void HAL_Delay(uint32_t value);

	HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef* obj);
	HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
	HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

	HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef* obj);
	HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);

	HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef* obj);
	HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
	HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
	HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
	HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

	extern Drivers::I2C i2c;
	extern Drivers::SPI spi;
	extern Drivers::UART uart;
	extern Drivers::QueueUART<25, 50> queueUart;
	extern Drivers::GPIO gpio;
	extern UART_HandleTypeDef huart;
	extern SPI_HandleTypeDef hspi;
	extern I2C_HandleTypeDef hi2c;

	using Registersmap = std::vector<std::vector<uint8_t> >;
	extern uint8_t testInCounter;
	extern uint8_t testOutCounter;
	extern uint16_t postTestInCounter;
	extern uint16_t postTestOutCounter;
	extern std::unique_ptr<Registersmap> radioRegsIn;
	extern std::unique_ptr<Registersmap> radioRegsOut;

	void testIn();
	void testOut();

}
