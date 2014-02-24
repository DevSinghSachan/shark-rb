#ifndef RB_RNG_POISSON_H
#define RB_RNG_POISSON_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Poisson.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Poisson {

	public:
		static VALUE rb_class();
		shark::Poisson< shark::Rng::rng_type >   model;
		shark::Poisson< shark::Rng::rng_type > * getModel();

		rb_RNG_Poisson();
};

void Init_rb_RNG_Poisson();

#endif
