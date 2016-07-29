# chip8

## Input

    1	2	3	4

    q	w	e	r

    a	s	d	f

    z	x	c	v

## Usage

#### Linux

Install SDL2 development library

Compile program with `make`

Run with `./chip8 ROM`

#### Mac

Install SDL2 development library (using 'brew' is recommended)

Compile with `make mac`

Run with `./chip8 ROM`

#### Windows

Install SDL2 development library

Requires mingw32

Compile with `make windows`

Run with `./chip8.exe ROM`

## Sample screenshots

![alt text](http://i.imgur.com/1v2JWOX.png "Pong")

![alt text](http://i.imgur.com/u6zsZgR.png "Brix")

![alt text](http://i.imgur.com/QSrQBtr.png "Space Invaders")

![alt text](http://i.imgur.com/brpEzyA.png "UFO")

## Notes

By default FX55 and FX65 will increment the I register. This can be disabled for compatibility purposes by using the -i flag
