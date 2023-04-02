#pragma once
struct GLFWwindow;

#define SECOND_WINDOW 1

class Window {
public:
	Window();
	void					Init(unsigned int width, unsigned int height);
	GLFWwindow*				GetHandle() const { return handle;};
	
	int						GetWidth()	{ return width; }
	int						GetHeight()	 { return height; }

private:
	struct GLFWwindow*		handle;
	int						width;
	int						height;
};