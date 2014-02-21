#ifndef RB_RBM_BINARY_LAYER_H
#define RB_RBM_BINARY_LAYER_H

#include <shark/Unsupervised/RBM/Neuronlayers/BinaryLayer.h>
#include "ruby.h"
#include "rb_RealVector.h"
#include "rb_RealVectorReference.h"
#include "rb_RealMatrix.h"
#include "rb_RealMatrixReference.h"
#include "rb_conversions.h"

class rb_RBMBinaryLayer {

	public:
		BinaryLayer model;
		BinaryLayer* getData();
		rb_RBMBinaryLayer::rb_RBMBinaryLayer(BinaryLayer const &);

};

void Init_RBMBinaryLayer();

#endif