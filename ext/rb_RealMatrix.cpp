#include "rb_RealMatrix.h"
using namespace shark;
using namespace std;

extern VALUE rb_optimizer_realmatrix_klass;
extern VALUE rb_optimizer_realvector_klass;


template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

rb_RealMatrix::rb_RealMatrix(RealMatrix _data) {
	data = _data;
}
rb_RealMatrix::rb_RealMatrix() {
}
RealVector rb_RealMatrix::get_row(int i) {
	return row(data, i);
}
RealVector rb_RealMatrix::get_column(int i) {
	return column(data, i);
}

void rb_RealMatrix::fill(double filling) {
	std::fill(data.storage(), data.storage()+(data.size1()*data.size2()), filling);
}

VALUE method_realmatrix_get_sqrt (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	m->data = sqrt(m->data);
	return self;
}
VALUE method_realmatrix_to_ary (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return realmatrix_to_rb_ary(m->data);
}

VALUE method_realmatrix_add (VALUE self, VALUE rb_other) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(rb_other) == T_DATA) {
		if (CLASS_OF(rb_other) == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *other;
			Data_Get_Struct(rb_other, rb_RealMatrix, other);
			if (other->data.size1() == m->data.size1() && other->data.size2() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + other->data)
					);
			} else if (other->data.size2() == m->data.size1() && other->data.size1() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + trans(other->data))
					);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else if (CLASS_OF(rb_other) == rb_optimizer_realvector_klass) {
			rb_RealVector *other;
			Data_Get_Struct(rb_other, rb_RealVector, other);
			if (other->data.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other->data.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else if (TYPE(rb_other) == T_ARRAY) {
		if (RARRAY_LEN(rb_other) > 0 && TYPE(rb_ary_entry(rb_other, 0)) == T_ARRAY) {
			RealMatrix other = rb_ary_to_realmatrix(rb_other);
			if (other.size1() == m->data.size1() && other.size2() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + other)
					);
			} else if (other.size2() == m->data.size1() && other.size1() == m->data.size2()) {
				return wrap_pointer<rb_RealMatrix>(
					rb_optimizer_realmatrix_klass,
					new rb_RealMatrix(m->data + trans(other))
					);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else {
			RealVector other = rb_ary_to_1d_realvector(rb_other);
			if (other.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else {
		rb_raise(rb_eArgError, "You can only add Arrays, RealVectors, and RealMatrices to a RealMatrix.");
	}
	return self;
}

VALUE method_realmatrix_add_equals (VALUE self, VALUE rb_other) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(rb_other) == T_DATA) {
		if (CLASS_OF(rb_other) == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *other;
			Data_Get_Struct(rb_other, rb_RealMatrix, other);
			if (other->data.size1() == m->data.size1() && other->data.size2() == m->data.size2()) {
				m->data += other->data;
			} else if (other->data.size2() == m->data.size1() && other->data.size1() == m->data.size2()) {
				m->data += trans(other->data);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else if (CLASS_OF(rb_other) == rb_optimizer_realvector_klass) {
			rb_RealVector *other;
			Data_Get_Struct(rb_other, rb_RealVector, other);
			if (other->data.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other->data.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other->data));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else if (TYPE(rb_other) == T_ARRAY) {
		if (RARRAY_LEN(rb_other) > 0 && TYPE(rb_ary_entry(rb_other, 0)) == T_ARRAY) {
			RealMatrix other = rb_ary_to_realmatrix(rb_other);
			if (other.size1() == m->data.size1() && other.size2() == m->data.size2()) {
				m->data += other;
			} else if (other.size2() == m->data.size1() && other.size1() == m->data.size2()) {
				m->data += trans(other);
			} else {
				rb_raise(rb_eArgError, "The Matrices you are summing don't have the same dimensions, so their sum is infeasible using our current (human) technology.");
			}
		} else {
			RealVector other = rb_ary_to_1d_realvector(rb_other);
			if (other.size() == m->data.size1()) {
				if (m->data.size2() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(row(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else if (other.size() == m->data.size2()) {
				if (m->data.size1() == 1) {
					return wrap_pointer<rb_RealVector>(
						rb_optimizer_realvector_klass,
						new rb_RealVector(column(m->data, 0) + other));
				} else {
					rb_raise(rb_eArgError, "The RealVector and RealMatrix suffer from incompatible dimensions.");
				}
			} else {
				rb_raise(rb_eArgError, "The RealVector you are adding doesn't have the same dimensions as either RealMatrix dimension, so their sum is infeasible using our current (human) technology.");
			}
		}
	} else {
		rb_raise(rb_eArgError, "You can only add Arrays, RealVectors, and RealMatrices to a RealMatrix.");
	}
	return self;
}

VALUE method_realmatrix_multiply (VALUE self, VALUE multiplier) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix((m->data) * NUM2DBL(multiplier))
			);
	} else if (TYPE(multiplier) == T_DATA) {
		VALUE multiplier_klass = CLASS_OF(multiplier);
		if (multiplier_klass == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *multi_matrix;
			Data_Get_Struct(multiplier, rb_RealMatrix, multi_matrix);
			if ((m->data).size2() != (multi_matrix->data).size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (multi_matrix->data).size1()).str().c_str());
			}/*
			RealMatrix temp(multi_matrix->data.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix->data, temp);*/
			return wrap_pointer<rb_RealMatrix>(
				rb_optimizer_realmatrix_klass,
				new rb_RealMatrix(prod(m->data, multi_matrix->data))
				);
		} else if (multiplier_klass == rb_optimizer_realvector_klass) {
			rb_RealVector *vec;
			Data_Get_Struct(multiplier, rb_RealVector, vec);
			if ((m->data).size2() != (vec->data).size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (vec->data).size()).str().c_str());
			}
			/*RealVector temp(m->data.size2());
			axpy_prod(m->data, vec->data, temp);*/
			// might be a vector?
			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(prod(m->data, vec->data))
				);
		} else {
			rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
		}
	} else if (TYPE(multiplier) == T_ARRAY) {
		if (RARRAY_LEN(multiplier) > 0 && TYPE(rb_ary_entry(multiplier, 0)) == T_ARRAY) {
			RealMatrix multi_matrix = rb_ary_to_realmatrix(multiplier);
			if ((m->data).size2() != multi_matrix.size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % multi_matrix.size1()).str().c_str());
			}/*
			RealMatrix temp(multi_matrix.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix, temp);*/
			return wrap_pointer<rb_RealMatrix>(
				rb_optimizer_realmatrix_klass,
				new rb_RealMatrix(prod(m->data, multi_matrix))
			);
		} else if (RARRAY_LEN(multiplier) > 0) {
			RealVector vec = rb_ary_to_1d_realvector(multiplier);
			if ((m->data).size2() != vec.size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % vec.size()).str().c_str());
			}/*
			RealVector temp(m->data.size2());
			axpy_prod(m->data, vec, temp);*/
			return wrap_pointer<rb_RealVector>(
				rb_optimizer_realvector_klass,
				new rb_RealVector(prod(m->data, vec))
			);
		} else {
			rb_raise(rb_eArgError, "It is tough to multiply by an empty matrix since the computer's imagination is limited by the Kernel.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
	}
	return self;
}
VALUE method_realmatrix_multiply_equals (VALUE self, VALUE multiplier) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	if (TYPE(multiplier) == T_FLOAT || TYPE(multiplier) == T_FIXNUM) {
		(m->data) *= NUM2DBL(multiplier);
	} else if (TYPE(multiplier) == T_DATA) {
		VALUE multiplier_klass = CLASS_OF(multiplier);
		if (multiplier_klass == rb_optimizer_realmatrix_klass) {
			rb_RealMatrix *multi_matrix;
			Data_Get_Struct(multiplier, rb_RealMatrix, multi_matrix);
			if ((m->data).size2() != (multi_matrix->data).size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (multi_matrix->data).size1()).str().c_str());
			}
			// RealMatrix temp(multi_matrix->data.size1(),m->data.size2());
			// axpy_prod(m->data, multi_matrix->data, temp);
			m->data = prod(m->data, multi_matrix->data);
		} else if (multiplier_klass == rb_optimizer_realvector_klass) {
			rb_RealVector *vec;
			Data_Get_Struct(multiplier, rb_RealVector, vec);
			if ((m->data).size2() != (vec->data).size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % (vec->data).size()).str().c_str());
			}
			/*RealVector temp(m->data.size2());
			axpy_prod(m->data, vec->data, temp);*/
			RealMatrix temp_matrix(m->data.size1(), 1);
			column(temp_matrix, 0) = prod(m->data, vec->data);
			// might be a vector?
			m->data = temp_matrix;
		} else {
			rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
		}
	} else if (TYPE(multiplier) == T_ARRAY) {
		if (RARRAY_LEN(multiplier) > 0 && TYPE(rb_ary_entry(multiplier, 0)) == T_ARRAY) {
			RealMatrix multi_matrix = rb_ary_to_realmatrix(multiplier);
			if ((m->data).size2() != multi_matrix.size1()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % multi_matrix.size1()).str().c_str());
			}
			/*RealMatrix temp(multi_matrix.size1(),m->data.size2());
			axpy_prod(m->data, multi_matrix, temp);*/
			m->data = prod(m->data, multi_matrix);
		} else if (RARRAY_LEN(multiplier) > 0) {
			RealVector vec = rb_ary_to_1d_realvector(multiplier);
			if ((m->data).size2() != vec.size()) {
				boost::format error_str("For matrix product A*B incompatible number of A columns (%d) and B rows (%d) for multiplication.");
				rb_raise(rb_eArgError, (error_str % (m->data).size2() % vec.size()).str().c_str());
			}/*
			RealVector temp(m->data.size2());
			axpy_prod(m->data, vec, temp);*/
			RealMatrix temp_matrix(m->data.size1(), 1);
			column(temp_matrix, 0) = prod(m->data, vec);
			m->data = temp_matrix;
		} else {
			rb_raise(rb_eArgError, "It is tough to multiply by an empty matrix since the computer's imagination is limited by the Kernel.");
		}
	} else {
		rb_raise(rb_eArgError, "Can only multiply RealMatrix by a scalar, RealMatrix, Array, or RealVector.");
	}
	return self;
}

