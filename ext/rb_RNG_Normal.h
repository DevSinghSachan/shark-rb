#ifndef RB_RNG_NORMAL_H
#define RB_RNG_NORMAL_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Normal.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Normal {

	public:
		static VALUE rb_class();
		shark::Normal< shark::Rng::rng_type >   model;
		shark::Normal< shark::Rng::rng_type > * getModel();

		rb_RNG_Normal();
};

void Init_rb_RNG_Normal();

#endif
