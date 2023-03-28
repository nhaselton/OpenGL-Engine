#pragma once
#include <vector>
#include <glm/glm.hpp>

#include <glm/gtc/quaternion.hpp>
#include <string>

struct TranslationKeyFrame {
	glm::vec3 translation;
	float time;
};

struct RotationKeyFrame {
	glm::quat rotaiton;
	float time;
};

struct ScaleKeyFrame {
	glm::vec3 scale;
	float time;
};

struct AnimChannel {
	int nodeID;

	std::vector<TranslationKeyFrame> translations;
	std::vector<RotationKeyFrame> rotations;
	std::vector<ScaleKeyFrame> scales;
};

struct Animation {
	std::string name;
	std::vector<AnimChannel> animChannels;
};

class Animator {
public:
	Animator();

	int			currentFrame;
	int			maxFrames;
	bool		looping;
 
public:
	void		SetAnimation( Animation* animation ) { this->animation = animation; }
	glm::mat4	ComputeNode( struct Node* node);
	//private:
	Animation*	animation;
//private:
	int GetAnimationIndex( float time, std::vector<float> times);
};
