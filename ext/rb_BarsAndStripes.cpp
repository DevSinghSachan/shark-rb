#include "rb_BarsAndStripes.h"

extern VALUE rb_optimizer_barsandstripes_klass;

#include "extras/utils/rb_pointer_wrapping.extras"

VALUE rb_BarsAndStripes::rb_class() {
	return rb_optimizer_barsandstripes_klass;
}

rb_BarsAndStripes::rb_BarsAndStripes() {};

VALUE method_barsandstripes_get_data(VALUE self) {
	rb_BarsAndStripes *p;
	Data_Get_Struct(self, rb_BarsAndStripes, p);

	return wrap_pointer<rb_UnlabeledData>(
		rb_UnlabeledData::rb_class(),
		new rb_UnlabeledData(p->problem.data())
		);
}

VALUE method_barsandstripes_allocate(VALUE klass) {
	return wrap_pointer<rb_BarsAndStripes>(
		klass,
		new rb_BarsAndStripes()
		);
}

VALUE method_barsandstripes_initialize(VALUE self) {
	return self;
}

VALUE method_barsandstripes_get_input_dimensions (VALUE self) {
	rb_BarsAndStripes *p;
	Data_Get_Struct(self, rb_BarsAndStripes, p);

	return INT2FIX(p->problem.inputDimension());
}

void Init_BarsAndStripes () {
	rb_define_alloc_func(rb_optimizer_barsandstripes_klass, (rb_alloc_func_t) method_barsandstripes_allocate);
	rb_define_method(rb_optimizer_barsandstripes_klass, "initialize", (rb_method) method_barsandstripes_initialize, 0);
	rb_define_method(rb_optimizer_barsandstripes_klass, "input_dimension", (rb_method) method_barsandstripes_get_input_dimensions, 0);
	rb_define_method(rb_optimizer_barsandstripes_klass, "data", (rb_method) method_barsandstripes_get_data, 0);
}