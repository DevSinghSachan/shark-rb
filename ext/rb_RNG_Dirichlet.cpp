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

VALUE method_rb_RNG_Dirichlet_set_lambdas (VALUE self, VALUE parameter_1) {
	rb_RNG_Dirichlet *r;
	
		if (TYPE(parameter_1) != T_ARRAY && CLASS_OF(parameter_1) != rb_RealVector::rb_class() && CLASS_OF(parameter_1) != rb_RealVectorReference::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixColumn::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixRow::rb_class())
			rb_raise(rb_eArgError, "Argument 1 must be an ArrayType (\"RealVector\", \"RealVectorReference\", \"RealMatrixColumn\", \"RealMatrixRow\", \"Array\").");
	

if (CLASS_OF(parameter_1) == rb_RealVector::rb_class()) {

	rb_RealVector * parameter_1_converted;
	Data_Get_Struct(parameter_1, rb_RealVector, parameter_1_converted);
r->getModel()->lambdas((* parameter_1_converted->getData()));
}else if (CLASS_OF(parameter_1) == rb_RealVectorReference::rb_class()) {

	rb_RealVectorReference * parameter_1_converted;
	Data_Get_Struct(parameter_1, rb_RealVectorReference, parameter_1_converted);
r->getModel()->lambdas((* parameter_1_converted->rb_RealVectorReference));
}else if (CLASS_OF(parameter_1) == rb_RealMatrixColumn::rb_class()) {

	rb_RealMatrixColumn * parameter_1_converted;
	Data_Get_Struct(parameter_1, rb_RealMatrixColumn, parameter_1_converted);
r->getModel()->lambdas((* parameter_1_converted->getData()));
}else if (CLASS_OF(parameter_1) == rb_RealMatrixRow::rb_class()) {

	rb_RealMatrixRow * parameter_1_converted;
	Data_Get_Struct(parameter_1, rb_RealMatrixRow, parameter_1_converted);
r->getModel()->lambdas((* parameter_1_converted->getData()));
}
;
;


VALUE method_rb_RNG_Dirichlet_get_lambdas (VALUE self) {
	rb_RNG_Dirichlet *r;
	
	r->getModel()->lambdas();
	return self;


VALUE method_rb_RNG_Dirichlet_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Dirichlet>(
		rb_RNG_Dirichlet::rb_class(),
		new rb_RNG_Dirichlet()
		);
}

VALUE method_rb_RNG_Dirichlet_initialize (VALUE self) {
	return self;
}

void Init_rb_RNG_Dirichlet () {
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "lambdas=", (rb_method) method_rb_RNG_Dirichlet_set_lambdas, 1);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "lambdas", (rb_method) method_rb_RNG_Dirichlet_get_lambdas, 0);
	rb_define_alloc_func(rb_RNG_Dirichlet::rb_class(), (rb_alloc_func_t) method_rb_RNG_Dirichlet_allocate);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "initialize", (rb_method) method_rb_RNG_Dirichlet_initialize, 0);
}