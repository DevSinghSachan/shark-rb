#ifndef RB_RBM_GAUSSIAN_LAYER_H
#define RB_RBM_GAUSSIAN_LAYER_H

#include <shark/Unsupervised/RBM/Neuronlayers/GaussianLayer.h>
#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_RealVectorReference.h"
#include "rb_RealMatrix.h"
#include "rb_RealMatrixReference.h"
#include "rb_conversions.h"

class rb_RBMGaussianLayer {

	public:
		static VALUE rb_class();
		GaussianLayer* model;
		GaussianLayer* getModel();
		GaussianLayer* getData();
		rb_RBMGaussianLayer(GaussianLayer*);

};

void Init_RBMGaussianLayer();

#endif