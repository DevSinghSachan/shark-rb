#ifndef RB_RNG_GEOMETRIC_H
#define RB_RNG_GEOMETRIC_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Geometric.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Geometric {

	public:
		static VALUE rb_class();
		shark::Geometric< shark::Rng::rng_type >   model;
		shark::Geometric< shark::Rng::rng_type > * getDistribution();
		shark::Geometric< shark::Rng::rng_type > * getModel();

		rb_RNG_Geometric();
};

void Init_rb_RNG_Geometric();

#endif
