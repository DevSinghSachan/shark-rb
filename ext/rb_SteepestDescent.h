#ifndef RB_STEEPEST_DESCENT_H
#define RB_STEEPEST_DESCENT_H

#include "ruby.h"
#include "rb_AbstractSingleObjectiveOptimizer.h"
#include <shark/Algorithms/GradientDescent/SteepestDescent.h> //resilient propagation as optimizer
#include "rb_BinaryCD.h"

using namespace shark;
class rb_SteepestDescent : rb_AbstractSingleObjectiveOptimizer {

	public:
		SteepestDescent _algorithm;
		SteepestDescent algorithm();
		void step(const ObjectiveFunctionType &);
		rb_SteepestDescent();

};

void Init_Steepest_Descent();

#endif