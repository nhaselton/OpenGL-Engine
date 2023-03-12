#pragma once

enum TextureType {
	TEXTURE_TYPE_ALBEDO,
	TEXTURE_TYPE_NORMAL,
	TEXTURE_TYPE_SPECULAR,
};

struct Texture {
	TextureType tType;
	unsigned int textureID;
	unsigned int width;
	unsigned int height;
};