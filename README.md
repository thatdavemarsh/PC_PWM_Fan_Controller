# PC_PWM_Fan_Controller
PC Fan/Pump Curve Translation Project

This is a simple project that came about from the need (or want) to drive a water cooling pump from a PC board that did not have fan curves that could be modified.  Low temperature fan PWM outputs were not enough to get the pump going.

This Arduino AT328 (Nano) project uses the timer and interupt functions of the chip to measure the incomming PWM signal and generate a new output PWM.

The tach signal is passed straight back.  Future modifications could be used to trick the host board.

The arduino and PWM signals from the PC board run on 5V.  A simple DC/DC buck converter from Amazon was used to step the 12V fan power voltage down to 5v to power the project.

This was a good exercise to better understand the timers and interrupts of the Arduino project.

The project was breadboarded and I also took my first crack at an ordered PCB.

Current status is in PCB assembly.
