#ifndef RB_LINEARMODEL_H
#define RB_LINEARMODEL_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <shark/Models/LinearModel.h>
#include "rb_UnlabeledData.h"
#include "rb_RealVector.h"
#include "rb_RealMatrix.h"


using namespace std;
using namespace shark;
class rb_LinearModel {

	public:
		RealVector& offset();
		RealMatrix& matrix();
		void setStructure(RealMatrix const&, RealVector const&);
		void setStructure(int, int, bool);
		int numberOfParameters();
		void setParameterVector(RealVector const&);
		RealVector parameterVector();
		int outputSize();
		int inputSize();
		bool hasOffset();
		Data<RealVector> operator()(Data<RealVector> const&);
		RealVector operator()(RealVector const&);
		Data<RealVector> eval(Data<RealVector> const&);
		RealVector eval(RealVector const&);
		rb_LinearModel();
		rb_LinearModel(LinearModel<RealVector>);
		rb_LinearModel(RealMatrix const&);
		rb_LinearModel(RealMatrix const&, RealVector const&);
		// should add eval as well.
		LinearModel<> model;
};

void Init_LinearModel();

#endif