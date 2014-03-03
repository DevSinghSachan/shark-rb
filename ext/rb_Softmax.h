#ifndef RB_SOFTMAX_H
#define RB_SOFTMAX_H

#include <shark/Models/Softmax.h>
#include "rb_Datatypes.h"
#include "ruby.h"
#include "rb_conversions.h"
#include "extras/models/rb_abstract_model.extras"

class rb_Softmax {

	public:
		static VALUE rb_class();
		shark::Softmax   model;
		shark::Softmax * getModel();

		rb_Softmax();
};

void Init_rb_Softmax();

#endif
