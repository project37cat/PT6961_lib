// управление LED дисплеем с контроллером PT6961 (7 знаков по 7 сегментов, от китайского DVD плеера)
// "led.h" версия 1 от 22.10.2014 
// компилятор HI-TECH C PRO PIC18 V9.63PL3


#define set_bit(ADDRESS,BIT) (ADDRESS |= (1<<BIT))
#define clear_bit(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))


// программный SPI
// работает на любых трех ногах
#define DAT RD2 //DIN
#define CLK RD1
#define STB RD0


char ledbuff[13]; //видеобуфер

const unsigned char ce=0b1000000; //"прочерк"
const unsigned char cn=0b0000000; //"пробел"
const unsigned char c0=0b0111111; //"0"
const unsigned char c1=0b0000110; //"1"
const unsigned char c2=0b1110011; //"2"
const unsigned char c3=0b1110110; //"3"
const unsigned char c4=0b1001110; //"4"
const unsigned char c5=0b1111100; //"5"
const unsigned char c6=0b1111101; //"6"
const unsigned char c7=0b0100110; //"7"
const unsigned char c8=0b1111111; //"8"
const unsigned char c9=0b1111110; //"9"


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_data(unsigned char data) //последовательная передача байта в порт начиная с нулевого бита
{
for(unsigned char i=0; i<8; i++)
	{
	if(data & 0x01) DAT=1;
	else DAT=0;
	data>>=0x01;
	CLK=0; //high-low-high
	CLK=1;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
led_dimm(unsigned char vol) //vol 0..7 - яркость
{
STB=0;
led_data(0b10001000+vol);
STB=1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_init(void) //инициализация PT6961
{
DAT=0;
CLK=1;
STB=1;

__delay_ms(30); //в даташите рекомендуют 200мс

STB=0;
led_data(0b01000000); //command 2
STB=1;

STB=0;
led_data(0b11000000); //command 3
for(unsigned char i=0; i<13; i++) led_data(0); //очистка дисплея
STB=1;

STB=0;
led_data(0b00000011); //command 1
STB=1;

STB=0;
led_data(0b10001000); //command 4  //биты 0..2 регулировка яркости (000..111)
STB=1;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
led_digit(unsigned char digit, unsigned char sign) // в позиции 1..7 нарисовать символ
{
unsigned char tmp=0;
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

for(unsigned char i=0; i<14; i=i+2) //передача кода в буфер
	{
	if(tmp & 0x01) set_bit(ledbuff[i],digit);
	else clear_bit(ledbuff[i],digit);
	tmp>>=0x01;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
led_print(unsigned char digit, const char *string) //вывод строки на дисплей //digit - знакоместо 1..7
{
for(unsigned char i=0; string[i]!='\0'; i++)
led_digit(i+digit,string[i]);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void led_update(void) //обновление картинки - отправляем весь буфер в порт
{
STB=0;
led_data(0b11000000); //command 3
for(unsigned char i=0; i<13; i++) led_data(ledbuff[i]);
STB=1;
}
