#include "rb_RBM_Analytics.h"

extern VALUE rb_optimizer_rbm_analytics_module;

using namespace shark;
using namespace std;

VALUE method_rbm_analytics_log_partition_function (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_rbm, rb_beta;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"11",
		&rb_rbm,
		&rb_beta);

	Check_Type(rb_rbm, T_DATA);
	if (CLASS_OF(rb_rbm) != rb_BinaryRBM::rb_class())
		rb_raise(rb_eArgError, "Log Partition Function requires an RBM");

	rb_BinaryRBM *r;
	Data_Get_Struct(rb_rbm, rb_BinaryRBM, r);

	if (rb_beta != Qnil) {
		if (TYPE(rb_beta) != T_FLOAT && TYPE(rb_beta) != T_FIXNUM)
			rb_raise(rb_eArgError, "Log partition Function's beta value must be a Float or a Fixnum.");
		return rb_float_new(logPartitionFunction(r->model, NUM2DBL(rb_beta)));
	} else {
		return rb_float_new(logPartitionFunction(r->model));
	}
};
VALUE method_rbm_analytics_negative_log_likelihood_from_log_partition (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_rbm, rb_data, rb_logPartition, rb_beta;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"31",
		&rb_rbm,
		&rb_data,
		&rb_logPartition, 
		&rb_beta);

	Check_Type(rb_rbm, T_DATA);
	if (CLASS_OF(rb_rbm) != rb_BinaryRBM::rb_class())
		rb_raise(rb_eArgError, "Negative Log Likelihood from Log Partition requires an RBM");

	Check_Type(rb_data, T_DATA);
	if (CLASS_OF(rb_data) != rb_UnlabeledData::rb_class())
		rb_raise(rb_eArgError, "Negative Log Likelihood from Log Partition requires UnlabeledData.");

	if (TYPE(rb_logPartition) != T_FLOAT && TYPE(rb_logPartition) != T_FIXNUM)
		rb_raise(rb_eArgError, "Negative Log Likelihood from Log Partition's LogPartition value (3rd argument) must be a Float or a Fixnum.");

	rb_BinaryRBM *r;
	Data_Get_Struct(rb_rbm, rb_BinaryRBM, r);
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_data, rb_UnlabeledData, d);

	if (rb_beta != Qnil) {
		if (TYPE(rb_beta) != T_FLOAT && TYPE(rb_beta) != T_FIXNUM)
			rb_raise(rb_eArgError, "Negative Log Likelihood from Log Partition's beta (4th optional argument) value must be a Float or a Fixnum.");
		return rb_float_new(negativeLogLikelihoodFromLogPartition(r->model, d->data, NUM2DBL(rb_logPartition), NUM2DBL(rb_beta)));
	} else {
		return rb_float_new(negativeLogLikelihoodFromLogPartition(r->model, d->data, NUM2DBL(rb_logPartition)));
	}
};
VALUE method_rbm_analytics_negative_log_likelihood (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_rbm, rb_data, rb_beta;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"21",
		&rb_rbm,
		&rb_data, 
		&rb_beta);

	Check_Type(rb_rbm, T_DATA);
	if (CLASS_OF(rb_rbm) != rb_BinaryRBM::rb_class())
		rb_raise(rb_eArgError, "Negative Log Likelihood requires an RBM");

	Check_Type(rb_data, T_DATA);
	if (CLASS_OF(rb_data) != rb_UnlabeledData::rb_class())
		rb_raise(rb_eArgError, "Negative Log Likelihood requires UnlabeledData.");

	rb_BinaryRBM *r;
	Data_Get_Struct(rb_rbm, rb_BinaryRBM, r);
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_data, rb_UnlabeledData, d);

	if (rb_beta != Qnil) {
		if (TYPE(rb_beta) != T_FLOAT && TYPE(rb_beta) != T_FIXNUM)
			rb_raise(rb_eArgError, "Negative Log Likelihood's beta (3rd optional argument) value must be a Float or a Fixnum.");
		return rb_float_new(negativeLogLikelihood(r->model, d->data, NUM2DBL(rb_beta)));
	} else {
		return rb_float_new(negativeLogLikelihood(r->model, d->data));
	}
};

void Init_RBM_Analytics () {
	rb_define_singleton_method(rb_optimizer_rbm_analytics_module, "negative_log_likelihood", (rb_method) method_rbm_analytics_negative_log_likelihood, -1);
	rb_define_singleton_method(rb_optimizer_rbm_analytics_module, "log_partition_function", (rb_method) method_rbm_analytics_log_partition_function, -1);
	rb_define_singleton_method(rb_optimizer_rbm_analytics_module, "negative_log_likelihood_from_log_partition", (rb_method) method_rbm_analytics_negative_log_likelihood_from_log_partition, -1);
}