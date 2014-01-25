#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
#include <shark/Algorithms/Trainers/PCA.h>

using namespace shark;
using namespace std;
class rb_PCA {

	public:
		void train(LinearModel<RealVector>&, UnlabeledData<RealVector> const&);
		void setWhitening(bool);
		bool whitening();
		PCA trainer;
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
}