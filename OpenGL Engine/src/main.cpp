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
* doing now
* Engine: SAT
* do next
* Render: DRAW LINES
* 
   ====     RENDERING       ======  
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
         12) Shadow Types Dynamic ( Done )
             a) directional ( Done )
             b) point ( Done )
             c) spot ( Done )
        16) Animated shadow casted Imp inside of sponza ( Done ) 
        13) Skybox ( Done ) 
        14) Draw Lines
        15) add lightmap size as light param, calcshadow uses 1/1024 for texel size, not 1/lightmapsize
        16) Animation Interpolation
        17) lights only update when changed
        18) shadow atlas only updates on change 
        Eventually
         Log draw calls
         Uber Shaders
         Frustum Culling
         Light "Frustum" Culling
   ======== ENGINE ========
        1) Transforms use velocity to move ( Done ) 
        2) Tick System
            a) Update Every 1/60 & draw infinitly ( Done ) 
            b) interpolate & predict Camera ( Done ) 
        3) Consistent Window size ( Done )
        4) Individual entity animations and non recursive draw ( Done ) 
        5) basic SAT
            Collision check
            Collison info ( penetration depth, normal )
        Eventually
         Broad Phase Checking
         BSP
   ======== GAME  ========

   */

int main( void ) {    
    Common common;
    common.Init();  
    common.Frame();
}