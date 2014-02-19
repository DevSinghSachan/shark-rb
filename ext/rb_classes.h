#ifndef RB_CLASSES_H
#define RB_CLASSES_H

#include "ruby.h"

VALUE rb_array_klass                   = rb_define_class("Array", rb_cObject);

// Big Wrapper for everything (not good):
VALUE rb_optimizer_klass               = rb_define_class("Optimizer", rb_cObject);

// Datatypes:
VALUE rb_optimizer_realvector_klass    = rb_define_class_under(rb_optimizer_klass, "RealVector", rb_cObject);
VALUE rb_optimizer_realvector_reference_klass = rb_define_class_under(rb_optimizer_klass, "RealVectorReference", rb_cObject);
VALUE rb_optimizer_unlabeleddata_klass = rb_define_class_under(rb_optimizer_klass, "UnlabeledData", rb_cObject);
VALUE rb_optimizer_regressionset_klass = rb_define_class_under(rb_optimizer_klass, "RegressionDataset", rb_cObject);
VALUE rb_optimizer_realmatrix_klass    = rb_define_class_under(rb_optimizer_klass, "RealMatrix", rb_cObject);
VALUE rb_optimizer_realmatrix_reference_klass = rb_define_class_under(rb_optimizer_klass, "RealMatrixReference", rb_cObject);
VALUE rb_optimizer_solutionset_klass   = rb_define_class_under(rb_optimizer_klass, "SolutionSet", rb_cObject);

// LinearModel
VALUE rb_optimizer_linearmodel_klass   = rb_define_class_under(rb_optimizer_klass, "LinearModel", rb_cObject);

// PCA algorithm:
VALUE rb_optimizer_pca_klass           = rb_define_class_under(rb_optimizer_klass, "PCA", rb_cObject);



VALUE rb_algorithms_module               = rb_define_module_under(rb_optimizer_klass, "Algorithms");

// Algorithms:
VALUE rb_optimizer_steepestdescent_klass = rb_define_class_under(rb_algorithms_module, "SteepestDescent", rb_cObject);
VALUE rb_optimizer_rprop_klass           = rb_define_class_under(rb_algorithms_module, "RProp", rb_cObject);
VALUE rb_optimizer_bfgs_klass            = rb_define_class_under(rb_algorithms_module, "BFGS", rb_cObject);
VALUE rb_optimizer_lbfgs_klass           = rb_define_class_under(rb_algorithms_module, "LBFGS", rb_cObject);


VALUE rb_problems_module                 = rb_define_module_under(rb_optimizer_klass, "Problems");

// Sample problems:
VALUE rb_optimizer_barsandstripes_klass = rb_define_class_under(rb_problems_module, "BarsAndStripes", rb_cObject);


// RBM Module:
VALUE rb_optimizer_rbm_module = rb_define_module_under(rb_optimizer_klass, "RBM");

// RBM Models:
VALUE rb_optimizer_binaryrbm_klass     = rb_define_class_under(rb_optimizer_rbm_module, "BinaryRBM", rb_cObject);
VALUE rb_optimizer_gaussbinaryrbm_klass= rb_define_class_under(rb_optimizer_rbm_module, "GaussianBinaryRBM", rb_cObject);
// RBM submodules: (analytics, energy, etc...)
VALUE rb_optimizer_rbm_analytics_module = rb_define_module_under(rb_optimizer_rbm_module, "Analytics");

// Objective Functions:

// Abstract Objective function
VALUE rb_optimizer_objective_function_klass = rb_define_class_under(rb_optimizer_klass, "ObjectiveFunction", rb_cObject);
VALUE rb_optimizer_binarycd_klass           = rb_define_class_under(rb_optimizer_klass, "BinaryCD", rb_optimizer_objective_function_klass);
VALUE rb_optimizer_exactgradient_klass      = rb_define_class_under(rb_optimizer_klass, "ExactGradient", rb_optimizer_objective_function_klass);


#endif