#pragma once

#include "GPIOconf.hh"

namespace Drivers{
GPIO_TypeDef *PORTS[5] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
bool doCircle = false;
bool getMesurementForAHRS = false;
unsigned int time = 0;

GPIO gpio(
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

SPI spi2(
	gpio, 
	[](uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)->BusStatus{
		auto returned = HAL_SPI_TransmitReceive_DMA(&hspi2, pTxData, pRxData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[](uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)->BusStatus{
		auto returned = HAL_SPI_TransmitReceive(&hspi2, pTxData, pRxData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[]()->bool{return(HAL_SPI_GetState(&hspi2) != HAL_SPI_StateTypeDef::HAL_SPI_STATE_READY);},
	HAL_Delay
);

I2C i2c1(
	[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)->BusStatus{
		auto returned = HAL_I2C_Mem_Write_DMA(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size)->BusStatus{
		auto returned = HAL_I2C_Mem_Read_DMA(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)->BusStatus{
		auto returned = HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[](uint8_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)->BusStatus{
		auto returned = HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
		if(returned == HAL_StatusTypeDef::HAL_OK)
			return BusStatus::OK;
		if(returned == HAL_StatusTypeDef::HAL_BUSY)
			return BusStatus::BUSY;
		if(returned == HAL_StatusTypeDef::HAL_TIMEOUT)
			return BusStatus::TIMEOUT;
		return BusStatus::ERR;
	},
	[]()->bool{return(HAL_I2C_GetState(&hi2c1) != HAL_I2C_StateTypeDef::HAL_I2C_STATE_READY);},
	HAL_Delay
);

}
