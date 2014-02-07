#ifndef RB_STEEPEST_DESCENT_H
#define RB_STEEPEST_DESCENT_H

#include "ruby.h"
#include <shark/Algorithms/GradientDescent/SteepestDescent.h> //resilient propagation as optimizer
#include "rb_BinaryCD.h"
#include "rb_ExactGradient.h"
#include "rb_SolutionSet.h"
#include "rb_conversions.h"

using namespace shark;

class rb_SteepestDescent {
		SteepestDescent _algorithm;
	public:
		SteepestDescent& algorithm();
		rb_SteepestDescent();

};

void Init_Steepest_Descent();

#endif