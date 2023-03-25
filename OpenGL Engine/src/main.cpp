#include <vector>
#include <GLFW/glfw3.h>
#include "glad/glad.h"

#include "Common.h"
#include "Window.h"

#include "Shader.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* TODO LIST BY THE TIME I DIE */
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
    7) Skeletal Animation ( Done ) 
    8) Shadow types Static
        a) directional ( Done )
        b) point ( Done )
        c) spot ( Fix )
    11) Basic Shadow Atlas
    9) different sized shadow casters
    10) Multiple Shader casters
    11) Depth prepas
    12) Shadow Types Dynamic
        a) directional
        b) point
        c) spot
   13) Animated shadow casted Imp inside of sponza 
   14) Uber Shaders

    Light Culling
   */

int main( void ) {    
    Common common;
    common.Init();
    common.Frame();
}