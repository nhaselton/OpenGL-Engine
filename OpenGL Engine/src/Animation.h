#pragma once
#include <vector>

//pass the vector of times from each keyframe ( easier than having 300 functions per keyframe type)
int GetAnimationIndex( float time, std::vector<float> times);
