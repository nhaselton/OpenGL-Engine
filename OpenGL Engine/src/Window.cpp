#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Input.h"


Window::Window() {

}

void Window::Init( unsigned int width, unsigned int height ) {
    this->width = width;
    this->height = height;
    if ( !glfwInit() )
        std::cout << "[ERROR] could not init glfw" << std::endl;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    handle = glfwCreateWindow( width, height, "Hello World", NULL, NULL );
    glfwSetWindowUserPointer( handle, this );

    if ( !handle ) {
        glfwTerminate();
    }
    
    glfwMakeContextCurrent( handle );

    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    
    glfwMakeContextCurrent( handle );
    glfwSwapInterval( 0 );
    glfwSetKeyCallback( handle, Input::KeyCallback );
}