#ifndef RB_STEEPEST_DESCENT_H
#define RB_STEEPEST_DESCENT_H

#include "ruby.h"
#include <shark/Algorithms/GradientDescent/SteepestDescent.h> //resilient propagation as optimizer
#include "rb_BinaryCD.h"
#include "rb_ExactGradient.h"
#include "rb_SolutionSet.h"
#include "rb_conversions.h"
#include "rb_classes.h"

using namespace shark;

class rb_SteepestDescent {
		SteepestDescent _algorithm;
	public:
		static VALUE rb_class;
		SteepestDescent& algorithm();
		rb_SteepestDescent();

};

VALUE rb_SteepestDescent::rb_class = rb_optimizer_steepestdescent_klass;

void Init_Steepest_Descent();

#endif