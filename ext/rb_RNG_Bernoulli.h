#ifndef RB_RNG_BERNOULLI_H
#define RB_RNG_BERNOULLI_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Bernoulli.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Bernoulli {

	public:
		static VALUE rb_class();
		shark::Bernoulli< shark::Rng::rng_type >   model;
		shark::Bernoulli< shark::Rng::rng_type > * getDistribution();
		shark::Bernoulli< shark::Rng::rng_type > * getModel();

		rb_RNG_Bernoulli();
};

void Init_rb_RNG_Bernoulli();

#endif