VALUE method_realmatrix_divide (VALUE self, VALUE divider) {
	if (TYPE(divider) == T_FLOAT || TYPE(divider) == T_FIXNUM) {
		rb_RealMatrix *m;
		Data_Get_Struct(self, rb_RealMatrix, m);
		m->data /= NUM2DBL(divider);
	} else {
		rb_raise(rb_eArgError, "Can only divide RealMatrix by a number");
	}
	return self;
}

VALUE method_realmatrix_length (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	VALUE size = rb_ary_new2(2);
	// number of rows
	rb_ary_store(size, 0, INT2FIX((m->data).size1()));
	// number of columns
	rb_ary_store(size, 1, INT2FIX((m->data).size2()));
	return size;
}

VALUE method_realmatrix_transpose (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(trans(m->data))
		);
}

VALUE method_realmatrix_size1 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).size1());
}
VALUE method_realmatrix_size2 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).size2());
}
VALUE method_realmatrix_stride1 (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).stride1());
}
VALUE method_realmatrix_stride2 (VALUE self) {
	Check_Type(index, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return INT2FIX((m->data).stride2());
}

VALUE method_realmatrix_get_row (VALUE self, VALUE rb_index) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	int index = NUM2INT(rb_index);

	if (index < 0 && (int)(index + m->data.size2()) < 0)
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	if (index >= (int)(m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->get_row(
			index < 0 ?
				index + m->data.size2() :
				index
			))
		);
}
VALUE method_realmatrix_get_column (VALUE self, VALUE rb_index) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	int index = NUM2INT(rb_index),
		neg_index = 0;

	if (index < neg_index && (index + (int)m->data.size1() < neg_index))
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	if (index >= (int)(m->data).size1())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(m->get_column(
			index < 0 ?
				index + m->data.size1() :
				index
			))
		);
}

