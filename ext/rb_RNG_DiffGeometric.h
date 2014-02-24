#ifndef RB_RNG_DIFFGEOMETRIC_H
#define RB_RNG_DIFFGEOMETRIC_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/DiffGeometric.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_DiffGeometric {

	public:
		static VALUE rb_class();
		shark::DiffGeometric< shark::Rng::rng_type >   model;
		shark::DiffGeometric< shark::Rng::rng_type > * getModel();

		rb_RNG_DiffGeometric();
};

void Init_rb_RNG_DiffGeometric();

#endif
