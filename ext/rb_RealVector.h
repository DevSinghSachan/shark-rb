#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
using namespace std;
using namespace shark;
class rb_RealVector {
	public:
		rb_RealVector(RealVector);
		RealVector data;
};