#ifndef RB_BINARYCD_H
#define RB_BINARYCD_H

#include "ruby.h"
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"
#include "rb_RealVector.h"
#include "rb_classes.h"

using namespace shark;
using namespace std;

class rb_BinaryCD {
		BinaryCD _objective;
	public:
		static VALUE rb_class;
		BinaryCD& objective();
		rb_BinaryCD(BinaryRBM &);
};

void Init_BinaryCD();

VALUE rb_BinaryCD::rb_class = rb_optimizer_binarycd_klass;

#endif