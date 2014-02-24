#ifndef RB_RNG_LOGNORMAL_H
#define RB_RNG_LOGNORMAL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/LogNormal.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_LogNormal {

	public:
		static VALUE rb_class();
		shark::LogNormal< shark::Rng::rng_type >   model;
		shark::LogNormal< shark::Rng::rng_type > * getModel();

		rb_RNG_LogNormal();
};

void Init_rb_RNG_LogNormal();

#endif
