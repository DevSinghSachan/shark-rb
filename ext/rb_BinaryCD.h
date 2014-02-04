#ifndef RB_BINARYCD_H
#define RB_BINARYCD_H

#include "ruby.h"
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"
#include "rb_BinaryRBM.h"
#include "rb_RealVector.h"

using namespace shark;
using namespace std;

class rb_BinaryCD {

	public:
		BinaryCD _objective;
		BinaryCD objective();
		rb_BinaryCD(BinaryRBM &);

};

void Init_BinaryCD();

#endif