#include <boost/any.hpp>
#include <boost/format.hpp>
using namespace std;
using namespace shark;
class rb_UnlabeledData {
	public:
		rb_UnlabeledData(UnlabeledData<RealVector>);
		UnlabeledData<RealVector> data;
		vector<RealVector> input ();
};