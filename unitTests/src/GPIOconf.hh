#pragma once

enum OutputList{
  none = -1,
  LED_Blue2 = 10,
  LED_Yellow1,
  LED_Yellow2,
  LED_Red1 = 15,
  MemoryWriteProtect = 21,
  LED_Green = 26,
  LED_Blue1,
  LED_Yellow3 = 42,
  LED_Red2  = 50,
  LDC_RS = 45,
  LDC_RW,
  LDC_E,    
  radioCSN,
  radioCE = 28,
  outPinA0 = 0,
  outPinA1 = 1,
  outPinA10 = 10,
  outPinB2 = 18,
  outPinB9 = 25,
  outPinC0 = 32,
  outPinC15 = 47,
  outPinD5 = 53,
  outPinE3 = 67,
  spiSlaveSelect = 10,
  ledPin1 = 25,
  ledPin2 = 26,
  Trig
};

enum InputList{
  JoyLeft = 5,
  OptStart,
  OptSpec,
  Encoder = 41,
  JoyRight = 43,
  SwitchRight = 44,
  OptStop = 17,
  OptFly = 18,
  SwitchLeft = 20,
  inPinC14 = 46,
  inPinC15 = 47,
  Echo
};

enum InterruptInputList{
  irqPinA11 = 0,
  irqPinB11,
  irqPinB12,
  irqPinB13,
  irqPinC1,
  irqPinC10,
  radioIRQ,
  SPL06_007_IrqPin,
  EchoIrq,
  endOfInterruptInputList
};

enum ADClist{
	battery,
	axisLY,
	axisLX,
	axisRY,
	axisRX,
  end
};
