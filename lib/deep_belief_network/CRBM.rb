class Optimizer
	module RBM
		class CRBM < Optimizer::RBM::RBM
			def propdown h
		        pre_activation = h * weight_matrix + vbias
		        return pre_activation
		    end

		  #   def sample_v_given_h h0_sample
				# a_h = propdown(h0_sample)
				# en = (-a_h).exp
				# ep = (a_h).exp

				# v1_mean = 1 / (1 - en) - 1 / a_h
				# # sample from uniform
				# U = numpy.array(self.numpy_rng.uniform(
				#     low=0,
				#     high=1,
				#     size=v1_mean.shape))

				# v1_sample = ((1 - U * (1 - ep))).log / a_h

				# return [v1_mean, v1_sample]
		  #   end
		end
	end
end