#include "rb_RealVector.h"

extern VALUE rb_array_klass;
extern VALUE rb_optimizer_realvector_klass;

//extern VALUE method_realvector_export_pgm; // this is a promise!

using namespace shark;
using namespace std;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/vector/rb_vector_methods.extras"

VALUE rb_RealVector::rb_class () {
	return rb_optimizer_realvector_klass;
}

rb_RealVector::rb_RealVector(RealVector const& _data) {
	data = _data;
}

RealVector* rb_RealVector::getData() {
	return &data;
}

rb_RealVector::rb_RealVector() {};

VALUE method_realvector_allocate (VALUE klass) {
	return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector()
			);
}

VALUE method_realvector_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	rb_RealVector *v;
	Data_Get_Struct(self, rb_RealVector, v);

	if (TYPE(dataset) == T_ARRAY) {
		v->data = rb_ary_to_1d_realvector(dataset);
	} else if (TYPE(dataset) == T_FIXNUM) {
		method_vector_resize<rb_RealVector>(self, dataset);
		if (rb_block_given_p()) {
			for (size_t i = 0; i< v->getData()->size();i++) {
				(*(v->getData()))(i) = NUM2DBL(rb_yield(INT2FIX(i)));
			}
		}
	}
	return self;
}

VALUE method_rb_ary_to_realvector (VALUE self) {
	if (RARRAY_LEN(self) > 0) {
		if (TYPE(rb_ary_entry(self, 0)) != T_FIXNUM && TYPE(rb_ary_entry(self, 0)) != T_FLOAT)
			rb_raise(rb_eArgError, "Can only convert to RealVector 1-dimensional Float or Fixnum vectors (please be gentle).");
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(rb_ary_to_1d_realvector(self))
			);
	} else {
		return wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector()
			);
	}
	return self;
}

void Init_RealVector () {

	Init_VectorMethods<rb_RealVector>();

	rb_define_method(rb_array_klass, "to_realvector", (rb_method)method_rb_ary_to_realvector, 0);
	rb_define_alloc_func(rb_optimizer_realvector_klass, (rb_alloc_func_t) method_realvector_allocate);
	rb_define_method(rb_optimizer_realvector_klass, "initialize", (rb_method)method_realvector_initialize,-1);

}