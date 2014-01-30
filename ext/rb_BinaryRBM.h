#include <shark/Unsupervised/RBM/BinaryRBM.h>
class rb_BinaryRBM {

	public: {
		RBM<BinaryLayer,BinaryLayer, Rng::rng_type> rbm;
		rb_BinaryRBM();
		~rb_BinaryRBM() {
			delete rbm;
		}
	}

}