VALUE method_realmatrix_clear (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	(m->data).clear();
	return self;
}

VALUE method_realmatrix_fill (VALUE self, VALUE filling) {
	if (TYPE(filling) != T_FLOAT && TYPE(filling) != T_FIXNUM)
		rb_raise(rb_eArgError, "Can only fill RealMatrix with floats.");
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	m->fill(NUM2DBL(filling));
	return self;
}

VALUE method_realmatrix_query (VALUE self, VALUE row, VALUE column) {
	Check_Type(row, T_FIXNUM);
	Check_Type(column, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	int neg_index = 0;
	if (NUM2INT(row) < neg_index || NUM2INT(column) < neg_index)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (int)(m->data).size1() || NUM2INT(column) >= (int)(m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	return rb_float_new((m->data)(NUM2INT(row), NUM2INT(column)));
}

VALUE method_realmatrix_insert (VALUE self, VALUE row, VALUE column, VALUE assignment) {
	Check_Type(row, T_FIXNUM);
	Check_Type(column, T_FIXNUM);
	if (TYPE(assignment) != T_FIXNUM && TYPE(assignment) != T_FLOAT)
		rb_raise(rb_eArgError, "Can only insert floats into RealMatrix.");

	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	int neg_index = 0;
	if (NUM2INT(row) < neg_index || NUM2INT(column) < neg_index)
		rb_raise(rb_eArgError, "Can only access positive positions in RealMatrix");
	if (NUM2INT(row) >= (int)(m->data).size1() || NUM2INT(column) >= (int)(m->data).size2())
		rb_raise(rb_eArgError, "Out of range of RealMatrix");
	(m->data)(NUM2INT(row), NUM2INT(column)) = NUM2DBL(assignment);
	return self;
}

VALUE method_realmatrix_resize (VALUE self, VALUE newRows, VALUE newColumns) {
	Check_Type(newRows, T_FIXNUM);
	Check_Type(newColumns, T_FIXNUM);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	(m->data).resize(NUM2INT(newRows), NUM2INT(newColumns));
	return self;
}

VALUE method_realmatrix_negate (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);
	return wrap_pointer<rb_RealMatrix>(
		rb_optimizer_realmatrix_klass,
		new rb_RealMatrix(-(m->data))
	);
}
VALUE method_realmatrix_allocate (VALUE klass) {
	return wrap_pointer<rb_RealMatrix>(
			rb_optimizer_realmatrix_klass,
			new rb_RealMatrix()
			);
}
VALUE method_realmatrix_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE dataset;
	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"01",
		&dataset);
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	if (TYPE(dataset) == T_ARRAY)
		m->data = (RARRAY_LEN(dataset) > 0 && TYPE(rb_ary_entry(dataset, 0)) == T_ARRAY) ?
						rb_ary_to_realmatrix(dataset) : 
						rb_1d_ary_to_realmatrix(dataset);

	return self;
}

