#include <vector>
#include <GLFW/glfw3.h>
#include "glad/glad.h"

#include "Common.h"
#include "Window.h"

#include "Shader.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* TODO LIST BY 3/12 */
/*
    1) Textures (Done)
    2) Lighting (Done)
        a) Directional (Done) 
        b) Point (Done)
        c) Spot (Done)
    3) ImGUI (Done) 
    4) Mappings for point light( Done )
        a) spec map  (Done)
        b) normalmap (Done)
    5) static GLTF Model import ( Done ) 
    6) static "Ubershader"  ( Done ) 
    7) Skeletal Animation
    8) Shadows
        a) directional
        b) point
        c) spot
   9) Animated shadow casted Imp inside of sponza
*/

int main( void ) {
    Common common;
    common.Init();
    common.Frame();
}