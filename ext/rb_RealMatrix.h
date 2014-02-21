#ifndef RB_REALMATRIX_H
#define RB_REALMATRIX_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_RealVector.h"
#include "rb_RealMatrixRow.h"
#include "rb_RealMatrixColumn.h"
#include "rb_RealMatrixReference.h"
#include "rb_classes.h"

using namespace std;
using namespace shark;
class rb_RealMatrix {
	public:
		static VALUE rb_class;
		rb_RealMatrix(RealMatrix const &);
		rb_RealMatrix();
		RealMatrix data;
		RealMatrix* getData();

};

VALUE rb_RealMatrix::rb_class = rb_optimizer_realmatrix_klass;

void Init_RealMatrix ();

#endif