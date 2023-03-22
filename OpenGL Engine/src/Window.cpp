#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Input.h"


Window::Window() {

}

void Window::Init() {
    if ( !glfwInit() )
        std::cout << "[ERROR] could not init glfw" << std::endl;

    width = 1280;
    height = 720;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    handle = glfwCreateWindow( width, height, "Hello World", NULL, NULL );
    glfwSetWindowUserPointer( handle, this );

    if ( !handle ) {
        glfwTerminate();
    }
    
    glfwMakeContextCurrent( handle );
    //gladLoadGL();

    if ( !gladLoadGLLoader( ( GLADloadproc ) glfwGetProcAddress ) )
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }


    glfwMakeContextCurrent( handle );

    glfwSwapInterval( 0 );

    glfwSetKeyCallback( handle, Input::KeyCallback );
}