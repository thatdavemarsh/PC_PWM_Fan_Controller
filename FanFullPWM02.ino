//User Timer1 to mesure input PWM on PD2(INT0) and Timer2 in fast PWM to output PWM on PD3(OC2B)
//works mostly but 100% output needs managing and mapping is not precise (suspect PWM freq mapping)
//Ads 10 segment LED driver

const int PWM_In = PD2;  //PD2, (INT0)
const int PWM_Out = PD3; //PD3, (OC2B/INT1)

const byte BsegMask = B00001111;  //using first four bits of Port B - these are the most sig bits
const byte CsegMask = B00111111;  //using first six bits of Port C - these are the least sig bits

int displayValue = 0; //integer to store mapped display from 0 to 10
byte bSeg = B0; //byte to store LEDs to light up for PORTB
byte cSeg = B0; //byte to store LEDs to light up for PORTC

//global variables
volatile unsigned long tFreq = 0;
volatile unsigned long tPeriod_In   = 0;
volatile unsigned long tLast = 0;

unsigned long tDuty_In;
byte period_PWM = 80;	//Spec is 25kHz PWM cycle
byte duty_PWM = 25;		//Initialize at 25%


void setup() {
	//Setup PWM In on PWM_In Pin - Input Configuration with Pull-Up Pin (later check not inverted...)
	DDRD &= ~( 1 << PWM_In );	// input - clear bit to 0 
	PORTD |= ( 1 << PWM_In ); // pull up - set bit to 1
	
	DDRB |= BsegMask; //set the LED bits to output in the direction register (OR with inverted mask)
	DDRC |= CsegMask;
	
	
	
	//Setup Interrupts for PWM_In
	//Conditions - Rising 11, Falling 10 - start with Rising
	//External Interrupt Control RegisterA
	EICRA |=  ( 1 << ISC01 );  // set 1
	EICRA |=  ( 1 << ISC00 );  // set 1
	
	//Enable Interrupt
	EIMSK |= ( 1 << INT0 );	// turn on INT0 attached to PD2 = Pin 2
	
	//Initialize PWM_In Tracking Timer TCNT1
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	//Start the timer
	TCCR1B ^= (1 << CS10);
	
	//===========================================================
	
	//Setup PWM Out on PWM_Out Pin
	//Setup Timer2 Registers
	//TCCR2A clear all bits
	TCCR2A = 0;
	//configure to compare with register B COM0B1 = 1, COM0B0 = 0
	TCCR2A ^= ( 1 << COM0B1 );
	//WGM - Mode 7
	TCCR2A ^= ( 1 << WGM01 );
	TCCR2A ^= ( 1 << WGM00 );
	
	//TCCR2B
	TCCR2B = 0;
	//finish WGM mode 7
	TCCR2B ^= ( 1 << WGM02 );
	
	//load in PWM setting
	TCNT2 = 0;
	OCR2A = period_PWM;
	OCR2B = duty_PWM;
	//set PD3 (OC2B) to output (pin3)
	DDRD ^= ( 1 << PWM_Out );
		
	//Start the timer with prescaler = 8
	TCCR2B ^= (1 << CS01);
	
	//Enable Global Interrupts
	sei();	
	
	//Serial.begin(9600);
	}	
	
void loop() {
	//duty cycle mapping
	//calculate percent duty cycle in
	//check tFreq range and trap if outside range and set to default
	if (tFreq < 571 ) {tFreq = 640;}
	if (tFreq > 761 ) {tFreq = 640;}
	tDuty_In = byte (100 * tPeriod_In / tFreq );	
	
	//trap bounds
	if (tDuty_In < 36) {tDuty_In = 35 ;}
	if (tDuty_In >90) {tDuty_In = 90 ;}

	//map it
	duty_PWM = map (tDuty_In , 35, 90, 20, 100);
	OCR2B = ( duty_PWM * period_PWM / 100);	
	
	//display driver
	bSeg = 0; //reset display values to zero
	cSeg = 0;
	displayValue = map (tDuty_In , 35, 90, 1, 11); //map input to 10 segment display
	//Serial.print(displayValue); Serial.print(" ");
	while (displayValue > 0 ) {
		if (displayValue > 6) {
			bSeg = bSeg << 1; //shift the lights over one,
			bSeg++; //turn on the light
			//Serial.print(" Bit Shift Left -B ");
			displayValue--;
			}
			else {
			cSeg = cSeg << 1;
			cSeg++;
			//Serial.print(" Bit Shift Left -C ");
			displayValue--;
			}
		}
	//Serial.print(cSeg, BIN);
	//Serial.println(bSeg, BIN);
	PORTB &= ~BsegMask; // mask out and clear bits for leds
	PORTC &= ~CsegMask;
	PORTB |= bSeg;  // set bits in the port
	PORTC |= cSeg;
	}
		
ISR (INT0_vect) {
	//Rising
	//Evaluate with Mask
	if (( ( EICRA & (1 << ISC00) ) >> ISC00) == 1) {
		//Rising
		tFreq = TCNT1 + 1; //small error introduced here to avoid div/0 condition
		TCNT1 = 0;
		//toggle to Falling by XOR last bit
		EICRA ^= (1 << ISC00);
		}
		else {
		//Falling
		tPeriod_In = TCNT1;
		//toggle to Rising by XOR last bit
		EICRA ^= (1 << ISC00);
	}
}
	