#ifndef RB_BINARYRBM_H
#define RB_BINARYRBM_H

#include <shark/Unsupervised/RBM/BinaryRBM.h>
class rb_BinaryRBM {

	public:
		BinaryRBM rbm;
		rb_BinaryRBM(Rng::rng_type&);

};

void Init_BinaryRBM ();

#endif