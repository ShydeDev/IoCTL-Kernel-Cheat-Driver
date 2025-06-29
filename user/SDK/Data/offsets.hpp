#pragma once
#include <iostream>

namespace offsets {
	// LocalPlayer
	constexpr uintptr_t dwLocalPlayer = 0x18560D0;
	constexpr uintptr_t dwForceJump = 0x184EE00;

	constexpr uintptr_t entityList = 0x1A020A8;
	constexpr uintptr_t fFlags = 0x3EC;
}

namespace status {
	// Functions
	constexpr uintptr_t RELEASE = 16777472;
	constexpr uintptr_t PRESS = 65537;

	// Status
	constexpr uintptr_t INAIR = 65564;
	constexpr uintptr_t ONGROUND = 65665;
}