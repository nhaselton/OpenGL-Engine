#pragma once
struct GLFWwindow;

#define SECOND_WINDOW 1

class Window {
public:
	Window();
	void					Init();
	GLFWwindow*				GetHandle() const { return handle;};
	
	int						GetWidth()	{ return width; }
	int						GetHeight()	 { return height; }

#if SECOND_WINDOW
	GLFWwindow* window2;
#endif

private:
	struct GLFWwindow*		handle;
	int						width;
	int						height;
};