#ifndef RB_BINARYRBM_H
#define RB_BINARYRBM_H

#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_RealMatrix.h"
#include "rb_RealMatrixReference.h"
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BarsAndStripes.h"
#include "rb_RBM_Analytics.h"
#include "rb_SteepestDescent.h"
#include "rb_RBMBinaryLayer.h"
#include <shark/Data/Dataset.h>
#include <shark/Unsupervised/RBM/BinaryRBM.h>


class rb_BinaryRBM {
	public:
		static VALUE rb_class();
		BinaryRBM model;
		BinaryRBM* getModel();
		rb_BinaryRBM();
		UnlabeledData<RealVector> eval(UnlabeledData<RealVector>const&);
};

void Init_BinaryRBM ();

#endif