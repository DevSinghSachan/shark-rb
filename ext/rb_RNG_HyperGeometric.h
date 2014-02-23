#ifndef RB_RNG_HYPERGEOMETRIC_H
#define RB_RNG_HYPERGEOMETRIC_H

#include <shark/Rng/HyperGeometric.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_HyperGeometric {

	public:
		static VALUE rb_class();
		shark::HyperGeometric< shark::Rng::rng_type >   model;
		shark::HyperGeometric< shark::Rng::rng_type > * getModel();
		rb_RNG_HyperGeometric();
};

void Init_rb_RNG_HyperGeometric();

#endif
