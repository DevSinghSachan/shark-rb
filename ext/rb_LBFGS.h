#ifndef RB_LBFGS_H
#define RB_LBFGS_H

#include "ruby.h"
#include <shark/Algorithms/GradientDescent/LBFGS.h> //resilient propagation as optimizer
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"
#include "rb_BinaryCD.h"
#include "rb_SolutionSet.h"
#include "rb_classes.h"

using namespace shark;
using namespace std;

class rb_LBFGS {
		LBFGS _algorithm;
	public:
		static VALUE rb_class;
		LBFGS & algorithm();
		rb_LBFGS();

};

VALUE rb_LBFGS::rb_class = rb_optimizer_lbfgs_klass;

void Init_LBFGS();

#endif