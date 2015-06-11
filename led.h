// PT6961 library v2  toxcat 2015 copyleft


#define _XTAL_FREQ 8000000 //for delay functions


/* software SPI */

#define DAT      RB3
#define DAT_PIN  TRISB3

#define CLK      RB2
#define CLK_PIN  TRISB2

#define STB      RB1
#define STB_PIN  TRISB1


#define SET_BIT(reg, bit) (reg |= (1<<bit))
#define CLR_BIT(reg, bit) (reg &= (~(1<<bit)))


typedef unsigned char uint8_t;


char ledbuff[14];

const uint8_t ce=0b1000000; //"-"
const uint8_t cn=0b0000000; //space
const uint8_t c0=0b0111111; //"0"
const uint8_t c1=0b0000110; //"1"
const uint8_t c2=0b1110011; //"2"
const uint8_t c3=0b1110110; //"3"
const uint8_t c4=0b1001110; //"4"
const uint8_t c5=0b1111100; //"5"
const uint8_t c6=0b1111101; //"6"
const uint8_t c7=0b0100110; //"7"
const uint8_t c8=0b1111111; //"8"
const uint8_t c9=0b1111110; //"9"


///////////////////////////////////////////////////////////////////////////////////////////////////
void delay_ms(uint8_t val)
{
while(val--) __delay_ms(1);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_data(uint8_t data) //последовательная передача байта в порт начиная с нулевого бита
{
for(uint8_t i=0; i<8; i++)
	{
	if(data & 0x01) DAT=1;
	else DAT=0;
	data>>=0x01;
	CLK=0; //high-low-high
	CLK=1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_init(void) //инициализация PT6961
{
DAT_PIN=0;
CLK_PIN=0;
STB_PIN=0;

DAT=0;
CLK=1;
STB=1;

delay_ms(200);

STB=0;
led_data(0b01000000); //command 2
STB=1;

STB=0;
led_data(0b11000000); //command 3
for(uint8_t i=0; i<14; i++) led_data(0); //очистка дисплея
STB=1;

STB=0;
led_data(0b00000011); //command 1  //биты 0..1 режим (11 - 7 digits, 11 segments)
STB=1;

STB=0;
led_data(0b10000011); //command 4  //бит 3 - display OFF  //биты 0..2 яркость
STB=1;

STB=0;
led_data(0b00000011); //command 1
STB=1;

STB=0;
led_data(0b10001011); //command 4  //display ON
STB=1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
led_digit(uint8_t pos, uint8_t sign) // в позиции 1..7 нарисовать символ
{
uint8_t tmp=0;

switch(sign) //по полученному коду символа выбираем код сегментов
	{
	case 32: tmp=cn; break; //пробел
	case 48: tmp=c0; break;
	case 49: tmp=c1; break;
	case 50: tmp=c2; break;
	case 51: tmp=c3; break;
	case 52: tmp=c4; break;
	case 53: tmp=c5; break;
	case 54: tmp=c6; break;
	case 55: tmp=c7; break;
	case 56: tmp=c8; break;
	case 57: tmp=c9; break;
	default: tmp=ce; break; //прочерк если неподдерживаемый символ
	}

for(uint8_t i=0; i<14; i=i+2) //передача кода в буфер
	{
	if(tmp & 0x01) SET_BIT(ledbuff[i],pos);
	else CLR_BIT(ledbuff[i],pos);
	tmp>>=0x01;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
led_print(uint8_t pos, const char *str) //вывод строки на дисплей //pos - позиция 1..7
{
for(uint8_t i=0; str[i]; i++) led_digit(i+pos, str[i]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_update(void) //обновление картинки - отправляем весь буфер в порт
{
STB=0;
led_data(0b11000000); //command 3
for(uint8_t i=0; i<14; i++) led_data(ledbuff[i]);
STB=1;
}
