#include "rb_BinaryRBM.h"

extern VALUE rb_optimizer_binaryrbm_klass;
extern VALUE rb_optimizer_realvector_klass;
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


VALUE method_binaryrbm_allocate (VALUE klass) {
	return wrap_pointer<rb_BinaryRBM>(
			rb_optimizer_binaryrbm_klass,
			new rb_BinaryRBM()
		);
}

VALUE method_binaryrbm_initialize (VALUE self) {
	return self;
}

rb_BinaryRBM::rb_BinaryRBM (): rbm(Rng::globalRng) {};

VALUE method_binaryrbm_get_number_of_parameters (VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return INT2FIX(r->rbm.numberOfParameters());
}

VALUE method_binaryrbm_get_parameter_vector (VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(r->rbm.parameterVector())
	);
}

VALUE method_binaryrbm_set_parameter_vector (VALUE self, VALUE rb_parameter) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	Check_Type(rb_parameter, T_DATA);
	if (CLASS_OF(rb_parameter) != rb_optimizer_realvector_klass)
		rb_raise(rb_eArgError, "BinaryRBM's parameter vector can only be set using a RealVector.");

	rb_RealVector *vec;
	Data_Get_Struct(rb_parameter, rb_RealVector, vec);

	r->rbm.setParameterVector(vec->data);
	return self;
}

VALUE method_binaryrbm_set_structure (VALUE self, VALUE rb_visible, VALUE rb_hidden) {

	Check_Type(rb_visible, T_FIXNUM);
	Check_Type(rb_hidden, T_FIXNUM);

	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	r->rbm.setStructure(NUM2INT(rb_visible), NUM2INT(rb_hidden));
	return self;
}

/*VALUE method_binaryrbm_hidden_neurons (VALUE self) {
	// ??
	return self;
}

VALUE method_binaryrbm_visible_neurons (VALUE self) {
	// ??
	return self;
}

VALUE method_binaryrbm_weight_matrix (VALUE self) {
	// ??
	return self;
}*/

/*VALUE method_binaryrbm_energy (VALUE self) {
	// ??
	return self;
}*/

/*VALUE method_binaryrbm_random_number_generator (VALUE self) {
	// ??
	return self;
}*/

VALUE method_binaryrbm_evaluation_type (VALUE self, VALUE rb_forward, VALUE rb_eval_mean) {
	if ((rb_forward != Qtrue || rb_forward != Qfalse) && (rb_eval_mean != Qtrue || rb_eval_mean != Qfalse))
		rb_raise(rb_eArgError, "Evaluation type takes 2 boolean options:\n    1. @param forward whether the forward view should be used false=backwards\n    2. @param evalMean whether the mean state should be returned. false=a sample is\n      returned.");
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	r->rbm.evaluationType(rb_forward == Qtrue, rb_eval_mean == Qtrue);
	return self;
}
/*VALUE method_binaryrbm_eval(VALUE self, VALUE rb_data) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);

	if (TYPE(rb_data) == T_ARRAY) {

		if (RARRAY_LEN(rb_data) > 0) {

			// Will work with 1 and 2D arrays:

			return wrap_pointer<rb_UnlabeledData>(
				rb_optimizer_unlabeleddata_klass,
				new rb_UnlabeledData(r->rbm.eval(rb_ary_to_unlabeleddata(rb_data)))
			);
		} else {
			rb_raise(rb_eArgError, "Can only evaluate non-empty data.");
		}
	} else {
		Check_Type(rb_data, T_DATA);

		if (CLASS_OF(rb_data) == rb_optimizer_unlabeleddata_klass) {
			rb_UnlabeledData *d;
			Data_Get_Struct(rb_data, rb_UnlabeledData, d);

			return wrap_pointer<rb_UnlabeledData>(
				rb_optimizer_unlabeleddata_klass,
				new rb_UnlabeledData(r->rbm.eval(d->data))
			);
		} else if (CLASS_OF(rb_data) == rb_optimizer_realvector_klass) {
			rb_RealVector *d;
			Data_Get_Struct(rb_data, rb_RealVector, d);

			std::vector<RealVector> vectors = realvector_to_stdvector(d->data);
			shark::UnlabeledData<shark::RealVector> samples = shark::createDataFromRange(vectors);

			// could also return a realvector with just the results... but why break the expected format?
			return wrap_pointer<rb_UnlabeledData>(
				rb_optimizer_unlabeleddata_klass,
				new rb_UnlabeledData(r->rbm.eval(samples))
			);
		} else {
			rb_raise(rb_eArgError, "BinaryRBM can evalute UnlabeledData, RealVectors, or Arrays.");
		}
		return self;
	}
	return self;
}*/
VALUE method_binaryrbm_number_of_hidden_neurons(VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return INT2FIX(r->rbm.numberOfHN());
}
VALUE method_binaryrbm_number_of_visible_neurons(VALUE self) {
	rb_BinaryRBM *r;
	Data_Get_Struct(self, rb_BinaryRBM, r);
	return INT2FIX(r->rbm.numberOfVN());
}

typedef VALUE (*rb_method)(...);

