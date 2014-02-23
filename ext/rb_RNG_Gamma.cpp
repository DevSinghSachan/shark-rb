#include "rb_RNG_Gamma.h"
#include "extras/utils/rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_rng_gamma_klass;

VALUE rb_RNG_Gamma::rb_class() {
	return rb_optimizer_rng_gamma_klass;
}

VALUE method_rb_RNG_Gamma_set_k (VALUE self, VALUE parameter_1) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getModel()->k(NUM2DBL(parameter_1));
	return self;
}

VALUE method_rb_RNG_Gamma_set_theta (VALUE self, VALUE parameter_1) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	r->getModel()->theta(NUM2DBL(parameter_1));
	return self;
}

VALUE method_rb_RNG_Gamma_get_k (VALUE self) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	return rb_float_new(r->getModel()->k());
}

VALUE method_rb_RNG_Gamma_get_theta (VALUE self) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	return rb_float_new(r->getModel()->theta());
}

VALUE method_rb_RNG_Gamma_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_Gamma_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_Gamma_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Gamma>(
		rb_RNG_Gamma::rb_class(),
		new rb_RNG_Gamma()
		);
}

VALUE method_rb_RNG_Gamma_sample (VALUE self) {
	rb_RNG_Gamma *r;
	Data_Get_Struct(self, rb_RNG_Gamma, r);

	return rb_float_new((*r->getModel())());
}

void Init_rb_RNG_Gamma () {
	rb_define_method(rb_RNG_Gamma::rb_class(), "k=", (rb_method) method_rb_RNG_Gamma_set_k, 1);
	rb_define_method(rb_RNG_Gamma::rb_class(), "theta=", (rb_method) method_rb_RNG_Gamma_set_theta, 1);
	rb_define_method(rb_RNG_Gamma::rb_class(), "k", (rb_method) method_rb_RNG_Gamma_get_k, 0);
	rb_define_method(rb_RNG_Gamma::rb_class(), "theta", (rb_method) method_rb_RNG_Gamma_get_theta, 0);
	rb_define_method(rb_RNG_Gamma::rb_class(), "p", (rb_method) method_rb_RNG_Gamma_get_p, 1);
	rb_define_method(rb_RNG_Gamma::rb_class(), "prob", (rb_method) method_rb_RNG_Gamma_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Gamma::rb_class(), (rb_alloc_func_t) method_rb_RNG_Gamma_allocate);
	rb_define_method(rb_RNG_Gamma::rb_class(), "sample", (rb_method) method_rb_RNG_Gamma_sample, 0);
}