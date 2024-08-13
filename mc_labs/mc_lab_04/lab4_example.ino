
#include <Arduino.h>
#define F_CPU 32000000UL  // Clock frequency in Hz

#define PRESCALER 256     // Timer prescaler value

const int buzzerPin = 28;

uint8_t number[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

uint8_t bcd_time[6] = { number[0], number[0], number[0], number[0], number[0], number[0] };

typedef struct Time {
  uint8_t second = 0;
  uint8_t minute = 0;
  uint8_t hour = 0;
}Time_t;

Time_t timerOne = { 0, 0, 0 };  
Time_t timerTwo = { 0, 0, 0 };

uint8_t key_cursor = 1;
uint8_t fig_number = 0;

bool timer1_active = true; 
bool timer1_started = false;
bool timer2_started = false;

void bcd(uint8_t number, uint8_t position);
ISR(TIMER2_COMPA_vect) {
  if (timer1_started) {
    if (timerOne.second > 0 || timerOne.minute > 0 || timerOne.hour > 0) {
      if (timerOne.second > 0)
        timerOne.second--;
      else {
timerOne.second = 59;
        if (timerOne.minute > 0) {
          timerOne.minute--;
        } else {
          timerOne.minute = 59;
          if (timerOne.hour > 0)
            timerOne.hour--;
        }
      }
    } else {
      for (int i = 0; i < 5; i++) {
        digitalWrite(buzzerPin, HIGH);
        delay(10000);
        digitalWrite(buzzerPin, LOW);
        delay(100);
      }
      timer1_started = false;
    }
  }
  if (timer1_active) {
    bcd(timerOne.hour, 0);
    bcd(timerOne.minute, 2);
    bcd(timerOne.second, 4);
  }
}


ISR(TIMER0_COMPB_vect) {  // Timer 0 interrupt
  key_cursor <<= 1;
  key_cursor &= 0b00111111;
  if (key_cursor == 0)
    key_cursor = 1;


  fig_number++;
  if (fig_number == 6)
    fig_number = 0;


  PORTC = key_cursor;
  PORTB = ~bcd_time[fig_number];
}


void setup() {
  noInterrupts();  // Disable all interrupts
  DDRC = 0xFF;  // Port is output
  PORTC = key_cursor;
  DDRA = 0xFF;   // Port is output
  PORTA = 0x00;  // +5V (segments off)
  // Port B -- buttons
  DDRD = 0x00;  // Port is input
  PORTD = 0xFF;
  TCNT0 = 0;
  OCR0A = 25;
  TCCR0A = (1 << WGM01);               // CTC
  TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler 1024
  TIMSK0 |= (1 << OCIE0A);             // Output Compare Match A Interrupt Enable


  TCCR2A = 0x00;
  TCCR2B = (1 << WGM12) | (1 << CS12) | (1 << CS10); //CTC mode & Prescaler @ 1024
  TIMSK2 = (1 << OCIE2A); // дозвіл на переривання по співпадінню
  OCR2A = 0x3D08 * 2;// compare value = 1 sec (16MHz AVR)
  interrupts();  // Enable global interrupts
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}
void loop() {
  // Button 1 - set hours
  if ((PIND & (1 << 2)) == 0) {
    if (timer1_active) {
      if (++timerOne.hour == 24)
        timerOne.hour = 0;
      bcd(timerOne.hour, 0);
    } else {
      if (++timerTwo.hour == 24)
        timerTwo.hour = 0;
      bcd(timerTwo.hour, 0);
    }
  }
  // Button 2 - set minutes
  if ((PIND & (1 << 3)) == 0) {
    if (timer1_active) {
      if (++timerOne.minute == 60)
        timerOne.minute = 0;
      bcd(timerOne.minute, 2);
    } else {
      if (++timerTwo.minute == 60)
        timerTwo.minute = 0;
      bcd(timerTwo.minute, 2);
    }
    delay(100000);
  }
  // Button 3 - switch between timers
  if ((PIND & (1 << 4)) == 0) {
    timer1_active = !timer1_active;
  }
  // Button 4 - start/stop timer
  if ((PIND & (1 << 5)) == 0) {
    if (timer1_active && timer1_started) {
      timerOne.hour = 0;
      timerOne.minute = 0;
      timerOne.second = 0;
      bcd(timerOne.hour, 0);
      bcd(timerOne.minute, 2);
      bcd(timerOne.second, 4);
      timer1_started = false;
    } else if (timer1_active && !timer1_started) {
      timer1_started = true;
    } 
  }
}
void bcd(uint8_t fig_in, uint8_t position) {
  uint8_t bcdL = fig_in % 10;
  uint8_t bcdH = fig_in / 10;
  bcd_time[position] = number[bcdH];
  bcd_time[position + 1] = number[bcdL];
}
