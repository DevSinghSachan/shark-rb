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

VALUE method_rb_RNG_Dirichlet_set_lambdas (VALUE self, VALUE parameter_1, VALUE parameter_2) {
	rb_RNG_Dirichlet *r;
	
	// Checking whether parameter_1 is an "array"
	if (TYPE(parameter_1) != T_ARRAY || (RARRAY_LEN(parameter_1) > 0 && TYPE(rb_ary_entry(parameter_1, 0)) != T_FLOAT && TYPE(rb_ary_entry(parameter_1, 0)) != T_FIXNUM) && CLASS_OF(parameter_1) != rb_RealVector::rb_class() && CLASS_OF(parameter_1) != rb_RealVectorReference::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixColumn::rb_class() && CLASS_OF(parameter_1) != rb_RealMatrixRow::rb_class())
		rb_raise(rb_eArgError, "Argument 1 must be an ArrayType (\"RealVector\", \"RealVectorReference\", \"RealMatrixColumn\", \"RealMatrixRow\", \"Array\").");

	// Checking whether parameter_2 is a "matrix"
	if (TYPE(parameter_2) != T_ARRAY || (RARRAY_LEN(parameter_2) > 0 && TYPE(rb_ary_entry(ary, 0))) != T_ARRAY || (RARRAY_LEN(parameter_2) > 0 && RARRAY_LEN(rb_ary_entry(parameter_2, 0)) > 0 && TYPE(rb_ary_entry(rb_ary_entry(parameter_2, 0), 0)) != T_FLOAT && TYPE(rb_ary_entry(rb_ary_entry(parameter_2, 0), 0)) != T_FIXNUM) && CLASS_OF(parameter_2) != rb_RealMatrix::rb_class() && CLASS_OF(parameter_2) != rb_RealMatrixReference::rb_class())
		rb_raise(rb_eArgError, "Argument 2 must be an MatrixType (\"RealMatrix\", \"RealMatrixReference\", \"Array< Array< Float > >\").");

	// Converting parameters "parameter_1", "parameter_2" before they can be used.
	if (CLASS_OF(parameter_1) == rb_RealVector::rb_class()) {
		rb_RealVector * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealVector, parameter_1_converted);
		if (CLASS_OF(parameter_2) == rb_RealMatrix::rb_class()) {
			rb_RealMatrix * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrix, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		} else if (CLASS_OF(parameter_2) == rb_RealMatrixReference::rb_class()) {
			rb_RealMatrixReference * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrixReference, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		}
	} else if (CLASS_OF(parameter_1) == rb_RealVectorReference::rb_class()) {
		rb_RealVectorReference * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealVectorReference, parameter_1_converted);
		if (CLASS_OF(parameter_2) == rb_RealMatrix::rb_class()) {
			rb_RealMatrix * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrix, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		} else if (CLASS_OF(parameter_2) == rb_RealMatrixReference::rb_class()) {
			rb_RealMatrixReference * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrixReference, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		}
	} else if (CLASS_OF(parameter_1) == rb_RealMatrixColumn::rb_class()) {
		rb_RealMatrixColumn * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealMatrixColumn, parameter_1_converted);
		if (CLASS_OF(parameter_2) == rb_RealMatrix::rb_class()) {
			rb_RealMatrix * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrix, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		} else if (CLASS_OF(parameter_2) == rb_RealMatrixReference::rb_class()) {
			rb_RealMatrixReference * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrixReference, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		}
	} else if (CLASS_OF(parameter_1) == rb_RealMatrixRow::rb_class()) {
		rb_RealMatrixRow * parameter_1_converted;
		Data_Get_Struct(parameter_1, rb_RealMatrixRow, parameter_1_converted);
		if (CLASS_OF(parameter_2) == rb_RealMatrix::rb_class()) {
			rb_RealMatrix * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrix, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		} else if (CLASS_OF(parameter_2) == rb_RealMatrixReference::rb_class()) {
			rb_RealMatrixReference * parameter_2_converted;
			Data_Get_Struct(parameter_2, rb_RealMatrixReference, parameter_2_converted);
			r->getModel()->lambdas(*(parameter_1_converted->getData()), *(parameter_2_converted->getData()));
			return self;
		}
	}

	return self; // cpp functions require return variable, so if all tests fail "self" is returned.
}

VALUE method_rb_RNG_Dirichlet_get_lambdas (VALUE self) {
	rb_RNG_Dirichlet *r;
	
	return wrap_pointer<rb_RealVectorReference>(rb_RealVectorReference::rb_class(), new rb_RealVectorReference(&(r->getModel()->lambdas())));
}

VALUE method_rb_RNG_Dirichlet_allocate (VALUE klass) {
	return wrap_pointer<rb_RNG_Dirichlet>(rb_RNG_Dirichlet::rb_class(), new rb_RNG_Dirichlet());
}

VALUE method_rb_RNG_Dirichlet_initialize (VALUE self) {
	return self;
}

void Init_rb_RNG_Dirichlet () {
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "lambdas=", (rb_method) method_rb_RNG_Dirichlet_set_lambdas, 2);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "lambdas", (rb_method) method_rb_RNG_Dirichlet_get_lambdas, 0);
	rb_define_alloc_func(rb_RNG_Dirichlet::rb_class(), (rb_alloc_func_t) method_rb_RNG_Dirichlet_allocate);
	rb_define_method(rb_RNG_Dirichlet::rb_class(), "initialize", (rb_method) method_rb_RNG_Dirichlet_initialize, 0);
}