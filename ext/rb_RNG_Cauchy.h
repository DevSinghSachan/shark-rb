#ifndef RB_RNG_CAUCHY_H
#define RB_RNG_CAUCHY_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Cauchy.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Cauchy {

	public:
		static VALUE rb_class();
		shark::Cauchy< shark::Rng::rng_type >   model;
		shark::Cauchy< shark::Rng::rng_type > * getModel();

		rb_RNG_Cauchy();
};

void Init_rb_RNG_Cauchy();

#endif
