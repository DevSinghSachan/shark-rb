#ifndef RB_BFGS_H
#define RB_BFGS_H

#include <shark/Algorithms/GradientDescent/BFGS.h> //resilient propagation as optimizer
#include "rb_conversions.h"
class rb_BFGS {

	public:
		BFGS algorithm;
		rb_BFGS();

};

#endif