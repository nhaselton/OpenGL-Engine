#include "Animation.h"

int GetAnimationIndex( float time, std::vector<float> times) {
	
	for ( int i = 0; i < times.size(); i++ ) {
		if ( time < times[i] )
			return i;
	}
	//if empty or too far
	return -1;
}