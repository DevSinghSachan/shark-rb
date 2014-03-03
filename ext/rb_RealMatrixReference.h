#ifndef RB_REALMATRIX_REFERENCE_H
#define RB_REALMATRIX_REFERENCE_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_Datatypes.h"


using namespace std;
using namespace shark;
class rb_RealMatrixReference {
	public:
		static VALUE rb_class();
		rb_RealMatrixReference(RealMatrix *);
		RealMatrix* data;
		RealMatrix* getData();

};

void Init_RealMatrixReference ();

#endif