VALUE stdvector_realvector_to_rb_ary_of_realvectors(const std::vector<RealVector> W) {
	VALUE ary = rb_ary_new2((int)W.size());
	for (size_t i = 0; i < W.size(); ++i) {
		rb_ary_store(ary, (int)i, wrap_pointer<rb_RealVector>(
			rb_optimizer_realvector_klass,
			new rb_RealVector(W[i])
		));
	}
	return ary;
}

std::vector<RealVector> realmatrix_to_stdvector_realvector (RealMatrix const& W) {
	std::vector<RealVector> vectors;
	for (size_t i=0; i< W.size1(); i++) {
		vectors.push_back(row(W, i));
	}
	return vectors;
}

VALUE method_realmatrix_to_realvectors (VALUE self) {
	rb_RealMatrix *m;
	Data_Get_Struct(self, rb_RealMatrix, m);

	std::vector<RealVector> vectors = realmatrix_to_stdvector_realvector(m->data);

	return stdvector_realvector_to_rb_ary_of_realvectors(vectors);
}

typedef VALUE (*rb_method)(...);

void Init_RealMatrix () {

	rb_define_method(rb_optimizer_realmatrix_klass, "sqrt", (rb_method)method_realmatrix_get_sqrt,0);
	rb_define_method(rb_optimizer_realmatrix_klass, "to_a", (rb_method)method_realmatrix_to_ary, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "to_realvectors", (rb_method)method_realmatrix_to_realvectors, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "*", (rb_method)method_realmatrix_multiply, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "*=", (rb_method)method_realmatrix_multiply_equals, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "/", (rb_method)method_realmatrix_divide, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "+", (rb_method)method_realmatrix_add, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "+=", (rb_method)method_realmatrix_add_equals, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "length", (rb_method)method_realmatrix_length, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "size", (rb_method)method_realmatrix_length, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "transpose", (rb_method)method_realmatrix_transpose, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "~", (rb_method)method_realmatrix_transpose, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "size1", (rb_method)method_realmatrix_size1, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "size2", (rb_method)method_realmatrix_size2, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "stride1", (rb_method)method_realmatrix_stride1, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "stride2", (rb_method)method_realmatrix_stride2, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "row", (rb_method)method_realmatrix_get_row, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "column", (rb_method)method_realmatrix_get_column, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "col", (rb_method)method_realmatrix_get_column, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "fill", (rb_method)method_realmatrix_fill, 1);
	rb_define_method(rb_optimizer_realmatrix_klass, "clear", (rb_method)method_realmatrix_clear, 0);
	rb_define_method(rb_optimizer_realmatrix_klass, "-@", (rb_method)method_realmatrix_negate,0);
	rb_define_method(rb_optimizer_realmatrix_klass, "[]", (rb_method)method_realmatrix_query,2);
	rb_define_method(rb_optimizer_realmatrix_klass, "resize", (rb_method)method_realmatrix_resize, 2);
	rb_define_method(rb_optimizer_realmatrix_klass, "[]=", (rb_method)method_realmatrix_insert,3);
	rb_define_alloc_func(rb_optimizer_realmatrix_klass, (rb_alloc_func_t) method_realmatrix_allocate);
	rb_define_method(rb_optimizer_realmatrix_klass, "initialize", (rb_method)method_realmatrix_initialize,-1);

}