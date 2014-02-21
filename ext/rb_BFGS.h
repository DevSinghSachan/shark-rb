#ifndef RB_BFGS_H
#define RB_BFGS_H

#include <shark/Algorithms/GradientDescent/BFGS.h> //resilient propagation as optimizer
#include "rb_conversions.h"
class rb_BFGS {

	public:
		static VALUE rb_class;
		BFGS algorithm;
		rb_BFGS();

};

VALUE rb_BFGS::rb_class = rb_optimizer_bfgs_klass;

#endif