#include "Animation.h"
#include "Model.h"
#include <iostream>

Animator::Animator() {
	animation = nullptr;
	currentFrame = 0;
	maxFrames = 0;
	looping = true;
}

int Animator::GetAnimationIndex( float time, std::vector<float> times) {
	
	for ( int i = 0; i < times.size(); i++ ) {
		if ( time < times[i] )
			return i;
	}
	//if empty or too far
	return -1;
}

glm::mat4 Animator::ComputeNode( Node* node ) {
	node->computedOffset = node->t.Matrix();//todo see if i even need computedOffset or can i just nuke

	if ( !animation )
		return node->t.Matrix();

	//todo map to find channel? maybe precompute this and just use int for index
	for ( int i = 0; i < animation->animChannels.size(); i++ ) {
		AnimChannel* anim = &animation->animChannels[i];
		if ( anim->nodeID == node->index && anim->rotations.size() > 0 ) {
			Transform t;
			int index = ( int ) currentFrame;
			t.SetRotation( glm::eulerAngles( glm::quat( anim->rotations[index].rotaiton ) ) );
			t.SetPosition( anim->translations[index].translation );
			t.SetScale( anim->scales[index].scale );
			return t.Matrix();
		}
	}
	//if not part of animation just return default
	return node->t.Matrix();
}