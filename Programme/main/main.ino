//Bibliothèques à implémenter
#include <arduino.h>
#include <Wire.h> //I2C
#include <Adafruit_PWMServoDriver.h> //PCA9685PW
#include <Adafruit_MCP23X17.h> //MCP23017
#include <U8g2lib.h>  // OLED
#include <DHT11.h> //Capteur DHT

//Attribution des pins de l'ESP
            //----- Input -----
#define SW_AU  25         //Switch Arrêt d'urgence
#define Cap_DHT11  5          //Data température + humidité
#define Temp_P1  34       //Capteur de température Analogique
#define Temp_P2  35       //Capteur de température Analogique
#define Temp_boitier  36  //Capteur de température Analogique
#define SW_Aspi  0        //Switch de l'aspirateur GPIO déporté MCP
#define SW_Vent  1        //Switch du ventilateur GPIO déporté MCP
#define SW_P1  2          //Switch de la pompe 1 GPIO déporté MCP
#define SW_P2  3          //Switch de la pompe 2 GPIO déporté MCP
#define SW_L  4           //Switch de la lumière GPIO déporté MCP
#define Capt_feu  5       //Capteur de flamme numérique GPIO déporté MCP
            //----- Output -----
#define Relai_AU  33      //Relai qui sectionne l'arrivée de courant de la CNC
#define Trans_vent1  4    //transistor ventilation boitier IHM
#define Buzzer  13        //indicateur sonore
#define Relai_vent2  14   //Ventilation CNC
#define D1_B  0           //LED RGB GPIO déporté PCA
#define D1_G  1           //LED RGB GPIO déporté PCA
#define D1_R  2           //LED RGB GPIO déporté PCA
#define D2_B  3           //LED RGB GPIO déporté PCA
#define D2_G  4           //LED RGB GPIO déporté PCA
#define D2_R  5           //LED RGB GPIO déporté PCA
#define D3_B  6           //LED RGB GPIO déporté PCA
#define D3_G  7           //LED RGB GPIO déporté PCA
#define D3_R  8           //LED RGB GPIO déporté PCA
#define D4_B  9           //LED RGB GPIO déporté PCA
#define D4_G  10          //LED RGB GPIO déporté PCA
#define D4_R  11          //LED RGB GPIO déporté PCA
#define D5_B  12          //LED GPIO déporté PCA
#define D6_B  13          //LED GPIO déporté PCA
#define Relai_Supp  8     //Relai supplémentaire GPIO déporté MCP
#define Relai_ElecV  9    //Relai électrovanne pour arrivé d'air GPIO déporté MCP
#define Relai_Aspi  10    //Relai de l'aspirateur GPIO déporté MCP
#define Relai_lumiere  11 //Relai de la lumière GPIO déporté MCP
#define Relai_Pompe1  12  //Relai de la lumière GPIO déporté MCP
#define Relai_Pompe2  13  //Relai de la lumière GPIO déporté MCP
            //----- OLED ----- 
#define OLED_CS  32       //broche CS de l'OLED
#define OLED_RST  19      //broche Reset de l'OLED
#define OLED_SDIN  23     //broche SDIN de l'OLED
#define OLED_SCLK  18     //broche SCLK de l'OLED
#define OLED_DC  12       //broche DC de l'OLED
U8G2_SSD1322_NHD_128X64_F_3W_SW_SPI u8g2(U8G2_R0,18,23,10,19);//clk,data,cs,reset
            //----- I2C -----
Adafruit_MCP23X17 MCP;
Adafruit_PWMServoDriver PCA;
            //----- DHT -----
DHT11 dht11(Cap_DHT11);
            //----- Variable -----
bool state_AU = 0;
#define blink_interval  500 //intervalle de clignotage pour la fonction millis()
unsigned long previousMillis_led = 0; ////temporalité pour la fonction millis()
unsigned long previousMillis_buzzer = 0;
unsigned long previousMillis_oled = 0;
            //----- Logo -----
