class Optimizer
	module RBM
		class CRBM < Optimizer::RBM::RBM

			def propdown h
				# propagation downwards without a sigmoid! (hence it's linear)
				h * ~weight_matrix + vbias
			end

		    def sample_v_given_h h0_sample
				a_h = propdown(h0_sample)
				en = (-a_h).exp
				ep = (a_h).exp

				v1_mean = (-en + 1).inverse - a_h.inverse
				# sample from uniform
				u = Shark::RealMatrix.new(v1_mean.size1, v1_mean.size2) do
					Random.rand()
				end

				v1_sample = ((-u.hadamard(-ep + 1) ) + 1).log / a_h

				[v1_mean, v1_sample]
		    end
		end
	end
end