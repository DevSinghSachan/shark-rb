#ifndef RB_RNG_DISCRETEUNIFORM_H
#define RB_RNG_DISCRETEUNIFORM_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/DiscreteUniform.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_DiscreteUniform {

	public:
		static VALUE rb_class();
		shark::DiscreteUniform< shark::Rng::rng_type >   model;
		shark::DiscreteUniform< shark::Rng::rng_type > * getModel();

		rb_RNG_DiscreteUniform();
};

void Init_rb_RNG_DiscreteUniform();

#endif
