#ifndef RB_GAUSSIANBINARY_RBM_H
#define RB_GAUSSIANBINARY_RBM_H

#include <shark/Unsupervised/RBM/GaussianBinaryRBM.h>
#include "rb_conversions.h"
#include "rb_classes.h"
class rb_GaussianBinaryRBM {

	public:
		static VALUE rb_class;
		GaussianBinaryRBM rbm;
		rb_GaussianBinaryRBM();


};

VALUE rb_GaussianBinaryRBM::rb_class = rb_optimizer_gaussbinaryrbm_klass;

#endif