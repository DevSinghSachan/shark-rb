#include "rb_BinaryCD.h"

extern VALUE rb_optimizer_binarycd_klass;
extern VALUE rb_optimizer_unlabeleddata_klass;

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

rb_BinaryCD::rb_BinaryCD(BinaryRBM *rbm): objective(rbm) {};

void static raise_objective_func_data_error () {
	rb_raise(rb_eArgError, "An objective function's data can only be set using UnlabeledData or Arrays.");
}

VALUE method_binarycd_set_k (VALUE self, VALUE rb_k) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);

	Check_Type(rb_k, T_FIXNUM);
	b->objective.setK(NUM2INT(rb_k));

	return self;
}

VALUE method_binarycd_get_number_of_variables (VALUE self) {
	rb_BinaryCD *b;
	Data_Get_Struct(self, rb_BinaryCD, b);
	return INT2FIX(b->objective.numberOfVariables());
}

VALUE method_binarycd_set_data (VALUE self, VALUE rb_data) {
	if (TYPE(rb_data) == T_DATA) {

		if (CLASS_OF(rb_data) != rb_optimizer_unlabeleddata_klass)
			raise_objective_func_data_error();

		rb_BinaryCD *b;
		Data_Get_Struct(self, rb_BinaryCD, b);

		rb_UnlabeledData *d;
		Data_Get_Struct(rb_data, rb_UnlabeledData, d);

		b->objective.setData(d->data);
	} else if (TYPE(rb_data) == T_ARRAY) {

		rb_BinaryCD *b;
		Data_Get_Struct(self, rb_BinaryCD, b);

		b->objective.setData(rb_ary_to_unlabeleddata(rb_data));

	} else {
		raise_objective_func_data_error();
	}
	return self;
};

typedef VALUE (*rb_method)(...);

void Init_BinaryCD () {
	rb_define_method(rb_optimizer_binarycd_klass, "data=", (rb_method) method_binarycd_set_data, 1);
	rb_define_method(rb_optimizer_binarycd_klass, "number_of_variables", (rb_method) method_binarycd_get_number_of_variables, 0);
	rb_define_method(rb_optimizer_binarycd_klass, "k=", (rb_method) method_binarycd_set_k, 1);
}