#include "rb_LinearModel.h"
using namespace std;
using namespace shark;

extern VALUE rb_optimizer_linearmodel_klass;

#include "extras/utils/rb_pointer_wrapping.extras"
#include "extras/models/rb_abstract_model.extras"

VALUE rb_LinearModel::rb_class () {
	return rb_optimizer_linearmodel_klass;
}

LinearModel<>* rb_LinearModel::getModel() {
	return &model;
}
Data<RealVector> rb_LinearModel::operator()(Data<RealVector> const& patterns) {
	return model(patterns);
};
RealVector rb_LinearModel::operator()(RealVector const& patterns) {
	return model(patterns);
};
Data<RealVector> rb_LinearModel::eval(Data<RealVector> const& patterns) {
	return model(patterns);
};
RealVector rb_LinearModel::eval(RealVector const& patterns) {
	return model(patterns);
};
rb_LinearModel::rb_LinearModel() {
};
rb_LinearModel::rb_LinearModel(LinearModel<RealVector> _model) {
	model = _model;
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix) {
	LinearModel<> *new_model = new LinearModel<>(matrix, *new RealVector());
	model = *new_model;
};
rb_LinearModel::rb_LinearModel(RealMatrix const& matrix, RealVector const& offset) {
	LinearModel<> *new_model = new LinearModel<>(matrix, offset);
	model = *new_model;
};

VALUE method_linearmodel_allocate (VALUE klass) {
	return wrap_pointer<rb_LinearModel>(
			rb_LinearModel::rb_class(),
			new rb_LinearModel()
		);
}

VALUE method_linearmodel_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	
	VALUE rb_matrix,
			rb_offsets,
			rb_bias;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"03",
		&rb_matrix,
		&rb_offsets,
		&rb_bias);

	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);

	if (TYPE(rb_matrix) == T_DATA) {

		if (CLASS_OF(rb_matrix) != rb_RealMatrix::rb_class())
			rb_raise(rb_eArgError, "LinearModel takes either:\n - a number of inputs, a number of outputs, and a boolean choice whether to use offsets, OR\n - a RealMatrix and a RealVector, OR\n - no parameters.");

		rb_RealMatrix *mat;
		Data_Get_Struct(rb_matrix, rb_RealMatrix, mat);

		if (TYPE(rb_offsets) == T_DATA) {
			// With offsets vector.
			if (CLASS_OF(rb_offsets) != rb_RealVector::rb_class())
				rb_raise(rb_eArgError, "LinearModel takes either:\n - a number of inputs, a number of outputs, and a boolean choice whether to use offsets, OR\n - a RealMatrix and a RealVector, OR\n - no parameters.");
			rb_RealVector *vec;
			Data_Get_Struct(rb_offsets, rb_RealVector, vec);
			m->getModel()->setStructure(mat->data, vec->data);

		} else {
			// Without offsets vector.
			m->getModel()->setStructure(mat->data, *new RealVector());

		}

	} else if (TYPE(rb_matrix) == T_FIXNUM && TYPE(rb_offsets) == T_FIXNUM) {
		// Using numbers to choose structure.
		m->getModel()->setStructure(NUM2INT(rb_matrix), NUM2INT(rb_offsets), rb_bias == Qtrue ? true : false);
	} else {
		// No settings for structure.
	}
	return self;
}

VALUE method_linearmodel_offset(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return wrap_pointer<rb_RealVector>(
		rb_RealVector::rb_class(),
		new rb_RealVector(m->getModel()->offset())
	);
};

VALUE method_linearmodel_matrix(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return wrap_pointer<rb_RealMatrix>(
		rb_RealMatrix::rb_class(),
		new rb_RealMatrix(m->getModel()->matrix())
	);
};

VALUE method_linearmodel_outputSize(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return INT2FIX(m->getModel()->outputSize());
};

VALUE method_linearmodel_inputSize(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return INT2FIX(m->getModel()->inputSize());
};

