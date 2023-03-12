#pragma once
struct GLFWwindow;

class Window {
public:
	Window();
	void					Init();
	GLFWwindow*				GetHandle() const { return handle;};
	
	int						GetWidth()	{ return width; }
	int						GetHeight()	 { return height; }
private:
	struct GLFWwindow*		handle;
	int						width;
	int						height;
};