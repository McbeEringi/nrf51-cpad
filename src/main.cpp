#include <Arduino.h>
#include <SPI.h>
#include <BLEHIDPeripheral.h>
#include <BLEMouse.h>
#define LED 17

void xtal_init(){// https://trac.switch-science.com/wiki/TY51822r3-uV5
	if (*(uint32_t *)0x10001008 == 0xFFFFFFFF) {
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		*(uint32_t *)0x10001008 = 0xFFFFFF00;
		NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos; while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NVIC_SystemReset();
		while(1);
	}
}

BLEHIDPeripheral hidp=BLEHIDPeripheral();

void setup() {
	xtal_init();
	pinMode(LED,OUTPUT);digitalWrite(LED,HIGH);delay(100);
	hidp.setDeviceName("AE-TYBLE16");
	hidp.setLocalName("AE-TYBLE16 HID");
	hidp.begin();
	digitalWrite(LED,LOW);delay(1000);
}

void loop() {
	BLECentral c = hidp.central();
	digitalWrite(LED,c&&c.connected()?HIGH:LOW);
}

