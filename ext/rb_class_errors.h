#ifndef RB_CLASS_ERRORS_H
#define RB_CLASS_ERRORS_H

#include "ruby.h"

void rb_check_class(VALUE,VALUE);
#define Check_Class(v,t) rb_check_type((VALUE)(v),(VALUE)(t))

#endif