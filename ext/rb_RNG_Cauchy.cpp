#include "rb_RNG_Cauchy.h"
#include "extras/utils/rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_rng_cauchy;

VALUE rb_RNG_Cauchy::rb_class() {
	return rb_optimizer_rng_cauchy;
}

VALUE method_rb_RNG_Cauchy_set_n (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	r->getModel()->n(parameter_1);
	return self;
}

VALUE method_rb_RNG_Cauchy_set_set_range (VALUE self, VALUE parameter_1, VALUE parameter_2) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	r->getModel()->setRange(parameter_1, parameter_2);
	return self;
}

VALUE method_rb_RNG_Cauchy_get_sigma (VALUE self) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	return NUM2DBL(r->getModel()->sigma());
}

VALUE method_rb_RNG_Cauchy_get_median (VALUE self) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	return NUM2DBL(r->getModel()->median());
}

VALUE method_rb_RNG_Cauchy_get_p (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	return NUM2DBL(r->getModel()->prob(parameter_1));
}

VALUE method_rb_RNG_Cauchy_get_prob (VALUE self, VALUE parameter_1) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	return NUM2DBL(r->getModel()->prob(parameter_1));
}

VALUE method_rb_RNG_Cauchy_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Cauchy>(
		rb_RNG_Cauchy::rb_class(),
		new rb_RNG_Cauchy()
		);
}

VALUE method_rb_RNG_Cauchy_sample (VALUE self) {
	rb_RNG_Cauchy *r;
	Data_Get_Struct(self, rb_RNG_Cauchy, r);
	return NUM2DBL((*r->getModel())());
}

void Init_rb_RNG_Cauchy () {
	rb_define_method(rb_RNG_Cauchy::rb_class(), "n=", (rb_method) method_rb_RNG_Cauchy_set_n, 1);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "set_range=", (rb_method) method_rb_RNG_Cauchy_set_set_range, 2);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "sigma", (rb_method) method_rb_RNG_Cauchy_get_sigma, 0);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "median", (rb_method) method_rb_RNG_Cauchy_get_median, 0);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "p", (rb_method) method_rb_RNG_Cauchy_get_p, 1);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "prob", (rb_method) method_rb_RNG_Cauchy_get_prob, 1);
	rb_define_alloc_func(rb_RNG_Cauchy::rb_class(), (rb_alloc_func_t) method_rb_RNG_Cauchy_allocate);
	rb_define_method(rb_RNG_Cauchy::rb_class(), "sample", (rb_method) method_rb_RNG_Cauchy_sample, 0);
}