VALUE method_linearmodel_hasOffset(VALUE self) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);
	return (m->getModel()->hasOffset() ? Qtrue : Qfalse);
};

void linearmodel_rb_error_unmatched_dimensions(shark::Exception e) {
	rb_raise(rb_eRuntimeError, "%s\nCheck that the the data you are inputting matches the linear models's input size.", e.what());
}

VALUE method_linearmodel_eval(VALUE self, VALUE dataset) {
	rb_LinearModel *m;
	Data_Get_Struct(self, rb_LinearModel, m);

	if (TYPE(dataset) == T_ARRAY) {

		if (RARRAY_LEN(dataset) > 0) {

			if (TYPE(rb_ary_entry(dataset, 0)) == T_ARRAY) {
				// 2D array.
				try {
					return wrap_pointer<rb_UnlabeledData>(
						rb_UnlabeledData::rb_class(),
						new rb_UnlabeledData(m->eval(rb_ary_to_unlabeleddata(dataset)))
					);
				} catch (shark::Exception e) {
					linearmodel_rb_error_unmatched_dimensions(e);
				}
			} else {
				// 1D array
				try {
					return wrap_pointer<rb_RealVector>(
						rb_RealVector::rb_class(),
						new rb_RealVector(m->eval(rb_ary_to_1d_realvector(dataset)))
					);
				} catch (shark::Exception e) {
					linearmodel_rb_error_unmatched_dimensions(e);
				}
			}
		} else {
			rb_raise(rb_eArgError, "Can only evaluate non-empty data. Look at LinearModel#offset for the constant part of the transformation.");
		}
	} else {
		Check_Type(dataset, T_DATA);

		if (CLASS_OF(dataset) == rb_UnlabeledData::rb_class()) {
			rb_UnlabeledData *d;
			Data_Get_Struct(dataset, rb_UnlabeledData, d);

			try {
				return wrap_pointer<rb_UnlabeledData>(
					rb_UnlabeledData::rb_class(),
					new rb_UnlabeledData(m->eval(d->data))
				);
			} catch (shark::Exception e) {
				linearmodel_rb_error_unmatched_dimensions(e);
			}
		} else if (CLASS_OF(dataset) == rb_RealVector::rb_class()) {
			rb_RealVector *d;
			Data_Get_Struct(dataset, rb_RealVector, d);

			try {
				return wrap_pointer<rb_RealVector>(
					rb_RealVector::rb_class(),
					new rb_RealVector(m->eval(d->data))
				);
			} catch (shark::Exception e) {
				linearmodel_rb_error_unmatched_dimensions(e);
			}
		} else {
			rb_raise(rb_eArgError, "LinearModel can evalute UnlabeledData, RealVectors, or Arrays.");
		}
		return self;
	}
};

void Init_LinearModel() {
	// Shark LinearModel class:
	InitAbstractModel<rb_LinearModel>();
	rb_define_alloc_func(rb_optimizer_linearmodel_klass,  (rb_alloc_func_t) method_linearmodel_allocate);
	rb_define_method(rb_optimizer_linearmodel_klass, "initialize", (rb_method)method_linearmodel_initialize, -1);
	rb_define_method(rb_optimizer_linearmodel_klass, "set_structure", (rb_method)method_linearmodel_initialize, -1);
	rb_define_method(rb_optimizer_linearmodel_klass, "offset" ,(rb_method) method_linearmodel_offset, 0);
	rb_define_method(rb_optimizer_linearmodel_klass, "matrix",(rb_method) method_linearmodel_matrix, 0);
	rb_define_method(rb_optimizer_linearmodel_klass, "output_size",(rb_method) method_linearmodel_outputSize, 0);
	rb_define_method(rb_optimizer_linearmodel_klass, "input_size",(rb_method) method_linearmodel_inputSize, 0);
	rb_define_method(rb_optimizer_linearmodel_klass, "has_offset?",(rb_method) method_linearmodel_hasOffset, 0);
	rb_define_method(rb_optimizer_linearmodel_klass, "eval",(rb_method) method_linearmodel_eval, 1);
}