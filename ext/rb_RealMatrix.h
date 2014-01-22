#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
using namespace std;
using namespace shark;
class rb_RealMatrix {
	public:
		rb_RealMatrix(RealMatrix);
		rb_RealMatrix();
		RealMatrix data;
};