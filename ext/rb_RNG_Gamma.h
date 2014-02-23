#ifndef RB_RNG_GAMMA_H
#define RB_RNG_GAMMA_H

#include <shark/Rng/Gamma.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Gamma {

	public:
		static VALUE rb_class();
		shark::Gamma< shark::Rng::rng_type >   model;
		shark::Gamma< shark::Rng::rng_type > * getModel();
		rb_RNG_Gamma();
};

void Init_rb_RNG_Gamma();

#endif
