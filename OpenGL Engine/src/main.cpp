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
        c) spot ( Done )
    11) Basic Shadow Atlas (Done)
    9) different sized shadow casters ( Done )
    10) Multiple Shader casters ( Done )
    10.5) Refactor & Clean ( Done )
    11) Depth prepas ( Done )
    12) Shadow Types Dynamic
        a) directional
        b) point
        c) spot
   13) Skybox
   14) lights only update when changed
   15) shadow atlas only updates on change 
   16) Animated shadow casted Imp inside of sponza 
   17) Uber Shaders

   Eventually
    1) Log draw calls
    2) Frustum Culling
    3) Light "Frustum" Culling
   */

int main( void ) {    
    Common common;
    common.Init();
    common.Frame();
}