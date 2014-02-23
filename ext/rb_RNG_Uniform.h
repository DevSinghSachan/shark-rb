#ifndef RB_RNG_UNIFORM_H
#define RB_RNG_UNIFORM_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Uniform.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Uniform {

	public:
		static VALUE rb_class();
		shark::Uniform< shark::Rng::rng_type >   model;
		shark::Uniform< shark::Rng::rng_type > * getModel();
		rb_RNG_Uniform();
};

void Init_rb_RNG_Uniform();

#endif
