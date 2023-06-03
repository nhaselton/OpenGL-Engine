# OpenGL-Engine
OpenGL Graphics Learning

This was made to learn the basics of game engines, it was suppose to be a week long spring break project but instead took months. It's messy, crude, and full of "magic numbers".

---FEATURES---
Renderer
  1) custom .GLTF importer with support for both animations and meshes
  2) Full skeletal animation
  3) Point, Spot, Directional lights, all shadow casting with shadow maps
  4) Shadows are put into an 8192x8192 texture atlas, when a new light is created and has shadows enabeld it looks for an empty spot in the atlas (its a 512x512 pixel grid on the cpu side)
  5) self shadows
  6) Skyboxes

"Physics"
1) SAT collision detection which returns hit normal & depth for collision resolution
2) OOBs, with a way to visualize them in the renderer
note: shortly after starting physics i realzied i would need to rewrite most of the non renderering code, so i plan to just start a new project for it
