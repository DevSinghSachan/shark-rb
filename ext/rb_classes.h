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
VALUE rb_optimizer_realmatrix_row_klass = rb_define_class_under(rb_optimizer_klass, "RealMatrixRow", rb_cObject);
VALUE rb_optimizer_realmatrix_column_klass = rb_define_class_under(rb_optimizer_klass, "RealMatrixColumn", rb_cObject);
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
VALUE rb_optimizer_rbm_binarylayer_klass = rb_define_class_under(rb_optimizer_rbm_module, "BinaryLayer", rb_cObject);
VALUE rb_optimizer_rbm_gaussianlayer_klass = rb_define_class_under(rb_optimizer_rbm_module, "GaussianLayer", rb_cObject);
VALUE rb_optimizer_gaussianbinaryrbm_klass= rb_define_class_under(rb_optimizer_rbm_module, "GaussianBinaryRBM", rb_cObject);
// RBM submodules: (analytics, energy, etc...)
VALUE rb_optimizer_rbm_analytics_module = rb_define_module_under(rb_optimizer_rbm_module, "Analytics");

// Objective Functions:

// Abstract Objective function
VALUE rb_optimizer_objective_function_klass = rb_define_class_under(rb_optimizer_klass, "ObjectiveFunction", rb_cObject);
VALUE rb_optimizer_binarycd_klass           = rb_define_class_under(rb_optimizer_klass, "BinaryCD", rb_optimizer_objective_function_klass);
VALUE rb_optimizer_exactgradient_klass      = rb_define_class_under(rb_optimizer_klass, "ExactGradient", rb_optimizer_objective_function_klass);


// RNG
VALUE rb_optimizer_rng_klass = rb_define_class_under(rb_optimizer_klass, "RNG", rb_cObject);

VALUE rb_optimizer_rng_weibull = rb_define_class_under(rb_optimizer_rng_klass, "Weibull", rb_cObject);
VALUE rb_optimizer_rng_bernoulli = rb_define_class_under(rb_optimizer_rng_klass, "Bernoulli", rb_cObject);
VALUE rb_optimizer_rng_binomial = rb_define_class_under(rb_optimizer_rng_klass, "Binomial", rb_cObject);
VALUE rb_optimizer_rng_cauchy = rb_define_class_under(rb_optimizer_rng_klass, "Cauchy", rb_cObject);
VALUE rb_optimizer_rng_diffgeometric = rb_define_class_under(rb_optimizer_rng_klass, "DiffGeometric", rb_cObject);
VALUE rb_optimizer_rng_dirichlet = rb_define_class_under(rb_optimizer_rng_klass, "Dirichlet", rb_cObject);
VALUE rb_optimizer_rng_discreteuniform = rb_define_class_under(rb_optimizer_rng_klass, "DiscreteUniform", rb_cObject);
VALUE rb_optimizer_rng_erlang = rb_define_class_under(rb_optimizer_rng_klass, "Erlang", rb_cObject);
VALUE rb_optimizer_rng_gamma = rb_define_class_under(rb_optimizer_rng_klass, "Gamma", rb_cObject);
VALUE rb_optimizer_rng_geometric = rb_define_class_under(rb_optimizer_rng_klass, "Geometric", rb_cObject);
VALUE rb_optimizer_rng_globalrng = rb_define_class_under(rb_optimizer_rng_klass, "GlobalRng", rb_cObject);
VALUE rb_optimizer_rng_hypergeometric = rb_define_class_under(rb_optimizer_rng_klass, "HyperGeometric", rb_cObject);
VALUE rb_optimizer_rng_lognormal = rb_define_class_under(rb_optimizer_rng_klass, "LogNormal", rb_cObject);
VALUE rb_optimizer_rng_negexponential = rb_define_class_under(rb_optimizer_rng_klass, "NegExponential", rb_cObject);
VALUE rb_optimizer_rng_normal = rb_define_class_under(rb_optimizer_rng_klass, "Normal", rb_cObject);
VALUE rb_optimizer_rng_poisson = rb_define_class_under(rb_optimizer_rng_klass, "Poisson", rb_cObject);
VALUE rb_optimizer_rng_uniform = rb_define_class_under(rb_optimizer_rng_klass, "Uniform", rb_cObject);



#endif