#include "rb_class_errors.h"
void rb_check_class(VALUE x, VALUE t) {
	VALUE found_class = CLASS_OF(x);
    if (x != t)
    	rb_raise(rb_eTypeError, "Wrong argument class %s (expected %s)", builtin_class_name(x), builtin_class_name(t));
}