const unsigned char bitmap_ROMA_logo [] PROGMEM = {// 'ROMA_logo', 64x64px
	0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x1f, 0xf8, 0x07, 0x1e, 0x0e, 0xf0, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x1f, 0xfc, 0x0f, 0x1e, 0x1f, 0xf0, 0x01, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x3f, 0xfe, 0x1f, 0x1f, 0x1f, 0xf8, 0x01, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x38, 0x0f, 0x1e, 0xbf, 0x1f, 0xf8, 0x01, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x38, 0x0f, 0x1c, 0xbf, 0x1f, 0xfc, 0x03, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x3c, 0x0f, 0x3c, 0xbf, 0x1f, 0xbc, 0x03, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x1f, 0x0f, 0x3c, 0xf7, 0x1d, 0x9c, 0x07, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x0f, 0x0f, 0x1e, 0xf7, 0x3d, 0xfe, 0x07, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x0f, 0x1e, 0x9f, 0xf7, 0x3d, 0xfe, 0x07, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x1e, 0xfe, 0x8f, 0xe7, 0x38, 0xfe, 0x0f, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x1c, 0xfc, 0x87, 0xe3, 0x38, 0x0f, 0x0f, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x3c, 0xf0, 0x83, 0x43, 0x38, 0x07, 0x0e, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x30, 0x00, 0x80, 0x00, 0x00, 0x18, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x00, 0x80, 0x00, 0x00, 0x38, 0x02, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x80, 0x18, 0x00, 0xc0, 0x00, 0x00, 0x30, 0x02, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xc0, 0x0d, 0x00, 0xc0, 0x00, 0x00, 0x20, 0x03, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x80, 0x05, 0x00, 0x00, 0x00, 0x00, 0x40, 0x03, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x80, 0x11, 0x00, 0xe0, 0x07, 0x00, 0x10, 0x03, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x19, 0x00, 0xf8, 0x1f, 0x00, 0x30, 0x11, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x30, 0x0c, 0x00, 0xfc, 0x3f, 0x00, 0x70, 0x18, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x70, 0x0c, 0x10, 0xfc, 0x7f, 0x0f, 0x60, 0x1c, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x60, 0x06, 0xe0, 0xfe, 0x7f, 0x01, 0x48, 0x0e, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xc0, 0x12, 0xc0, 0xfe, 0x7f, 0x00, 0x98, 0x06, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x18, 0x80, 0xfe, 0xff, 0x00, 0x38, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x30, 0x18, 0x00, 0xfe, 0xff, 0x00, 0x30, 0x18, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x48, 0x00, 0xfe, 0xff, 0x00, 0x34, 0x0e, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xc0, 0x6d, 0x00, 0xfe, 0x7f, 0x00, 0x24, 0x07, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x60, 0x00, 0xfe, 0x7f, 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x60, 0x00, 0xfc, 0x3f, 0x80, 0x0c, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xe0, 0x21, 0x03, 0xf8, 0x3f, 0x80, 0x0d, 0x0f, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0xc0, 0x27, 0x03, 0xf0, 0x4f, 0x80, 0xc9, 0x03, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x03, 0xc8, 0x67, 0x80, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x1b, 0x1c, 0xc0, 0x90, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x80, 0x3f, 0x19, 0x06, 0x80, 0xb0, 0xfd, 0x01, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x0c, 0x18, 0x02, 0x80, 0x30, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x18, 0x00, 0x00, 0x10, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0xf8, 0x13, 0x00, 0x00, 0x90, 0x3f, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0xd0, 0x1b, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x1f, 0xd0, 0x13, 0xf0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0xc0, 0x47, 0x50, 0x1b, 0xc6, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00
};
// '107818', 34x64px
const unsigned char bitmap_temp_logo [] PROGMEM = {
	0xff, 0xfc, 0x0f, 0xff, 0xc0, 0xff, 0xf0, 0x03, 0xff, 0xc0, 0xff, 0xe0, 0x01, 0xff, 0xc0, 0xff, 
	0xc0, 0x00, 0xff, 0xc0, 0xff, 0x83, 0xe0, 0x7f, 0xc0, 0xff, 0x87, 0xf8, 0x7f, 0xc0, 0xff, 0x0f, 
	0xf8, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 
	0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 
	0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 0xff, 0x0f, 0xfc, 0x3f, 0xc0, 
	0xff, 0x0f, 0x3c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 
	0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 
	0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 
	0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 
	0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 
	0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 
	0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xff, 0x0e, 0x1c, 0x3f, 0xc0, 0xfe, 0x0e, 
	0x1c, 0x1f, 0xc0, 0xfc, 0x1e, 0x1c, 0x0f, 0xc0, 0xf8, 0x3e, 0x1e, 0x07, 0xc0, 0xf0, 0x7e, 0x1f, 
	0x07, 0xc0, 0xf0, 0xfe, 0x1f, 0x83, 0xc0, 0xe0, 0xfc, 0x1f, 0xc3, 0xc0, 0xe1, 0xf8, 0x07, 0xe1, 
	0xc0, 0xe1, 0xf0, 0x03, 0xe1, 0xc0, 0xc3, 0xe0, 0x01, 0xe1, 0xc0, 0xc3, 0xe0, 0x01, 0xf1, 0xc0, 
	0xc3, 0xc0, 0x01, 0xf0, 0xc0, 0xc3, 0xc0, 0x01, 0xf0, 0xc0, 0xc3, 0xe0, 0x01, 0xf0, 0xc0, 0xc3, 
	0xe0, 0x01, 0xf1, 0xc0, 0xc3, 0xe0, 0x03, 0xe1, 0xc0, 0xe1, 0xf0, 0x03, 0xe1, 0xc0, 0xe1, 0xf8, 
	0x0f, 0xe1, 0xc0, 0xe0, 0xfe, 0x3f, 0xc3, 0xc0, 0xf0, 0x7f, 0xff, 0x83, 0xc0, 0xf8, 0x3f, 0xff, 
	0x07, 0xc0, 0xf8, 0x1f, 0xfe, 0x0f, 0xc0, 0xfc, 0x07, 0xf8, 0x1f, 0xc0, 0xfe, 0x00, 0x00, 0x3f, 
	0xc0, 0xff, 0x80, 0x00, 0x7f, 0xc0, 0xff, 0xc0, 0x01, 0xff, 0xc0, 0xff, 0xf8, 0x0f, 0xff, 0xc0
};

