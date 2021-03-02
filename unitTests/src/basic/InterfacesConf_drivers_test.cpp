#include <gtest/gtest.h>
#include <InterfacesConf_drivers_test.hh>

namespace drivers_test{

	SPI_HandleTypeDef hspi;
	I2C_HandleTypeDef hi2c;
	UART_HandleTypeDef huart;

    GPIO_TypeDef gpioAobj, gpioBobj, gpioCobj, gpioDobj, gpioEobj;
    GPIO_TypeDef *GPIOA = &gpioAobj, *GPIOB = &gpioBobj, *GPIOC = &gpioCobj, *GPIOD = &gpioDobj, *GPIOE = &gpioEobj;
	GPIO_TypeDef *PORTS[5] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};

		void SPI_HandleTypeDef::clear(){
			spiIsBussy = false;
			pinState = true;
			dataIn.clear();
			dataOut.clear();
			size = 0;
			Timeout = 0;
			dma = false;
		}
		void UART_HandleTypeDef::clear(){
			uartIsBussy = false;
			dataIn.clear();
			dataOut.clear();
			size = 0;
			Timeout = 0;
			dma = false;
		}
		void I2C_HandleTypeDef::clear(){
			i2cIsBussy = false;
			data.clear();
			DevAddress = 0;
			MemAddSize = 0;
			MemAddress = 0;
			Timeout = 0;
			dma = false;
		}
		GpioIrqTest::GpioIrqTest():Drivers::DriverIfc(HAL_Delay){}
		bool GpioIrqTest::getWill(){return willBeCalled;}
		bool GpioIrqTest::getWas(){return wasCalled;}
		void GpioIrqTest::callMe(){
			EXPECT_TRUE(wasCalled);
			EXPECT_FALSE(willBeCalled);
			willBeCalled = true;
			wasCalled = false;
		}
	    void GpioIrqTest::handleFinish(DriverIfc*){
			EXPECT_TRUE(willBeCalled);
			willBeCalled = false;
			wasCalled = true;
		}
		SPIIrqTest::SPIIrqTest():Drivers::DriverIfc(HAL_Delay){}
		bool SPIIrqTest::getWill(){return willBeCalled;}
		bool SPIIrqTest::getWas(){return wasCalled;}
		void SPIIrqTest::callMe(){
			EXPECT_TRUE(wasCalled);
			EXPECT_FALSE(willBeCalled);
			willBeCalled = true;
			wasCalled = false;
		}
	    void SPIIrqTest::handleFinish(DriverIfc*){
			EXPECT_TRUE(willBeCalled);
			willBeCalled = false;
			wasCalled = true;
		}
		UARTIrqTest::UARTIrqTest():Drivers::DriverIfc(HAL_Delay){}
		bool UARTIrqTest::getWill(){return willBeCalled;}
		bool UARTIrqTest::getWas(){return wasCalled;}
		void UARTIrqTest::callMe(){
			EXPECT_TRUE(wasCalled);
			EXPECT_FALSE(willBeCalled);
			willBeCalled = true;
			wasCalled = false;
		}
	    void UARTIrqTest::handleFinish(DriverIfc*){
			EXPECT_TRUE(willBeCalled);
			willBeCalled = false;
			wasCalled = true;
		}
		I2CIrqTest::I2CIrqTest():Drivers::DriverIfc(HAL_Delay){}
		bool I2CIrqTest::getWill(){return willBeCalled;}
		bool I2CIrqTest::getWas(){return wasCalled;}
		void I2CIrqTest::callMe(){
			EXPECT_TRUE(wasCalled);
			EXPECT_FALSE(willBeCalled);
			willBeCalled = true;
			wasCalled = false;
		}
	    void I2CIrqTest::handleFinish(DriverIfc*){
			EXPECT_TRUE(willBeCalled);
			willBeCalled = false;
			wasCalled = true;
		}
    
    uint32_t delayPeriod = 0;
    void HAL_Delay(uint32_t value){
        delayPeriod = value;
    }
	
    HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef* obj){
		return obj->uartIsBussy? HAL_UART_StateTypeDef::errUART : HAL_UART_StateTypeDef::HAL_UART_STATE_READY; 
	}
	HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout){
		if(huart->uartIsBussy){
			if(huart->testErrFunc != nullptr)
				huart->testErrFunc();
			return HAL_StatusTypeDef::HAL_BUSY;
		}
		if(Timeout < 10){
			if(huart->testErrFunc != nullptr)
				huart->testErrFunc();
			return HAL_StatusTypeDef::HAL_TIMEOUT;
		}
		if(huart->testFuncIn != nullptr)
			huart->testFuncIn();
		huart->uartIsBussy = false;
		for(uint16_t i = 0; i < Size; i++){
			huart->dataOut.push_back(pData[i]);
		}
		huart->size = Size;
		huart->Timeout = Timeout;
		huart->dma = false;
		if(huart->testFuncOut != nullptr)
			huart->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
		if(huart->uartIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		if(huart->testFuncIn != nullptr)
			huart->testFuncIn();
		huart->uartIsBussy = true;
		for(uint16_t i = 0; i < Size; i++){
			huart->dataOut.push_back(pData[i]);
		}
		huart->size = Size;
		huart->Timeout = 0;
		huart->dma = true;
		if(huart->testFuncOut != nullptr)
			huart->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout){
		if(huart->uartIsBussy){
			if(huart->testErrFunc != nullptr)
				huart->testErrFunc();
			return HAL_StatusTypeDef::HAL_BUSY;
		}
		if(Timeout < 10){
			if(huart->testErrFunc != nullptr)
				huart->testErrFunc();
			return HAL_StatusTypeDef::HAL_TIMEOUT;
		}
		if(huart->testFuncIn != nullptr)
			huart->testFuncIn();
		huart->uartIsBussy = false;
		for(uint16_t i = 0; i < Size; i++){
			if(i < huart->dataIn.size())
				pData[i] = huart->dataIn[i];
			else
				pData[i] = 0;
		}
		huart->size = Size;
		huart->Timeout = Timeout;
		huart->dma = false;
		if(huart->testFuncOut != nullptr)
			huart->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size){
		if(huart->uartIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		if(huart->testFuncIn != nullptr)
			huart->testFuncIn();
		huart->uartIsBussy = true;
		for(uint16_t i = 0; i < Size; i++){
			if(i < huart->dataIn.size())
				pData[i] = huart->dataIn[i];
			else
				pData[i] = 0;
		}
		huart->size = Size;
		huart->Timeout = 0;
		huart->dma = true;
		if(huart->testFuncOut != nullptr)
			huart->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}


    HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef* obj){
		return obj->spiIsBussy? HAL_SPI_StateTypeDef::errSPI : HAL_SPI_StateTypeDef::HAL_SPI_STATE_READY; 
	}
	HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size){
		hspi->pinState = gpio.pin(OutputList::spiSlaveSelect);
		if(hspi->spiIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		if(hspi->testFuncIn != nullptr)
			hspi->testFuncIn();
		hspi->spiIsBussy = true;
		for(uint16_t i = 0; i < Size; i++){
			hspi->dataOut.push_back(pTxData[i]);
			if(i < hspi->dataIn.size())
				pRxData[i] = hspi->dataIn[i];
			else
				pRxData[i] = 0;
		}
		hspi->pinState = gpio.pin(OutputList::spiSlaveSelect);
		hspi->size = Size;
		hspi->Timeout = 0;
		hspi->dma = true;
		if(hspi->testFuncOut != nullptr)
			hspi->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout){
		hspi->pinState = gpio.pin(OutputList::spiSlaveSelect);
		if(hspi->spiIsBussy){
			if(hspi->testErrFunc != nullptr)
				hspi->testErrFunc();
			return HAL_StatusTypeDef::HAL_BUSY;
		}
		if(Timeout < 10){
			if(hspi->testErrFunc != nullptr)
				hspi->testErrFunc();
			return HAL_StatusTypeDef::HAL_TIMEOUT;
		}
		if(hspi->testFuncIn != nullptr)
			hspi->testFuncIn();
		hspi->spiIsBussy = false;
		for(uint16_t i = 0; i < Size; i++){
			hspi->dataOut.push_back(pTxData[i]);
			if(i < hspi->dataIn.size())
				pRxData[i] = hspi->dataIn[i];
			else
				pRxData[i] = 0;
		}
		hspi->pinState = gpio.pin(OutputList::spiSlaveSelect);
		hspi->size = Size;
		hspi->Timeout = Timeout;
		hspi->dma = false;
		if(hspi->testFuncOut != nullptr)
			hspi->testFuncOut();
		return HAL_StatusTypeDef::HAL_OK;
	}

	HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef* obj){
		return obj->i2cIsBussy? HAL_I2C_StateTypeDef::errI2C : HAL_I2C_StateTypeDef::HAL_I2C_STATE_READY; 
	}
	HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size){
		if(hi2c->i2cIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		hi2c->i2cIsBussy = true;
		for(uint16_t i = 0; i < Size; i++)
			hi2c->data.push_back(pData[i]);
		hi2c->DevAddress = DevAddress;
		hi2c->MemAddSize = MemAddSize;
		hi2c->MemAddress = MemAddress;
		hi2c->Timeout = 0;
		hi2c->dma = true;
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size){
		if(hi2c->i2cIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		hi2c->i2cIsBussy = true;
		for(uint16_t i = 0; i < Size; i++)
			if(i < hi2c->data.size())
				pData[i] = hi2c->data[i];
			else
				pData[i] = 255;
		hi2c->DevAddress = DevAddress;
		hi2c->MemAddSize = MemAddSize;
		hi2c->MemAddress = MemAddress;
		hi2c->Timeout = 0;
		hi2c->dma = true;
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
		if(hi2c->i2cIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		if(Timeout < 10)
			return HAL_StatusTypeDef::HAL_TIMEOUT;
		hi2c->i2cIsBussy = false;
		for(uint16_t i = 0; i < Size; i++)
			hi2c->data.push_back(pData[i]);
		hi2c->DevAddress = DevAddress;
		hi2c->MemAddSize = MemAddSize;
		hi2c->MemAddress = MemAddress;
		hi2c->Timeout = Timeout;
		hi2c->dma = false;
		return HAL_StatusTypeDef::HAL_OK;
	}
	HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout){
		if(hi2c->i2cIsBussy)
			return HAL_StatusTypeDef::HAL_BUSY;
		if(Timeout < 10)
			return HAL_StatusTypeDef::HAL_TIMEOUT;
		hi2c->i2cIsBussy = false;
		for(uint16_t i = 0; i < Size; i++)
			if(i < hi2c->data.size())
				pData[i] = hi2c->data[i];
			else
				pData[i] = 255;
		hi2c->DevAddress = DevAddress;
		hi2c->MemAddSize = MemAddSize;
		hi2c->MemAddress = MemAddress;
		hi2c->Timeout = Timeout;
		hi2c->dma = false;
		return HAL_StatusTypeDef::HAL_OK;
	}



	Drivers::GPIO gpio(
		[](const OutputList pin, const bool state)->void{
			if(pin == OutputList::none)
				return;
			uint8_t portId = static_cast<uint8_t>(pin) / 16;
			uint8_t pinId = static_cast<uint8_t>(pin) % 16;
			if(state)
				PORTS[portId]->ODR |= (1 << pinId);
			else
				PORTS[portId]->ODR &= ~((uint32_t)(1 << pinId));
		},
		[](const OutputList pin)->bool{
			uint8_t portId = static_cast<uint8_t>(pin) / 16;
			uint8_t pinId = static_cast<uint8_t>(pin) % 16;
			uint32_t tmpState = PORTS[portId]->ODR;
			return ((tmpState & (1 << pinId)) != 0);
		},
		[](const InputList pin)->bool{
			uint8_t portId = static_cast<uint8_t>(pin) / 16;
			uint8_t pinId = static_cast<uint8_t>(pin) % 16;
			uint32_t tmpState = PORTS[portId]->IDR;
			return ((tmpState & (1 << pinId)) != 0);
		},
		HAL_Delay
	);

	Drivers::UART uart(
	[](uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
		auto returned = HAL_UART_Receive(&huart, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[](uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
		auto returned = HAL_UART_Transmit(&huart, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[](uint8_t *pData, uint16_t Size)->Drivers::BusStatus{
		auto returned = HAL_UART_Transmit_DMA(&huart, pData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[]()->bool{return(HAL_UART_GetState(&huart) != HAL_UART_StateTypeDef::HAL_UART_STATE_READY);},
	HAL_Delay
	);

	Drivers::QueueUART<25, 50> queueUart(
	[](uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
		auto returned = HAL_UART_Receive(&huart, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[](uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
		auto returned = HAL_UART_Transmit(&huart, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[](uint8_t *pData, uint16_t Size)->Drivers::BusStatus{
		auto returned = HAL_UART_Transmit_DMA(&huart, pData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[]()->bool{return(HAL_UART_GetState(&huart) != HAL_UART_StateTypeDef::HAL_UART_STATE_READY);},
	HAL_Delay
	);

	Drivers::SPI spi(
	gpio, 
	[](uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)->Drivers::BusStatus{
		auto returned = HAL_SPI_TransmitReceive_DMA(&hspi, pTxData, pRxData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[](uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
		auto returned = HAL_SPI_TransmitReceive(&hspi, pTxData, pRxData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return Drivers::BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return Drivers::BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return Drivers::BusStatus::TIMEOUT;
		return Drivers::BusStatus::ERR;
	},
	[]()->bool{return(HAL_SPI_GetState(&hspi) != HAL_SPI_StateTypeDef::HAL_SPI_STATE_READY);},
	[]()->bool{return true;},
	HAL_Delay
	);

	Drivers::I2C i2c(
		[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)->Drivers::BusStatus{
			auto returned = HAL_I2C_Mem_Write_DMA(&hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
			if(returned == HAL_StatusTypeDef::HAL_OK)
				return Drivers::BusStatus::OK;
			if(returned == HAL_StatusTypeDef::HAL_BUSY)
				return Drivers::BusStatus::BUSY;
			if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
				return Drivers::BusStatus::TIMEOUT;
			return Drivers::BusStatus::ERR;
		},
		[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)->Drivers::BusStatus{
			auto returned = HAL_I2C_Mem_Read_DMA(&hi2c, DevAddress, MemAddress, MemAddSize, pData, Size);
			if(returned == HAL_StatusTypeDef::HAL_OK)
				return Drivers::BusStatus::OK;
			if(returned == HAL_StatusTypeDef::HAL_BUSY)
				return Drivers::BusStatus::BUSY;
			if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
				return Drivers::BusStatus::TIMEOUT;
			return Drivers::BusStatus::ERR;
		},
		[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
			auto returned = HAL_I2C_Mem_Write(&hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
			if(returned == HAL_StatusTypeDef::HAL_OK)
				return Drivers::BusStatus::OK;
			if(returned == HAL_StatusTypeDef::HAL_BUSY)
				return Drivers::BusStatus::BUSY;
			if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
				return Drivers::BusStatus::TIMEOUT;
			return Drivers::BusStatus::ERR;
		},
		[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)->Drivers::BusStatus{
			auto returned = HAL_I2C_Mem_Read(&hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
			if(returned == HAL_StatusTypeDef::HAL_OK)
				return Drivers::BusStatus::OK;
			if(returned == HAL_StatusTypeDef::HAL_BUSY)
				return Drivers::BusStatus::BUSY;
			if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
				return Drivers::BusStatus::TIMEOUT;
			return Drivers::BusStatus::ERR;
		},
		[]()->bool{return(HAL_I2C_GetState(&hi2c) != HAL_I2C_StateTypeDef::HAL_I2C_STATE_READY);},
		HAL_Delay
	);

	uint8_t testInCounter = 0;
	uint8_t testOutCounter = 0;
	uint16_t postTestInCounter = 0;
	uint16_t postTestOutCounter = 0;
	std::unique_ptr<Registersmap> radioRegsIn(nullptr);
	std::unique_ptr<Registersmap> radioRegsOut(nullptr);

	void testIn(){
		EXPECT_FALSE(gpio.pin(OutputList::radioCSN));
		if(!radioRegsIn){
			EXPECT_TRUE(radioRegsIn);
			return;
		}
		if(testInCounter < radioRegsIn->size()){
			auto& row = (*radioRegsIn)[testInCounter];
			hspi.clear();
			for(uint8_t i = 0; i < row.size(); i++)
				hspi.dataIn.push_back(row[i]);
			testInCounter++;
		}
		if(testInCounter >= radioRegsIn->size()){
			radioRegsIn.reset(nullptr);
			hspi.testFuncIn = nullptr;
			postTestInCounter = testInCounter;
			testInCounter = 0;
		}
	}

	void testOut(){
		EXPECT_FALSE(gpio.pin(OutputList::radioCSN));
		if(!radioRegsOut){
			EXPECT_TRUE(radioRegsOut);
			return;
		}
		if(testOutCounter < radioRegsOut->size()){
			auto& row = (*radioRegsOut)[testOutCounter];
			if(hspi.size != row.size()){
				EXPECT_EQ(hspi.size, row.size());
				return;
			}
			for(uint16_t i = 0; i < row.size(); i++)
				EXPECT_EQ(hspi.dataOut[i], row[i]) << "Failure for i = " << i;
			testOutCounter++;
			hspi.clear();
		}
		if(testOutCounter >= radioRegsOut->size()){
			radioRegsOut.reset(nullptr);
			hspi.testFuncOut = nullptr;
			postTestOutCounter = testOutCounter;
			testOutCounter = 0;
			hspi.clear();
		}
	}

}