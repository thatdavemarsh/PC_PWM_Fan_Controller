# PC_PWM_Fan_Controller
PC Fan/Pump Curve Translation Project

This is a simple project that came about from the need (or want) to drive a water cooling pump from a PC board that did not have fan curves that could be modified.  Low temperature fan PWM outputs were not enough to get the pump going.

This Arduino AT328 (Nano) project uses the timer and interupt functions of the chip to measure the incomming PWM signal and generate a new output PWM.

The tach signal is passed straight back.  Future modifications could be used to trick the host board.

The arduino and PWM signals from the PC board run on 5V.  A simple DC/DC buck converter from Amazon was used to step the 12V fan power voltage down to 5v to power the project.

This was a good exercise to better understand the timers and interrupts of the Arduino project.

The project was breadboarded and I also took my first crack at an ordered PCB.

Current status is in PCB assembly.

There is no fancy translation function for the output PWM, currentlyt uses a linear map function:

duty_PWM = map (tDuty_In , 35, 90, 20, 100);

Takes the precoditioned %duty cycle as measured and spans it from 20% to 100% output.  Any transform here can take the measured input range of 35-90 and output any valid PWM output from 0 to 100.  In testing, the board I had never really output less than 35% or more than 90%.  The code check for ranges outside of these and caps them prior to this line.
