#ifndef RB_RPROP_H
#define RB_RPROP_H

#include <shark/Algorithms/GradientDescent/Rprop.h> //resilient propagation as optimizer

class rb_Rprop {

	public:
		static VALUE rb_class();
		RpropMinus algorithm;
		rb_Rprop();

};

#endif