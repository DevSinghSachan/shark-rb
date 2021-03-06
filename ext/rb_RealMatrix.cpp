#include "rb_RealMatrix.h"
using namespace shark;
using namespace std;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/matrix/rb_matrix_methods.extras"

extern VALUE rb_array_klass;
extern VALUE rb_optimizer_realmatrix_klass;

VALUE rb_RealMatrix::rb_class() {
	return rb_optimizer_realmatrix_klass;
}

rb_RealMatrix::rb_RealMatrix() {}
rb_RealMatrix::rb_RealMatrix(RealMatrix const& _data) {
	data = _data;// implicit copy here
}
rb_RealMatrix::rb_RealMatrix(RealMatrix & _data) {
	data = _data;// implicit copy here
}
RealMatrix* rb_RealMatrix::getData() {
	return &data;
}
VALUE method_realmatrix_allocate (VALUE klass) {
	return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix()
			);
}
VALUE method_realmatrix_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset, rb_columns;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"03",
		&dataset,
		&rb_columns);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(dataset) == T_ARRAY) {
		m->data = (RARRAY_LEN(dataset) > 0 && TYPE(rb_ary_entry(dataset, 0)) == T_ARRAY) ?
						rb_ary_to_realmatrix(dataset) : 
						rb_1d_ary_to_realmatrix(dataset);
	} else if (TYPE(rb_columns) == T_FIXNUM && TYPE(dataset) == T_FIXNUM) {
		method_matrix_resize<rb_RealMatrix>(self, dataset, rb_columns);
		if (rb_block_given_p()) {
			for (size_t i = 0; i < m->getData()->size1();i++) {
				for (size_t j = 0; j< m->getData()->size2();j++) {
					(*(m->getData()))(i, j) = NUM2DBL(rb_yield_values(2, INT2FIX(i), INT2FIX(j)));
				}
			}
		}
	}
	return self;
}

VALUE method_rb_ary_to_realmatrix (VALUE self) {
	if (RARRAY_LEN(self) > 0) {
		if (TYPE(rb_ary_entry(self, 0)) != T_ARRAY) {
			if (TYPE(rb_ary_entry(self, 0)) != T_FIXNUM && TYPE(rb_ary_entry(self, 0)) != T_FLOAT)
				rb_raise(rb_eArgError, "Can only convert to RealMatrix 1 to 2-dimensional Float or Fixnum vectors (please be gentle).");
			return wrap_pointer<rb_RealMatrix>(
				rb_RealMatrix::rb_class(),
				new rb_RealMatrix(rb_1d_ary_to_realmatrix(self))
				);
		} else {
			if (RARRAY_LEN(rb_ary_entry(self, 0)) > 0 && TYPE(rb_ary_entry(rb_ary_entry(self, 0),0)) != T_FIXNUM && TYPE(rb_ary_entry(rb_ary_entry(self, 0),0)) != T_FLOAT)
				rb_raise(rb_eArgError, "Can only convert to RealVector 1 to 2-dimensional Float or Fixnum vectors (please be gentle).");
			return wrap_pointer<rb_RealMatrix>(
				rb_RealMatrix::rb_class(),
				new rb_RealMatrix(rb_ary_to_realmatrix(self))
				);
		}
	} else {
		return wrap_pointer<rb_RealMatrix>(
			rb_RealMatrix::rb_class(),
			new rb_RealMatrix()
			);
	}
	return self;
}

void Init_RealMatrix () {
	Init_MatrixMethods<rb_RealMatrix>();
	rb_define_method(rb_array_klass, "to_matrix", (rb_method)method_rb_ary_to_realmatrix, 0);
	rb_define_alloc_func(rb_optimizer_realmatrix_klass, (rb_alloc_func_t) method_realmatrix_allocate);
	rb_define_method(rb_optimizer_realmatrix_klass, "initialize", (rb_method)method_realmatrix_initialize,-1);
}