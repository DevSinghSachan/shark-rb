#ifndef RB_REALVECTOR_REFERENCE_H
#define RB_REALVECTOR_REFERENCE_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealVector.h"
#include "rb_RealMatrix.h"
#include "rb_classes.h"

using namespace std;
using namespace shark;
class rb_RealVectorReference {
	public:
		static VALUE rb_class;
		rb_RealVectorReference(RealVector *);
		RealVector* data;
		RealVector* getData();

};

void Init_RealVectorReference ();

VALUE rb_RealVectorReference::rb_class = rb_optimizer_realvector_reference_klass;

#endif