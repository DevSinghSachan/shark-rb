#ifndef RB_GAUSSIANBINARY_RBM_H
#define RB_GAUSSIANBINARY_RBM_H

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
#include <shark/Unsupervised/RBM/GaussianBinaryRBM.h>

class rb_GaussianBinaryRBM {

	public:
		static VALUE rb_class();
		GaussianBinaryRBM model;
		GaussianBinaryRBM* getModel();
		rb_GaussianBinaryRBM();
};

void Init_GaussianBinaryRBM ();

#endif