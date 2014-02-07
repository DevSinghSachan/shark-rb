#ifndef RB_RBM_ANALYTICS_H
#define RB_RBM_ANALYTICS_H

#include "ruby.h"
#include "rb_UnlabeledData.h"
#include "rb_BinaryRBM.h"
#include "rb_conversions.h"
#include <shark/Unsupervised/RBM/analytics.h>

VALUE method_rbm_analytics_negativeLogLikelihood (int, VALUE*, VALUE);
VALUE method_rbm_analytics_logPartitionFunction (int, VALUE*, VALUE);
VALUE method_rbm_analytics_negativeLogLikelihoodFromLogPartition (int, VALUE*, VALUE);

void Init_RBM_Analytics();

#endif