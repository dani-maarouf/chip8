# chip8

## Input

    1	2	3	4

    q	w	e	r

    a	s	d	f

    z	x	c	v

## Usage

Requires SDL2

#### Linux

Install with `make`

Run with `./chip8 ROM`

#### Mac

Install with `make mac`

Run with `./chip8 ROM`

#### Windows

Cross-compile using 

## Sample screenshots

![alt text](http://i.imgur.com/dWVxIf3.png "Pong")

![alt text](http://i.imgur.com/t9B8qsA.png "Brix")

## Notes

Audio works intermittently in cases where sound timer is set to a small value.
This issue mostly occurs on Windows, occasionaly on Linux and apparently not at all on Mac.
This can be observed in BRIX and PONG. Maybe this is related to rapidly pausing and unpausing the SDL audio device.

By default FX55 and FX65 will increment the I register. This can be disabled for compatibility purposes by using the -i flag
