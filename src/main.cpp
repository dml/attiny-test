#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define RIGHT_LED_PIN 0
#define LEFT_LED_PIN  1

typedef struct {
  int pwm_level = 0;
  int increment_by = 1;
  int skipping = 0;
} Led;

volatile Led left_led, right_led;

void setupTimer1() {
  noInterrupts();

  TCNT1 = 0;
  TCCR1 = 0;

  // 100.16025641025641 Hz (1000000/((155+1)*64))
  OCR1C = 30;
  OCR1A = OCR1C;
  TCCR1 |= (1 << CTC1);
  TCCR1 |= (1 << CS12) | (1 << CS11) | (1 << CS10);
  TIMSK |= (1 << OCIE1A);

  interrupts();
}

void splash(int pin, volatile Led &led) {
  if (led.skipping > 0) {
    led.skipping--;
    return;
  }

  led.pwm_level += led.increment_by;

  analogWrite(pin, led.pwm_level);

  if(led.pwm_level > 254) { led.increment_by = -1; }
  if(led.pwm_level <   1) {
    led.increment_by = 1;
    led.skipping = 255;
  }
}

void setup() {
  pinMode(RIGHT_LED_PIN, OUTPUT);
  pinMode(LEFT_LED_PIN,  OUTPUT);

  left_led.skipping = 255 + 127;

  setupTimer1();
}

void loop() {
}

ISR(TIMER1_COMPA_vect) {
  splash(LEFT_LED_PIN, left_led);
  splash(RIGHT_LED_PIN, right_led);
}
