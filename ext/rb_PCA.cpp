#include "rb_PCA.h"

using namespace shark;
using namespace std;

extern VALUE rb_optimizer_pca_klass;

#include "extras/utils/rb_pointer_wrapping.extras"

void rb_PCA::train(LinearModel<RealVector> &model, UnlabeledData<RealVector> const& inputs) {
	trainer.train(model, inputs);
}
void rb_PCA::setWhitening(bool whitening) {
	m_whitening = whitening;
	trainer.setWhitening(whitening);
};
bool rb_PCA::whitening() {
	return m_whitening;
};

void rb_PCA::setData(UnlabeledData<RealVector> const & inputs) {
	trainer.setData(inputs);
}
rb_PCA::rb_PCA(): m_whitening(false) {
}
rb_PCA::rb_PCA(bool whitening): m_whitening(whitening) {
	trainer = new PCA(whitening);
}
rb_PCA::rb_PCA(UnlabeledData<RealVector> const &inputs, bool whitening): m_whitening(whitening) {
	trainer = new PCA(inputs, whitening);
}
void rb_PCA::encoder(LinearModel<> & model, size_t m) {
	trainer.encoder(model, m);
}
void rb_PCA::decoder(LinearModel<> & model, size_t m) {
	trainer.decoder(model, m);
}
RealVector const& rb_PCA::eigenvalues() {
	return trainer.eigenvalues();
};
double rb_PCA::eigenvalue(size_t i) {
	return trainer.eigenvalue(i);
};
RealMatrix const& rb_PCA::eigenvectors() {
	return trainer.eigenvectors();
};
RealVector const& rb_PCA::mean() {
	return trainer.mean();
};


VALUE method_pca_allocate (VALUE klass) {
return wrap_pointer<rb_PCA>(
			rb_PCA::rb_class(),
			new rb_PCA()
		);
};

VALUE method_pca_initialize (int number_of_arguments, VALUE* ruby_arguments, VALUE self) {
	VALUE rb_data,
			rb_whitening;

	rb_scan_args(
		number_of_arguments,
		ruby_arguments,
		"02",
		&rb_data,
		&rb_whitening);

	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	if (TYPE(rb_data) == T_DATA) {

		if (CLASS_OF(rb_data) != rb_UnlabeledData::rb_class())
			rb_raise(rb_eArgError, "PCA takes either:\n - UnlabeledData, and a boolean choice whether to do whitening, OR\n - a boolean whether to do whitening, OR\n - no parameters.");

		bool whitening = true;
		rb_UnlabeledData *d;
		Data_Get_Struct(rb_data, rb_UnlabeledData, d);

		if (rb_whitening != Qnil) {
			if (rb_whitening == Qtrue) {
				whitening = true;
			} else {
				whitening = false;
			}
		}
		p->m_whitening = whitening;
		p->trainer.setWhitening(whitening);
		p->trainer.setData(d->data);

	} else if (TYPE(rb_data) != Qnil) {
		if (rb_data == Qtrue) {
			p->setWhitening(true);
			p->m_whitening = true;
		} else {
			p->setWhitening(false);
			p->m_whitening = false;
		}
	} // else No settings made to structure.. doing that later I guess.

	return self;
};


VALUE method_pca_train (VALUE self, VALUE rb_model, VALUE rb_inputs) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	Check_Type(rb_model, T_DATA);
	Check_Type(rb_inputs, T_DATA);
	if (CLASS_OF(rb_model) != rb_LinearModel::rb_class() || CLASS_OF(rb_inputs) != rb_UnlabeledData::rb_class())
		rb_raise(rb_eArgError, "PCA is trained using a LinearModel and UnlabeledData.");

	rb_LinearModel *m;
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_model, rb_LinearModel, m);
	Data_Get_Struct(rb_inputs, rb_UnlabeledData, d);

	p->train(m->model, d->data);

	return self;
};

VALUE method_pca_setWhitening (VALUE self, VALUE whitening) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	if (whitening == Qtrue) {
		p->m_whitening = true;
		p->setWhitening(true);
	} else {
		p->m_whitening = false;
		p->setWhitening(false);
	}
	return self;
};

