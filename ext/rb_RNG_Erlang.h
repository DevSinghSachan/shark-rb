#ifndef RB_RNG_ERLANG_H
#define RB_RNG_ERLANG_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Erlang.h>
#include "ruby.h"
#include "rb_conversions.h"

class rb_RNG_Erlang {

	public:
		static VALUE rb_class();
		shark::Erlang< shark::Rng::rng_type >   model;
		shark::Erlang< shark::Rng::rng_type > * getModel();

		rb_RNG_Erlang();
};

void Init_rb_RNG_Erlang();

#endif
