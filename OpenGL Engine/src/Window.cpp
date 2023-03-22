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

    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    //create second window first so the renderer one with input appears on top
#if SECOND_WINDOW
    window2 = glfwCreateWindow( width, height, "Show Shadows", NULL, NULL );
#endif

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

    glfwSwapInterval( 0 );

    glfwSetKeyCallback( handle, Input::KeyCallback );
}