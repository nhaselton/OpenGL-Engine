#include "Input.h"

bool Input::keys[2048];


Input::Input() {

}
void Input::KeyCallback( struct GLFWwindow* window, int key, int scancode, int action, int mods ) {

	keys[key] = action;
}