void initializeWeights(BinaryRBM& rbm){
	RealVector weights(rbm.numberOfParameters());
	for(size_t i = 0; i != weights.size(); ++i){
		weights(i) = Rng::uni(-0.1,0.1);
	}
	rbm.setParameterVector(weights);
}

void Test_RBM () {

	//we first create the problem. in this tutorial, we use BarsAndStripes
	BarsAndStripes problem;
	UnlabeledData<RealVector> data = problem.data();
	
	//some constants needed for training
	size_t numberOfHidden = 32;//hidden units of the rbm
	size_t numberOfVisible = problem.inputDimension();//visible units of the inputs

	//create rbm with simple binary units
	rb_BinaryRBM *rb_rbm = new rb_BinaryRBM();
	//BinaryRBM rbm(Rng::globalRng);
	cout << "Called rbm once" << endl;

	rb_rbm->rbm.setStructure(numberOfVisible,numberOfHidden);
	
	//create derivative to optimize the rbm
	//we want a simple vanilla CD-1
	rb_BinaryCD *rb_cd = new rb_BinaryCD(rb_rbm->rbm);

	cout << "Got past BinaryCD" << endl;
	//BinaryCD cd = rb_cd->objective();
	//BinaryCD cd(&rbm);
	rb_cd->objective().setK(1);
	rb_cd->objective().setData(data);
    
    
	//generate optimizer
	rb_SteepestDescent *rb_optimizer = new rb_SteepestDescent();
    
    ///*
	//SteepestDescent optimizer = rb_optimizer->algorithm();
	rb_optimizer->algorithm().setMomentum(0);
	rb_optimizer->algorithm().setLearningRate(0.1);
	
	//now we train the rbm and evaluate the mean negative log-likelihood at the end
	unsigned int numIterations = 1000;//iterations for training
	unsigned int numTrials = 10;//number of trials for training
	double meanResult = 0;

	// for testing purposes
	initializeWeights(rb_rbm->rbm);
	rb_optimizer->algorithm().init(rb_cd->objective());

	cout << "optimizer.solution().point = " << rb_optimizer->algorithm().solution().point << endl;
	cout << "optimizer.solution().value = " << rb_optimizer->algorithm().solution().value << endl;
	initializeWeights(rb_rbm->rbm);
	rb_optimizer->algorithm().step(rb_cd->objective());
    // */

    /*
	SteepestDescent optimizer = rb_optimizer->algorithm();
	optimizer.setMomentum(0);
	optimizer.setLearningRate(0.1);
	
	//now we train the rbm and evaluate the mean negative log-likelihood at the end
	unsigned int numIterations = 1000;//iterations for training
	unsigned int numTrials = 10;//number of trials for training
	double meanResult = 0;

	// for testing purposes
	initializeWeights(rb_rbm->rbm);
	optimizer.init(rb_cd->objective());

	cout << "optimizer.solution().point = " << optimizer.solution().point << endl;
	cout << "optimizer.solution().value = " << optimizer.solution().value << endl;
	initializeWeights(rb_rbm->rbm);
	optimizer.step(rb_cd->objective());
    */



	/*for(unsigned int trial = 0; trial != numTrials; ++trial) {
		initializeWeights(rb_rbm->rbm);
		optimizer.init(cd);

		for(unsigned int iteration = 0; iteration != numIterations; ++iteration) {
			optimizer.step(cd);
		}
		//evaluate exact likelihood after training. this is only possible for small problems!
		double likelihood = negativeLogLikelihood(rb_rbm->rbm,data);
		std::cout<<trial<<" "<<likelihood<<std::endl;
		meanResult +=likelihood;
	}
	meanResult /= numTrials;

	//print the mean performance
	cout << "RESULTS: " << std::endl;
	cout << "======== " << std::endl;
	cout << "mean negative log likelihood: " << meanResult << std::endl;*/

}

void Init_BinaryRBM () {

	rb_define_alloc_func(rb_optimizer_binaryrbm_klass, (rb_alloc_func_t) method_binaryrbm_allocate);
	rb_define_method(rb_optimizer_binaryrbm_klass, "set_structure", (rb_method) method_binaryrbm_set_structure, 2);
	rb_define_method(rb_optimizer_binaryrbm_klass, "evaluation_type", (rb_method) method_binaryrbm_evaluation_type, 2);
	rb_define_method(rb_optimizer_binaryrbm_klass, "initialize", (rb_method) method_binaryrbm_initialize, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "parameter_vector=", (rb_method) method_binaryrbm_set_parameter_vector, 1);
	rb_define_method(rb_optimizer_binaryrbm_klass, "parameter_vector", (rb_method) method_binaryrbm_get_parameter_vector, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "number_of_parameters", (rb_method) method_binaryrbm_get_number_of_parameters, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "number_of_hidden_neurons", (rb_method) method_binaryrbm_number_of_hidden_neurons, 0);
	rb_define_method(rb_optimizer_binaryrbm_klass, "number_of_visible_neurons", (rb_method) method_binaryrbm_number_of_visible_neurons, 0);

}

