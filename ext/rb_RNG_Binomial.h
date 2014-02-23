#ifndef RB_RNG_BINOMIAL_H
#define RB_RNG_BINOMIAL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Binomial.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Binomial {

	public:
		static VALUE rb_class();
		shark::Binomial< shark::Rng::rng_type >   model;
		shark::Binomial< shark::Rng::rng_type > * getModel();
		rb_RNG_Binomial();
};

void Init_rb_RNG_Binomial();

#endif
