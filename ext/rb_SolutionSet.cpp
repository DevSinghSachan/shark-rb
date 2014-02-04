#include "rb_SolutionSet"

extern VALUE rb_optimizer_solutionset_klass;
extern VALUE rb_optimizer_realvector_klass;

template<class Obtype> void delete_objects(Obtype *ptr){
	delete ptr;
}

template<class Obtype> VALUE wrap_pointer(VALUE klass, Obtype *ptr){
	return Data_Wrap_Struct(klass,0,delete_objects<Obtype>,ptr);
}

template<class Obtype> VALUE alloc_ob(VALUE self) {
	return wrap_pointer<Obtype>(self,new Obtype());
}

rb_SolutionSet::rb_SolutionSet (const ResultSet& _result) : result(_result) {};
rb_SolutionSet::rb_SolutionSet () {};

VALUE method_solutionset_point (VALUE self) {
	rb_SolutionSet *s;
	Data_Get_Struct(self, rb_SolutionSet, s);

	return wrap_pointer<rb_RealVector>(
		rb_optimizer_realvector_klass,
		new rb_RealVector(s->result.point)
		);
}

VALUE method_solutionset_allocate (VALUE klass) {
	return wrap_pointer<rb_SolutionSet>(
		rb_optimizer_solutionset_klass,
		new rb_SolutionSet()
		);
}
VALUE method_solutionset_initialize (VALUE self) {
	return self;
}

VALUE method_solutionset_value (VALUE self) {
	rb_SolutionSet *s;
	Data_Get_Struct(self, rb_SolutionSet, s);

	return rb_float_new(s->result.value);
}

typedef VALUE (*rb_method)(...);

void Init_SolutionSet () {
	rb_define_alloc_func(rb_optimizer_solutionset_klass,  (rb_alloc_func_t) method_solutionset_allocate);
	rb_define_method(rb_optimizer_solutionset_klass, "initialize", (rb_method) method_solutionset_initialize, 0);
	rb_define_method(rb_optimizer_solutionset_klass, "point", method_solutionset_point, 0);
	rb_define_method(rb_optimizer_solutionset_klass, "value", method_solutionset_value, 0);
}