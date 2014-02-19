#ifndef RB_REALVECTOR_H
#define RB_REALVECTOR_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealMatrix.h"
#include "rb_RealVectorReference.h"

using namespace std;
using namespace shark;
class rb_RealVector {
	public:
		rb_RealVector(RealVector const &);
		rb_RealVector();
		RealVector data;
		RealVector* getData();
};

void Init_RealVector ();
VALUE stdvector_realvector_to_rb_ary_of_realvectors(const std::vector<RealVector>&);

#endif