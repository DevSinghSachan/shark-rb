#include "rb_BarsAndStripes.h"

extern VALUE rb_optimizer_unlabeleddata_klass;
extern VALUE rb_optimizer_barsandstripes_klass;

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

rb_BarsAndStripes::rb_BarsAndStripes() {};

VALUE method_barsandstripes_get_data(VALUE self) {
	rb_BarsAndStripes *p;
	Data_Get_Struct(self, rb_BarsAndStripes, p);

	return wrap_pointer<rb_UnlabeledData>(
		rb_optimizer_unlabeleddata_klass,
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

typedef VALUE (*rb_method)(...);


void Init_BarsAndStripes () {
	rb_define_alloc_func(rb_optimizer_barsandstripes_klass, (rb_alloc_func_t) method_barsandstripes_allocate);
	rb_define_method(rb_optimizer_barsandstripes_klass, "initialize", (rb_method) method_barsandstripes_initialize, 0);
	rb_define_method(rb_optimizer_barsandstripes_klass, "input_dimension", (rb_method) method_barsandstripes_get_input_dimensions, 0);
	rb_define_method(rb_optimizer_barsandstripes_klass, "data", (rb_method) method_barsandstripes_get_data, 0);
}