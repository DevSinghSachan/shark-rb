#ifndef RB_LBFGS_H
#define RB_LBFGS_H

#include "ruby.h"
#include <shark/Algorithms/GradientDescent/LBFGS.h> //resilient propagation as optimizer
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"
#include "rb_BinaryCD.h"

using namespace shark;
using namespace std;

class rb_LBFGS {
	public:
		LBFGS _algorithm;
		LBFGS algorithm();
		rb_LBFGS();

};

void Init_LBFGS();

#endif