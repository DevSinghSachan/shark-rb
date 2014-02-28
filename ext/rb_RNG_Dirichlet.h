#ifndef RB_RNG_DIRICHLET_H
#define RB_RNG_DIRICHLET_H

#include <shark/Rng/GlobalRng.h>
#include <shark/Rng/Dirichlet.h>
#include "ruby.h"
#include "rb_conversions.h"
#include "rb_Datatypes.h"

class rb_RNG_Dirichlet {

	public:
		static VALUE rb_class();
		shark::Dirichlet< shark::Rng::rng_type >   model;
		shark::Dirichlet< shark::Rng::rng_type > * getModel();

		rb_RNG_Dirichlet();
};

void Init_rb_RNG_Dirichlet();

#endif
