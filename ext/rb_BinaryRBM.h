#ifndef RB_BINARYRBM_H
#define RB_BINARYRBM_H

#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_SteepestDescent.h"
#include <shark/Unsupervised/RBM/BinaryRBM.h>
class rb_BinaryRBM {

	public:
		BinaryRBM rbm;
		rb_BinaryRBM();

};

void Init_BinaryRBM ();

#endif