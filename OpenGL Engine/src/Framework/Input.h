#pragma once

class Input {
public:
	Input();
	static bool keys[2048];
	static void KeyCallback( struct GLFWwindow* window, int key, int scancode, int action, int mods );
};