#ifndef RB_PCA_H
#define RB_PCA_H

#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <shark/Algorithms/Trainers/PCA.h>
#include "rb_conversions.h"
#include "rb_RealMatrix.h"
#include "rb_RealVector.h"
#include "rb_LinearModel.h"
#include "rb_UnlabeledData.h"


using namespace shark;
using namespace std;
class rb_PCA {

	public:
		static VALUE rb_class();
		void train(LinearModel<RealVector>&, UnlabeledData<RealVector> const&);
		void setWhitening(bool);
		bool whitening();
		PCA trainer;
		bool m_whitening;
		void setData(UnlabeledData<RealVector> const&);
		rb_PCA();
		rb_PCA(bool);
		
		rb_PCA(UnlabeledData<RealVector> const&, bool);
		void encoder(LinearModel<RealVector> &, size_t);
		void decoder(LinearModel<RealVector> &, size_t);
		RealVector const& eigenvalues();
		double eigenvalue(size_t);
		RealMatrix const& eigenvectors();
		RealVector const& mean();
};

void Init_PCA();

#endif