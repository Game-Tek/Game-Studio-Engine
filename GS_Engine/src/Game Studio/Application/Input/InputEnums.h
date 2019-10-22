#pragma once

#include "Core.h"

enum class KeyState : uint8
{
	PRESSED, RELEASED, NONE
};

#define MAX_KEYBOARD_KEYS 71

enum class KeyboardKeys : uint8
{
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	Z,
	X,
	C,
	V,
	B,
	N,
	M,

	Keyboard0,
	Keyboard1,
	Keyboard2,
	Keyboard3,
	Keyboard4,
	Keyboard5,
	Keyboard6,
	Keyboard7,
	Keyboard8,
	Keyboard9,


	Enter,

	Tab,

	Esc,

	RShift,
	LShift,

	RControl,
	LControl,

	Alt,
	AltGr,

	UpArrow,
	RightArrow,
	DownArrow,
	LeftArrow,

	SpaceBar,
	
	Numpad0,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,

	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum JoystickButtons
{
	RightMenuButton,
	LeftMenuButton,

	TopFaceButton,
	RightFaceButton,
	BottomFaceButton,
	LeftFaceButton,

	TopDPadButton,
	RightDPadButton,
	BottomDPadButton,
	LeftDPadButton,

	RightBackButton,
	LeftBackButton,
};