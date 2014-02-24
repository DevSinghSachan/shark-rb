#ifndef RB_RNG_NEGEXPONENTIAL_H
#define RB_RNG_NEGEXPONENTIAL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/NegExponential.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_NegExponential {

	public:
		static VALUE rb_class();
		shark::NegExponential< shark::Rng::rng_type >   model;
		shark::NegExponential< shark::Rng::rng_type > * getModel();

		rb_RNG_NegExponential();
};

void Init_rb_RNG_NegExponential();

#endif