VALUE method_pca_get_whitening (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	return (p->whitening() ? Qtrue : Qfalse);
};

VALUE method_pca_setData (VALUE self, VALUE rb_data) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);
	Check_Type(rb_data, T_DATA);
	if (CLASS_OF(rb_data) != rb_UnlabeledData::rb_class())
		rb_raise(rb_eArgError, "PCA's data is set using UnlabeledData.");
	rb_UnlabeledData *d;
	Data_Get_Struct(rb_data, rb_UnlabeledData, d);

	p->setData(d->data);
	return self;
};

VALUE method_pca_encoder (VALUE self, VALUE rb_model, VALUE numDimensions) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(numDimensions, T_FIXNUM);
	Check_Type(rb_model, T_DATA);
	if (CLASS_OF(rb_model) != rb_LinearModel::rb_class())
		rb_raise(rb_eArgError, "PCA's encoder must be used with a LinearModel");

	rb_LinearModel *m;
	Data_Get_Struct(rb_model, rb_LinearModel, m);

	p->encoder(m->model, NUM2INT(numDimensions));

	return self;
};

VALUE method_pca_decoder (VALUE self, VALUE rb_model, VALUE numDimensions) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(numDimensions, T_FIXNUM);
	Check_Type(rb_model, T_DATA);
	if (CLASS_OF(rb_model) != rb_LinearModel::rb_class())
		rb_raise(rb_eArgError, "PCA's decoder must be used with a LinearModel");

	rb_LinearModel *m;
	Data_Get_Struct(rb_model, rb_LinearModel, m);

	p->decoder(m->model, NUM2INT(numDimensions));

	return self;
};

VALUE method_pca_eigenvalues (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealVector>(
		rb_RealVector::rb_class(),
		new rb_RealVector(p->eigenvalues())
	);
};

VALUE method_pca_eigenvalue (VALUE self, VALUE position) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	Check_Type(position, T_FIXNUM);

	return rb_float_new(p->eigenvalue(NUM2INT(position)));
};

VALUE method_pca_eigenvectors (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealMatrix>(
		rb_RealMatrix::rb_class(),
		new rb_RealMatrix(p->eigenvectors())
	);
};

VALUE method_pca_mean (VALUE self) {
	rb_PCA *p;
	Data_Get_Struct(self, rb_PCA, p);

	return wrap_pointer<rb_RealVector>(
		rb_RealVector::rb_class(),
		new rb_RealVector(p->mean())
	);
};

void Init_PCA () {

	// Shark PCA class:
	rb_define_alloc_func(rb_optimizer_pca_klass,  (rb_alloc_func_t) method_pca_allocate);
	rb_define_method(rb_optimizer_pca_klass, "initialize", (rb_method)method_pca_initialize, -1);
	rb_define_method(rb_optimizer_pca_klass, "train",(rb_method) method_pca_train, 2);
	rb_define_method(rb_optimizer_pca_klass, "whitening=",(rb_method) method_pca_setWhitening, 1);
	rb_define_method(rb_optimizer_pca_klass, "whitening",(rb_method) method_pca_get_whitening, 0);
	rb_define_method(rb_optimizer_pca_klass, "set_data",(rb_method) method_pca_setData, 1);
	rb_define_method(rb_optimizer_pca_klass, "encoder",(rb_method) method_pca_encoder, 2);
	rb_define_method(rb_optimizer_pca_klass, "decoder",(rb_method) method_pca_decoder, 2);
	rb_define_method(rb_optimizer_pca_klass, "eigenvalues",(rb_method) method_pca_eigenvalues, 0);
	rb_define_method(rb_optimizer_pca_klass, "eigenvalue",(rb_method) method_pca_eigenvalue, 1);
	rb_define_method(rb_optimizer_pca_klass, "eigenvectors",(rb_method) method_pca_eigenvectors, 0);
	rb_define_method(rb_optimizer_pca_klass, "mean",(rb_method) method_pca_mean, 0);

};