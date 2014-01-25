#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <shark/Models/LinearModel.h>
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
		
		rb_LinearModel();
		rb_LinearModel(LinearModel<RealVector>);
		rb_LinearModel(RealMatrix const&);
		rb_LinearModel(RealMatrix const&, RealVector const&);
		// should add eval as well.
		LinearModel<RealVector> model;
};