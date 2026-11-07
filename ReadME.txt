For hver motor er det 4 kabler +  2 strømkabler + og -. Har bare brukt + og - kablane så langt, ingen av dei andre 4.

PE9 - TIM_CH1(PWM) - Kobla til L293D pin 1   - Formål: hastigheten til venstre motor
PF13 - GPIO output - Kobla til L293D pin 2   - Formål: venstre motor retning input 1
PF12 - GPIO output - Kobla til L293D pin 7   - Formål: venstre motor retning input 2
PE11 - TIM_CH2(PWM) - Kobla til L293D pin 9  - Formål: Høyre motor hastighet
PF14 - GPIO output - Kobla til L293D pin 10  - Formål: Høyre motor retning input 1
PF15 - GPIO output - Kobla til L293D pin 15  - Formål: Høyre motor retning input 2

Joystick pins:
PA3 - ADC3_IN3 (rank2) - Kobla til Joystick VRx  - Formål: X axis
PC0 - ADC3_IN10(rank1) - Kobla til Joystick VRy  - Formål: Y axis
PB9 - GPIO input, pull up - Kobla til Joystick SW - Formål: Button(LOW når den blir trykka ner)

Serial:
PD8 - USART3_TX - Formål: USB serial til pc - 115200 baud
PD9 - USART3_RX - Formål: USB serial til pc

Andre pins som er brukt, men som ein mest sannsynlig ikkje trenger i koden, men lista anyway for oversikt:
Board 5v - Kobla til L293D pin 16 og pin 8
Board 3.3v -  Kobla til joystick +5v pin
Board GND - Kobla til breadboard rail, brukt av L293D pin 4, 5, 12, 13 og joystick GND.

L293D pin 3 og 6 - Kobla til venstre motor + og -
L293D pin 11 og 14 - Kobla til høyre motor + og -
