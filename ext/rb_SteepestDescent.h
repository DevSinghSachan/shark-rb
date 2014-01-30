#ifndef RB_STEEPEST_DESCENT_H
#define RB_STEEPEST_DESCENT_H

#include "ruby.h"
#include <shark/Algorithms/GradientDescent/SteepestDescent.h> //resilient propagation as optimizer
using namespace shark;
class rb_SteepestDescent {

	public:
		SteepestDescent algorithm;
		rb_SteepestDescent();

};

void Init_Steepest_Descent();

#endif