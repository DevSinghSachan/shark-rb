#include "rb_RNG_Cauchy.h"
#include "extras/utils/rb_pointer_wrapping.extras"

rb_RNG_Cauchy::rb_RNG_Cauchy() : model(shark::Rng::globalRng) {}

shark::Cauchy< shark::Rng::rng_type > * rb_RNG_Cauchy::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_cauchy_klass;

VALUE rb_RNG_Cauchy::rb_class() {
	return rb_optimizer_rng_cauchy_klass;
}

VALUE method_rb_RNG_Cauchy_set_median (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FLOAT && TYPE(parameter_1) != T_FIXNUM)
		rb_raise(rb_eArgError, "1st Argument must be a Float.");

	r->getModel()->median(NUM2DBL(parameter_1));
	return self;
}

VALUE method_rb_RNG_Cauchy_set_sigma (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FLOAT && TYPE(parameter_1) != T_FIXNUM)
		rb_raise(rb_eArgError, "1st Argument must be a Float.");

	r->getModel()->sigma(NUM2DBL(parameter_1));
	return self;
}

VALUE method_rb_RNG_Cauchy_get_sigma (VALUE self) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);

	return rb_float_new(r->getModel()->sigma());
}

VALUE method_rb_RNG_Cauchy_get_median (VALUE self) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);

	return rb_float_new(r->getModel()->median());
}

VALUE method_rb_RNG_Cauchy_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FLOAT && TYPE(parameter_1) != T_FIXNUM)
		rb_raise(rb_eArgError, "1st Argument must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_Cauchy_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	// Checking whether parameter_1 is a "double"
	if (TYPE(parameter_1) != T_FLOAT && TYPE(parameter_1) != T_FIXNUM)
		rb_raise(rb_eArgError, "1st Argument must be a Float.");

	return rb_float_new(r->getModel()->p(NUM2DBL(parameter_1)));
}

VALUE method_rb_RNG_Cauchy_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Cauchy>(rb_RNG_Cauchy::rb_class(), new rb_RNG_Cauchy());
}

VALUE method_rb_RNG_Cauchy_initialize (VALUE self) {
	return self;
}

VALUE method_rb_RNG_Cauchy_sample (VALUE self) {
	rb_RNG_Cauchy * r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);

	return rb_float_new((*(r->getModel()))());
}

void Init_rb_RNG_Cauchy () {
	rb_define_method(rb_RNG_Cauchy::rb_class(), "median=", (rb_method) method_rb_RNG_Cauchy_set_median, 1);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "sigma=", (rb_method) method_rb_RNG_Cauchy_set_sigma, 1);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "sigma", (rb_method) method_rb_RNG_Cauchy_get_sigma, 0);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "median", (rb_method) method_rb_RNG_Cauchy_get_median, 0);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "p", (rb_method) method_rb_RNG_Cauchy_get_p, 1);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "prob", (rb_method) method_rb_RNG_Cauchy_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Cauchy::rb_class(), (rb_alloc_func_t) method_rb_RNG_Cauchy_allocate);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "initialize", (rb_method) method_rb_RNG_Cauchy_initialize, 0);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "sample", (rb_method) method_rb_RNG_Cauchy_sample, 0);
}