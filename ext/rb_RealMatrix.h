#ifndef RB_REALMATRIX_H
#define RB_REALMATRIX_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include "rb_conversions.h"
#include "rb_Datatypes.h"


using namespace std;
using namespace shark;
class rb_RealMatrix {
	public:
		static VALUE rb_class();
		rb_RealMatrix(RealMatrix const &);
		rb_RealMatrix(RealMatrix &);
		rb_RealMatrix();
		RealMatrix data;
		RealMatrix* getData();

};

void Init_RealMatrix ();

#endif