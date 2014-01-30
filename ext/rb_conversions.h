#include "ruby.h"
#include <boost/any.hpp>
#include <boost/format.hpp>
#include <shark/Data/Dataset.h>
using namespace std;
using namespace shark;

template<class Obtype> void delete_objects(Obtype *);
template<class Obtype> VALUE wrap_pointer(VALUE, Obtype *);

template<class Obtype> VALUE alloc_ob(VALUE);
shark::RealVector rb_ary_to_1d_realvector(VALUE);
RealMatrix rb_ary_to_realmatrix(VALUE);
RealMatrix rb_1d_ary_to_realmatrix(VALUE);
std::vector<shark::RealMatrix> rb_ary_to_realmatrices(VALUE);
VALUE realmatrix_to_rb_ary(const RealMatrix&);
VALUE realmatrix_to_rb_ary(const RealMatrix&, bool);
VALUE stdvector_realmatrix_to_rb_ary(const std::vector<RealMatrix>);
std::vector<shark::RealVector> rb_ary_to_realvector(VALUE);
shark::UnlabeledData<shark::RealVector> rb_ary_to_unlabeleddata(VALUE);
VALUE realvector_to_rb_ary(const RealVector&);