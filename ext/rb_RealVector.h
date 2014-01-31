#ifndef RB_REALVECTOR_H
#define RB_REALVECTOR_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealMatrix.h"

using namespace std;
using namespace shark;
class rb_RealVector {
	public:
		rb_RealVector(RealVector);
		rb_RealVector();
		void fill(double);
		void remove_NaN(double);
		RealVector data;
};

void Init_RealVector ();

#endif