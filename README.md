# OpenGL-Engine
OpenGL Graphics Learning

This was made to learn the basics of game engines, it was suppose to be a week long spring break project but instead took months. It's messy, crude, and full of "magic numbers".

---FEATURES---
Renderer
  1) custom .GLTF importer with support for both animations and meshes
  2) Normal/Specular maps
  3) Full skeletal animation
  4) Point, Spot, Directional lights, all shadow casting with shadow maps
  5) Shadows are put into an 8192x8192 texture atlas, when a new light is created and has shadows enabeld it looks for an empty spot in the atlas (its a 512x512 pixel grid on the cpu side)
  6) self shadows
  7) Skyboxes

"Physics"
1) SAT collision detection which returns hit normal & depth for collision resolution
2) OOBs, with a way to visualize them in the renderer
note: shortly after starting physics i realzied i would need to rewrite most of the non renderering code, so i plan to just start a new project for it!


![Lights](https://github.com/nhaselton/OpenGL-Engine/blob/master/screenshots/GreenSpot.png)

![Maps](https://github.com/nhaselton/OpenGL-Engine/blob/master/screenshots/Normals.png)

![C](https://github.com/nhaselton/OpenGL-Engine/blob/master/screenshots/directional.png)
