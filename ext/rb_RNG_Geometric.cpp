#include "rb_RNG_Geometric.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Geometric::rb_RNG_Geometric() : model(shark::Rng::globalRng) {}

shark::Geometric< shark::Rng::rng_type > * rb_RNG_Geometric::getDistribution() {
	return &model;
}

shark::Geometric< shark::Rng::rng_type > * rb_RNG_Geometric::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_geometric_klass;

VALUE rb_RNG_Geometric::rb_class() {
	return rb_optimizer_rng_geometric_klass;
}

VALUE method_rb_RNG_Geometric_set_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Geometric * r;
	Data_Get_Struct(self, rb_RNG_Geometric)
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getDistribution()->prob(rb_float_new(parameter_1));
	return self
}

VALUE method_rb_RNG_Geometric_set_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Geometric * r;
	Data_Get_Struct(self, rb_RNG_Geometric)
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	r->getDistribution()->prob(rb_float_new(parameter_1));
	return self
}

VALUE method_rb_RNG_Geometric_get_p (VALUE self) {
	rb_RNG_Geometric * r;
	Data_Get_Struct(self, rb_RNG_Geometric)
	
	return rb_float_new(r->getDistribution()->prob());
}

VALUE method_rb_RNG_Geometric_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Geometric * r;
	Data_Get_Struct(self, rb_RNG_Geometric)
	
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");
	return rb_float_new(r->getDistribution()->p(rb_float_new(parameter_1)));
}

VALUE method_rb_RNG_Geometric_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Geometric>(rb_RNG_Geometric::rb_class(), new rb_RNG_Geometric());
}

VALUE method_rb_RNG_Geometric_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Geometric_sample (VALUE self) {
	rb_RNG_Geometric * r;
	Data_Get_Struct(self, rb_RNG_Geometric)
	
	return rb_float_new((*(r->getDistribution()))());
}

void Init_rb_RNG_Geometric () {
	rb_define_method(rb_RNG_Geometric::rb_class(), "p=", (rb_method) method_rb_RNG_Geometric_set_p, 1);
	rb_define_method(rb_RNG_Geometric::rb_class(), "prob=", (rb_method) method_rb_RNG_Geometric_set_prob, 1);
	rb_define_method(rb_RNG_Geometric::rb_class(), "p", (rb_method) method_rb_RNG_Geometric_get_p, 0);
	rb_define_method(rb_RNG_Geometric::rb_class(), "prob", (rb_method) method_rb_RNG_Geometric_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Geometric::rb_class(), (rb_alloc_func_t) method_rb_RNG_Geometric_allocate);
	rb_define_method(rb_RNG_Geometric::rb_class(), "initialize", (rb_method) method_rb_RNG_Geometric_initialize, 0);
	rb_define_method(rb_RNG_Geometric::rb_class(), "sample", (rb_method) method_rb_RNG_Geometric_sample, 0);
}