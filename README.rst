.. _blinky-sample:

Sinusoidal Motor Control
######

Overview
********

Code to spin a brushless DC electric motor using sinusoidal PWM (SPWM).

Requirements
************

Works only for STM32 Nucleo-F446RE.

Building and Running
********************

For a STM32 Nucleo-F446RE, run the following from the parent directory:

Build:
``west build -b nucleo_f446re -p auto sinusoid``

Run:
``west flash``

To see the output, run:
``minicom -b 115200 -D /dev/ttyACM0``