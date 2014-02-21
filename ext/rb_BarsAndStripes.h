#ifndef RB_BARSANDSTRIPES_H
#define RB_BARSANDSTRIPES_H

#include "ruby.h"
#include "rb_conversions.h"
#include "rb_UnlabeledData.h"
#include <shark/Unsupervised/RBM/Problems/BarsAndStripes.h>


class rb_BarsAndStripes {

	public:
		static VALUE rb_class();
		BarsAndStripes problem;
		rb_BarsAndStripes();
};

void Init_BarsAndStripes ();

#endif