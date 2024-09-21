#include <Arduino.h>
#include <SPI.h>
#include <BLEHIDPeripheral.h>
#include <BLEKeyboard.h>
#include <BLEMouse.h>
#define FOR(I,X) for(uint8_t I=0;I<X;I++)
#define LED 17

const uint8_t
	pu_pin[]={3,23,25,19},
	bus_pin[]={6,21,2},
	st_pin[]={4,5};

uint16_t btn=0,btn_p=btn,st_o[2];
uint32_t loopt=0;
int16_t st[3];


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
BLEKeyboard kbd;
BLEMouse mouse;

uint16_t get_btn(){
	uint16_t x=0;
	for(uint8_t i=0,p=0;i<3;i++){
		digitalWrite(bus_pin[i],LOW);
		for(uint8_t j=0;j<4;j++,p++)x|=(!digitalRead(pu_pin[j]))<<p;
		digitalWrite(bus_pin[i],HIGH);
	}	
	return x;
}

void setup(){
	xtal_init();
	pinMode(LED,OUTPUT);
	FOR(i,4)pinMode(pu_pin[i],INPUT_PULLUP);
	FOR(i,3){pinMode(bus_pin[i],OUTPUT);digitalWrite(bus_pin[i],HIGH);}
	FOR(i,2){pinMode(st_pin[i],INPUT);st_o[i]=analogRead(st_pin[i]);}
	
	if(get_btn()==0x0c0)hidp.clearBondStoreData();

	hidp.setReportIdOffset(1);
	hidp.setDeviceName("nRF51-CPAD");
	hidp.setLocalName("nRF51-CPAD");
	hidp.addHID(kbd);
	hidp.addHID(mouse);
	hidp.begin();
}

void loop(){
	while(millis()<(loopt+5))delay(1);
	loopt=millis();
	btn=get_btn();
	FOR(p,12){
		if((btn_p^btn)&(1<<p)){
			uint8_t x=(btn>>p)&1;
			switch(p){
				//    6  7
				//  b 4  5 3 
				// a 8    0 2
				//  9      1 
				case 2:{if(x)kbd.press(KEYCODE_SPACE);else kbd.release(KEYCODE_SPACE);break;}// A
				case 1:{if(x)kbd.press(KEYCODE_RIGHT_SHIFT,KEYCODE_MOD_RIGHT_SHIFT);else kbd.release(KEYCODE_RIGHT_SHIFT,KEYCODE_MOD_RIGHT_SHIFT);break;}// B
				case 3:{if(x)mouse.press(MOUSEBTN_RIGHT_MASK);else mouse.release(MOUSEBTN_RIGHT_MASK);break;}// X
				case 0:{if(x)mouse.press(MOUSEBTN_LEFT_MASK);else mouse.release(MOUSEBTN_LEFT_MASK);break;}// Y

				case 4:{if(x)kbd.press(KEYCODE_ESC);else kbd.release(KEYCODE_ESC);break;}// -
				case 5:{break;}// +
				case 6:{if(x)mouse.move(0,0,1);break;}// L
				case 7:{if(x)mouse.move(0,0,-1);break;}// R

				case 11:{if(x)kbd.press(KEYCODE_W);else kbd.release(KEYCODE_W);break;}// W
				case 10:{if(x)kbd.press(KEYCODE_A);else kbd.release(KEYCODE_A);break;}// A
				case 9: {if(x)kbd.press(KEYCODE_S);else kbd.release(KEYCODE_S);break;}// S
				case 8: {if(x)kbd.press(KEYCODE_D);else kbd.release(KEYCODE_D);break;}// D
			}
		}
	}

	FOR(i,2)st[i]=analogRead(st_pin[i])-st_o[i];

	int16_t tmp;
	tmp=st[0]-st[1];
	st[1]=st[0]+st[1];
	st[0]=tmp;

	FOR(i,2){st[i]>>=6;st[i]=(st[i]<0?-1:1)*max(0,abs(st[i])-2);}
	st[2]=0;//((btn>>6)&1?1:0)+((btn>>7)&1?-1:0);

	if(st[0]||st[1]||st[2])mouse.move(st[0],st[1],st[2]);

	BLECentral c=hidp.central();
	digitalWrite(LED,c&&c.connected()?LOW:HIGH);
	btn_p=btn;
}

