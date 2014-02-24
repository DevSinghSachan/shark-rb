#ifndef RB_RNG_TRUNCATEDEXPONENTIAL_H
#define RB_RNG_TRUNCATEDEXPONENTIAL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/TruncatedExponential.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_TruncatedExponential {

	public:
		static VALUE rb_class();
		shark::TruncatedExponential< shark::Rng::rng_type >   model;
		shark::TruncatedExponential< shark::Rng::rng_type > * getDistribution();
		shark::TruncatedExponential< shark::Rng::rng_type > * getModel();

		rb_RNG_TruncatedExponential();
};

void Init_rb_RNG_TruncatedExponential();

#endif
