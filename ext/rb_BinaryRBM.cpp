#include "rb_BinaryRBM.h"
#include "rb_pointer_wrapping.extras"

extern VALUE rb_optimizer_binaryrbm_klass;
extern VALUE rb_optimizer_realvector_klass;
extern VALUE rb_optimizer_unlabeleddata_klass;

#include "rb_abstract_model.extras"

VALUE method_binaryrbm_allocate (VALUE klass) {
	return wrap_pointer<rb_BinaryRBM>(
			rb_optimizer_binaryrbm_klass,
			new rb_BinaryRBM()
		);
}

VALUE method_binaryrbm_initialize (VALUE self) {
	return self;
}

rb_BinaryRBM::rb_BinaryRBM (): model(Rng::globalRng) {};

VALUE method_binaryrbm_set_structure (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_opts_or_visible, rb_hidden;
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"11",
		&rb_opts_or_visible,
		&rb_hidden
		);

	if (TYPE(rb_opts_or_visible) == T_HASH) {

		Check_Type(rb_hash_aref(rb_opts_or_visible, rb_sym_new("visible")), T_FIXNUM);
		Check_Type(rb_hash_aref(rb_opts_or_visible, rb_sym_new("hidden")),  T_FIXNUM);

		r->model.setStructure(
			NUM2INT(rb_hash_aref(rb_opts_or_visible, rb_sym_new("visible"))),
			NUM2INT(rb_hash_aref(rb_opts_or_visible, rb_sym_new("hidden")))
			);
	} else {

		Check_Type(rb_opts_or_visible, T_FIXNUM);
		Check_Type(rb_hidden,          T_FIXNUM);

		r->model.setStructure(NUM2INT(rb_opts_or_visible), NUM2INT(rb_hidden));
	}
	return self;
}

void rb_error_binaryrbm_wrong_format () {
	rb_raise(rb_eArgError, "A BinaryRBM can sample using Arrays, UnlabeledData, RealVectors, or by passing a Hash.");
}

UnlabeledData<RealVector> rb_BinaryRBM::eval(UnlabeledData<RealVector> const& dataset) {

	std::vector<shark::RealVector> evaluated_batches;

	BOOST_FOREACH(RealMatrix const& batch,dataset.batches()) {
		RealMatrix evaluated_batch;
		model.eval(batch, evaluated_batch);

		for (size_t i = 0; i< evaluated_batch.size1(); i++)
			evaluated_batches.push_back(row(evaluated_batch,i));
	}

	return shark::createDataFromRange(evaluated_batches);
}

VALUE method_binaryrbm_eval (VALUE self, VALUE rb_opts) {
	VALUE rb_direction, rb_dataset = Qnil, rb_mean = Qfalse;

	if (TYPE(rb_opts) == T_HASH) {
		rb_dataset   = rb_hash_aref(rb_opts, rb_sym_new("samples"));
		rb_direction = rb_hash_aref(rb_opts, rb_sym_new("direction"));
		rb_mean = rb_hash_aref(rb_opts, rb_sym_new("mean"));
	} else {
		rb_dataset = rb_opts;
	}
	rb_mean = (rb_mean == Qnil || rb_mean != Qtrue) ? Qfalse : Qtrue;
	rb_direction = rb_direction == Qnil ? rb_sym_new("forward") : rb_direction;

	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	if (TYPE(rb_dataset) == T_DATA) {
		if (CLASS_OF(rb_dataset) == rb_optimizer_unlabeleddata_klass) {
			rb_UnlabeledData *d;
			Data_Get_Struct(rb_dataset, rb_UnlabeledData, d);
			
			r->model.evaluationType(rb_direction == rb_sym_new("forward"), rb_mean == Qtrue);
			try {
				return wrap_pointer<rb_UnlabeledData>(
					rb_optimizer_unlabeleddata_klass,
					new rb_UnlabeledData(r->eval(d->data))
				);
			} catch (shark::Exception e) {
				rb_raise(rb_eRuntimeError, (std::string(e.what()) + "\nCheck that the the data you are inputting matches the rbm's hidden or visible dimensions.").c_str());
			}
		} else if (CLASS_OF(rb_dataset) == rb_optimizer_realvector_klass) {
			rb_RealVector *d;
			Data_Get_Struct(rb_dataset, rb_RealVector, d);
			std::vector<RealVector> vectors = realvector_to_stdvector(d->data);
			r->model.evaluationType(rb_direction == rb_sym_new("forward"), rb_mean == Qtrue);
			try {
				return wrap_pointer<rb_UnlabeledData>(
					rb_optimizer_unlabeleddata_klass,
					new rb_UnlabeledData(r->eval(shark::createDataFromRange(vectors)))
				);
			} catch (shark::Exception e) {
				rb_raise(rb_eRuntimeError, (std::string(e.what()) + "\nCheck that the the data you are inputting matches the rbm's hidden or visible dimensions.").c_str());
			}

		} else {
			rb_error_binaryrbm_wrong_format();
		}
	} else if (TYPE(rb_dataset) == T_ARRAY) {

		r->model.evaluationType(rb_direction == rb_sym_new("forward"), rb_mean == Qtrue);
		try {
			return wrap_pointer<rb_UnlabeledData>(
					rb_optimizer_unlabeleddata_klass,
					new rb_UnlabeledData(r->eval(rb_ary_to_unlabeleddata(rb_dataset)))
			);
		} catch (shark::Exception e) {
			rb_raise(rb_eRuntimeError, (std::string(e.what()) + "\nCheck that the the data you are inputting matches the rbm's hidden or visible dimensions.").c_str());
		}

	} else {
		rb_error_binaryrbm_wrong_format();
	}

	return self;
}

VALUE method_binaryrbm_evaluation_type (VALUE self, VALUE rb_forward, VALUE rb_eval_mean) {
	if ((rb_forward != Qtrue || rb_forward != Qfalse) && (rb_eval_mean != Qtrue || rb_eval_mean != Qfalse))
		rb_raise(rb_eArgError, "Evaluation type takes 2 boolean options:\n    1. @param forward whether the forward view should be used false=backwards\n    2. @param evalMean whether the mean state should be returned. false=a sample is\n      returned.");
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	r->model.evaluationType(rb_forward == Qtrue, rb_eval_mean == Qtrue);
	return self;
}
VALUE method_binaryrbm_number_of_hidden_neurons(VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return INT2FIX(r->model.numberOfHN());
}
VALUE method_binaryrbm_number_of_visible_neurons(VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return INT2FIX(r->model.numberOfVN());
}

void Init_BinaryRBM () {
	InitAbstractModel<rb_BinaryRBM>(rb_optimizer_binaryrbm_klass);
	rb_define_alloc_func(rb_optimizer_binaryrbm_klass, (rb_alloc_func_t) method_binaryrbm_allocate);
	rb_define_method(rb_optimizer_binaryrbm_klass, "eval", (rb_method) method_binaryrbm_eval, 1);
	rb_define_method(rb_optimizer_binaryrbm_klass, "set_structure", (rb_method) method_binaryrbm_set_structure, -1);
	rb_define_method(rb_optimizer_binaryrbm_klass, "evaluation_type", (rb_method) method_binaryrbm_evaluation_type, 2);
	rb_define_method(rb_optimizer_binaryrbm_klass, "initialize", (rb_method) method_binaryrbm_initialize, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "number_of_hidden_neurons", (rb_method) method_binaryrbm_number_of_hidden_neurons, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "number_of_visible_neurons", (rb_method) method_binaryrbm_number_of_visible_neurons, 0);
}
