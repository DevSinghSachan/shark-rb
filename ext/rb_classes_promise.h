#ifndef RB_CLASSES_PROMISE_H
#define RB_CLASSES_PROMISE_H

#include "ruby.h"

extern VALUE rb_array_klass                         ;

// Big Wrapper for everything (not good):
extern VALUE rb_optimizer_klass                     ;

// Datatypes:
extern VALUE rb_optimizer_realvector_klass          ;
extern VALUE rb_optimizer_realvector_reference_klass;
extern VALUE rb_optimizer_unlabeleddata_klass       ;
extern VALUE rb_optimizer_regressionset_klass       ;
extern VALUE rb_optimizer_realmatrix_klass          ;
extern VALUE rb_optimizer_realmatrix_row_klass      ;
extern VALUE rb_optimizer_realmatrix_column_klass   ;
extern VALUE rb_optimizer_realmatrix_reference_klass;
extern VALUE rb_optimizer_solutionset_klass         ;

// LinearModel
extern VALUE rb_optimizer_linearmodel_klass         ;

// PCA algorithm:
extern VALUE rb_optimizer_pca_klass                 ;



extern VALUE rb_algorithms_module                   ;

// Algorithms:
extern VALUE rb_optimizer_steepestdescent_klass     ;
extern VALUE rb_optimizer_rprop_klass               ;
extern VALUE rb_optimizer_bfgs_klass                ;
extern VALUE rb_optimizer_lbfgs_klass               ;


extern VALUE rb_problems_module                     ;

// Sample problems:
extern VALUE rb_optimizer_barsandstripes_klass      ;


// RBM Module:
extern VALUE rb_optimizer_rbm_module                ;

// RBM Models:
extern VALUE rb_optimizer_binaryrbm_klass           ;
extern VALUE rb_optimizer_rbm_binarylayer_klass     ;
extern VALUE rb_optimizer_gaussbinaryrbm_klass      ;
// RBM submodules: (analytics, energy, etc...)
extern VALUE rb_optimizer_rbm_analytics_module      ;

// Objective Functions:

// Abstract Objective function
extern VALUE rb_optimizer_objective_function_klass  ;
extern VALUE rb_optimizer_binarycd_klass            ;
extern VALUE rb_optimizer_exactgradient_klass       ;


#endif