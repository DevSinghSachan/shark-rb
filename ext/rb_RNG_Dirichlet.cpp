#include "rb_RNG_Dirichlet.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Dirichlet::rb_RNG_Dirichlet() : model(shark::Rng::globalRng) {}

shark::Dirichlet< shark::Rng::rng_type > * rb_RNG_Dirichlet::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_dirichlet_klass;

VALUE rb_RNG_Dirichlet::rb_class() {
	return rb_optimizer_rng_dirichlet_klass;
}

VALUE method_rb_RNG_Dirichlet_set_alphas (VALUE self, VALUE parameter_1) {
	rb_RNG_Dirichlet * r;
	Data_Get_Struct(self, rb_RNG_Dirichlet, r);
	
	r->getModel()->alphas(parameter_1);
	return self;
}

VALUE method_rb_RNG_Dirichlet_get_alphas (VALUE self) {
	rb_RNG_Dirichlet * r;
	Data_Get_Struct(self, rb_RNG_Dirichlet, r);
	
	return realvector_to_rb_ary(r->getModel()->alphas());
}

VALUE method_rb_RNG_Dirichlet_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Dirichlet>(rb_RNG_Dirichlet::rb_class(), new rb_RNG_Dirichlet());
}

VALUE method_rb_RNG_Dirichlet_initialize (VALUE self) {
	return self;
}

void Init_rb_RNG_Dirichlet () {
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "alphas=", (rb_method) method_rb_RNG_Dirichlet_set_alphas, 1);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "alphas", (rb_method) method_rb_RNG_Dirichlet_get_alphas, 0);
	rb_define_alloc_func(rb_RNG_Dirichlet::rb_class(), (rb_alloc_func_t) method_rb_RNG_Dirichlet_allocate);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "initialize", (rb_method) method_rb_RNG_Dirichlet_initialize, 0);
}