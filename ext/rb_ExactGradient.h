#ifndef RB_BINARYCD_H
#define RB_BINARYCD_H

#include "ruby.h"
#include <shark/Unsupervised/RBM/GradientApproximations/ExactGradient.h>
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"

using namespace shark;
using namespace std;

class rb_ExactGradient {

	public:
		ExactGradient _objective;
		ExactGradient objective();
		rb_ExactGradient(BinaryRBM &);

};

void Init_ExactGradient();

#endif