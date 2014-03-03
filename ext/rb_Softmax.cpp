#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/models/rb_abstract_model.extras"
#include "rb_Softmax.h"

rb_Softmax::rb_Softmax() : model(shark::Rng::globalRng) {}

shark::Softmax * rb_Softmax::getModel() {
	return &model;
}

extern VALUE rb_optimizer_rng_poisson_klass;

VALUE rb_Softmax::rb_class() {
	return rb_optimizer_rng_poisson_klass;
}

VALUE method_rb_Softmax_get_number_of_outputs (VALUE self) {
	rb_Softmax * r;
	Data_Get_Struct(self, rb_Softmax, r);
	
	return INT2FIX(r->getModel()->outputSize());
}

VALUE method_rb_Softmax_get_number_of_inputs (VALUE self) {
	rb_Softmax * r;
	Data_Get_Struct(self, rb_Softmax, r);
	
	return INT2FIX(r->getModel()->inputSize());
}

VALUE method_rb_Softmax_allocate (VALUE klass) {
	return wrap_pointer<rb_Softmax>(rb_Softmax::rb_class(), new rb_Softmax());
}

VALUE method_rb_Softmax_initialize (VALUE self) {
	return self;
}

VALUE method_rb_Softmax_set_structure (VALUE self, VALUE parameter_1) {
	rb_Softmax * r;
	Data_Get_Struct(self, rb_Softmax, r);
	
	// Checking whether parameter_1 is an "int"
	if (TYPE(parameter_1) != T_FIXNUM && TYPE(parameter_1) != T_FLOAT)
		rb_raise(rb_eArgError, "Argument 1 must be an Integer.");
	r->getModel()->setStructure(NUM2INT(parameter_1));
	return self;
}

VALUE method_rb_Softmax_eval (VALUE self, VALUE parameter_1) {
	rb_Softmax * r;
	Data_Get_Struct(self, rb_Softmax, r);
	
	// Checking whether parameter_1 is a "vector"
	if (TYPE(parameter_1) != T_ARRAY || (RARRAY_LEN(parameter_1) > 0 && TYPE(rb_ary_entry(parameter_1, 0)) != T_FLOAT && TYPE(rb_ary_entry(parameter_1, 0)) != T_FIXNUM) && CLASS_OF(parameter_1) != rb_RealVector::rb_class() && CLASS_OF(parameter_1) != rb_RealVectorReference::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixColumn::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixRow::rb_class())
		rb_raise(rb_eArgError, "Argument 1 must be an ArrayType (\"RealVector\", \"RealVector\", \"RealMatrixColumn\", \"RealMatrixRow\", \"Array\").");

	// Converting parameters "parameter_1" before they can be used.
	if (CLASS_OF(parameter_1) == rb_RealVector::rb_class()) {

		rb_RealVector * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealVector, parameter_1_converted);
		RealVector casted_output = (*(r->getModel()))(*(parameter_1_converted->getData()));
		return wrap_pointer<rb_RealVector>(rb_RealVector::rb_class(), new rb_RealVector(casted_output));
	} else if (CLASS_OF(parameter_1) == rb_RealVectorReference::rb_class()) {

		rb_RealVectorReference * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealVectorReference, parameter_1_converted);
		RealVector casted_output = (*(r->getModel()))(*(parameter_1_converted->getData()));
		return wrap_pointer<rb_RealVector>(rb_RealVector::rb_class(), new rb_RealVector(casted_output));
	} else if (CLASS_OF(parameter_1) == rb_RealMatrixColumn::rb_class()) {

		rb_RealMatrixColumn * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealMatrixColumn, parameter_1_converted);
		RealVector casted_output = (*(r->getModel()))(*(parameter_1_converted->getData()));
		return wrap_pointer<rb_RealVector>(rb_RealVector::rb_class(), new rb_RealVector(casted_output));
	} else if (CLASS_OF(parameter_1) == rb_RealMatrixRow::rb_class()) {

		rb_RealMatrixRow * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealMatrixRow, parameter_1_converted);
		RealVector casted_output = (*(r->getModel()))(*(parameter_1_converted->getData()));
		return wrap_pointer<rb_RealVector>(rb_RealVector::rb_class(), new rb_RealVector(casted_output));
	} else if (TYPE(parameter_1) == T_ARRAY) {
		RealVector parameter_1_converted = rb_ary_to_1d_realvector(parameter_1);
		RealVector casted_output = (*(r->getModel()))(parameter_1_converted);
		return wrap_pointer<rb_RealVector>(rb_RealVector::rb_class(), new rb_RealVector(casted_output));
	}

	return self; // cpp functions require return variable, so if all tests fail "self" is returned.
}

void Init_rb_Softmax () {
	rb_define_method(rb_Softmax::rb_class(), "number_of_outputs", (rb_method) method_rb_Softmax_get_number_of_outputs, 0);
	rb_define_method(rb_Softmax::rb_class(), "number_of_inputs", (rb_method) method_rb_Softmax_get_number_of_inputs, 0);
	rb_define_alloc_func(rb_Softmax::rb_class(), (rb_alloc_func_t) method_rb_Softmax_allocate);
	rb_define_method(rb_Softmax::rb_class(), "initialize", (rb_method) method_rb_Softmax_initialize, 0);
	rb_define_method(rb_Softmax::rb_class(), "set_structure", (rb_method) method_rb_Softmax_set_structure, 1);
	rb_define_method(rb_Softmax::rb_class(), "eval", (rb_method) method_rb_Softmax_eval, 1);
InitAbstractModel<rb_Softmax>();
}