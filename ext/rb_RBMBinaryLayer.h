#ifndef RB_RBM_BINARY_LAYER_H
#define RB_RBM_BINARY_LAYER_H

#include <shark/Unsupervised/RBM/Neuronlayers/BinaryLayer.h>
#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_RealVectorReference.h"
#include "rb_RealMatrix.h"
#include "rb_RealMatrixReference.h"
#include "rb_conversions.h"
#include "rb_classes.h"

class rb_RBMBinaryLayer {

	public:
		static VALUE rb_class;
		BinaryLayer model;
		BinaryLayer* getData();
		rb_RBMBinaryLayer(BinaryLayer const &);

};

VALUE rb_RBMBinaryLayer::rb_class = rb_optimizer_rbm_binarylayer_klass;

void Init_RBMBinaryLayer();

#endif