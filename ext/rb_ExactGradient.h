#ifndef RB_EXACTGRADIENT_H
#define RB_EXACTGRADIENT_H

#include "ruby.h"
#include <shark/Unsupervised/RBM/GradientApproximations/ExactGradient.h>
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"
#include "rb_classes.h"

using namespace shark;
using namespace std;

class rb_ExactGradient {
		ExactGradient<BinaryRBM> _objective;
	public:
		static VALUE rb_class;
		ExactGradient<BinaryRBM>& objective();
		rb_ExactGradient(BinaryRBM &);

};

VALUE rb_ExactGradient::rb_class = rb_optimizer_exactgradient_klass;

void Init_ExactGradient();

#endif