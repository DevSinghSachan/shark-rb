#ifndef RB_RNG_WEIBULL_H
#define RB_RNG_WEIBULL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Weibull.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Weibull {

	public:
		static VALUE rb_class();
		shark::Weibull< shark::Rng::rng_type >   model;
		shark::Weibull< shark::Rng::rng_type > * getModel();
		rb_RNG_Weibull();
};

void Init_rb_RNG_Weibull();

#endif
