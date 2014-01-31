#ifndef RB_BINARYCD_H
#define RB_BINARYCD_H

#include "ruby.h"
#include <shark/Unsupervised/RBM/BinaryRBM.h>
#include "rb_UnlabeledData.h"
#include "rb_conversions.h"

using namespace shark;
using namespace std;

class rb_BinaryCD {

	public:
		BinaryCD objective;
		rb_BinaryCD();

};

void Init_BinaryCD();

#endif