void Setup_pin(){
  pinMode(Temp_boitier, INPUT);
  pinMode(Temp_P1, INPUT);
  pinMode(Temp_P2, INPUT);
  pinMode(SW_AU, INPUT);
  pinMode(Cap_DHT11, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Relai_AU, OUTPUT);
  pinMode(Trans_vent1, OUTPUT);
  pinMode(Relai_vent2, OUTPUT);
}
void OLED_setup(){
  digitalWrite(OLED_DC,LOW);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.drawXBMP(0, 0, 128, 64, bitmap_ROMA_logo);
  u8g2.sendBuffer();  // Envoyer les données à l'écran
  delay(2000);
}
void I2C_setup(){
  Wire.begin();
  MCP.begin_I2C(0x20);
  PCA.begin(0x40);
}
/*--------------------Fonction LED_RGB--------------------
Input : Température, Seuil de déclenchement alarme,Led R/G/B
Output : Overflow de la température
Description : fonction regroupant 2 blocs :
Défini la couleur de la led en fonction de la température
Fait clignoter la led si la température dépasse le seuil voulu
--------------------------------------------------------*/
bool LED_RGB(float temp, float tempMAX, int L_R, int L_G, int L_B) {
  //self explain
  bool state_LED = 1;
  float PWM_B, PWM_R, PWM_G =0;
  bool overflow =0;
  if (temp >= tempMAX) {
    if(millis() - previousMillis_led >= blink_interval) {//fonction millis()
      previousMillis_led = millis();
      state_LED = !state_LED;
      overflow =1;
    }
  }
  if (temp<=35) {
    PWM_R = 0;
    PWM_B = max(0.0, min(100.0, -0.0816 * temp * temp + 100));
    PWM_G = max(0.0, min(100.0, -0.0816 * (temp - 35) * (temp - 35) + 100));
  }else if (temp>35) {
    PWM_B = 0;
    PWM_R = max(0.0, min(100.0, 0.0816 * (temp - 35) * (temp  - 35)));
    PWM_G = max(0.0, min(100.0, -0.0816 * (temp - 35) * (temp - 35) + 100));
  }

  if(state_LED) { 
    PCA.setPWM(L_R,0,map(PWM_R, 0, 100, 0, 4095)); // allumer les leds en fonction de leurs niveau de température à leur PWM correspondant
    PCA.setPWM(L_G,0,map(PWM_G, 0, 100, 0, 4095));
    PCA.setPWM(L_B,0,map(PWM_B, 0, 100, 0, 4095));
  }else { // Si en phase OFF du clignotement
    PCA.setPWM(L_R,0,0); // Éteindre tous les canaux PWM
    PCA.setPWM(L_G,0,0);
    PCA.setPWM(L_B,0,0);
  }
  return overflow;
}
/*------------------Fonction Lire_temp-------------------
Input : tension sur le DAC
Output : température
Description : conversion d'une donnée analogique de température en numérique
--------------------------------------------------------*/
float Lire_temp(int Pin_capt){
  float tension = (analogRead(Pin_capt)/4095) * 3.3;
  float temp = (tension - 0.5) * 100; 
  return temp;
}
/*------------------Fonction FCT_buzzer-------------------
Input : état du buzzer, fréquence du signal
Output :/
Description : fonction qui fait biper le buzzer à une fréquence demandé
--------------------------------------------------------*/
void FCT_buzzer(bool state_buzzer,int blink_interval_buzzer){
  bool state_bu =0;
  if (state_buzzer==1) { // Si le buzzer doit biper
    if(millis() - previousMillis_buzzer >= blink_interval_buzzer) {//fonction millis()
      previousMillis_buzzer = millis();
      state_bu = !state_bu;
      digitalWrite(Buzzer, state_bu);
    }
  }else{
     digitalWrite(Buzzer, 0);
  }
}
/*------------------Fonction IHM_OLED-------------------
Input :
Output :
Description :
--------------------------------------------------------*/
void IHM_OLED() {
  if (millis() - previousMillis_oled >= 5000) {
    previousMillis_oled = millis(); // Mettre à jour le temps

    float temp1 = Lire_temp(Temp_P1);
    float temp2 = Lire_temp(Temp_P2);
    float temp3 = Lire_temp(Temp_boitier);
    int t_dht, h_dht =0;
    dht11.readTemperatureHumidity(t_dht,h_dht);

    // Affichage OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    // Affichage capteur 1
    u8g2.setCursor(5, 15);
    u8g2.print("Temp Pompe Moteur: ");
    u8g2.print(temp1);
    u8g2.print(" °C");
    // Affichage capteur 2    
    u8g2.setCursor(5, 30);
    u8g2.print("Temp Liquide : ");
    u8g2.print(temp2);
    u8g2.print(" °C");
    // Affichage capteur 3
    u8g2.setCursor(5, 45);
    u8g2.print("Temp boitier: ");
    u8g2.print(temp3);
    u8g2.print(" °C");
    // Affichage capteur DHT11
    u8g2.setCursor(5, 60);
    u8g2.print("Temp CNC: ");
    u8g2.print(t_dht);
    u8g2.print(" °C & Humidité ");
    u8g2.print(h_dht);
    u8g2.print(" %");

    u8g2.sendBuffer();//MAJ des données
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println("\n Initialisation en cours");
  Setup_pin();
  I2C_setup();
  OLED_setup();
}

void loop() {
  while(analogRead(SW_AU) ==0 && analogRead(Capt_feu)==0){
    digitalWrite(Relai_AU,0);
    FCT_buzzer(0,500);

  }
  if(analogRead(SW_AU)){
    FCT_buzzer(1,500);
    digitalWrite(Relai_AU,1);
  }else if(analogRead(Capt_feu)){
    FCT_buzzer(1,200);
    digitalWrite(Relai_AU,1);
  }

}