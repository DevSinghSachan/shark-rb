class Optimizer
	module RBM
		module RBMMethods

			def input=(val)
				@input = val
			end

			def input
				@input
			end

			def hbias
				hidden_neurons.bias
			end

			def hbias=(val)
				hidden_neurons.bias=val
			end

			def output
			end

			def vbias
				visible_neurons.bias
			end

			def vbias=(val)
				visible_neurons.bias=val
			end

			def sigmoid x
				((-x).exp + 1.0).inverse
			end

			def initialize_random_uniform val=0.1
				self.weight_matrix = Shark::RealMatrix.new(self.weight_matrix.size1, self.weight_matrix.size2) do |i|
					Random.rand(2*val)-val
				end
			end

			def sigmoid_activation_hidden input
				pre_sigmoid_activation_h = input * ~weight_matrix + hidden_neurons.bias
				sigmoid(pre_sigmoid_activation_h)
			end

			def sigmoid_activation_visible input
				pre_sigmoid_activation_v = input * weight_matrix + visible_neurons.bias
				sigmoid(pre_sigmoid_activation_v)
			end

			def get_reconstruction_cross_entropy input
				sigmoid_activation_h     = sigmoid_activation_hidden input
				sigmoid_activation_v     = sigmoid_activation_visible sigmoid_activation_h

				- (
						(
							input.hadamard(sigmoid_activation_v.log) +
						    (-input + 1.0).hadamard((-sigmoid_activation_v + 1.0).log)
					    ).sum(axis:1)
				).mean
			end

			def propup v
				pre_sigmoid_activation = v * (~weight_matrix) + hidden_neurons.bias
				return sigmoid(pre_sigmoid_activation)
			end

			def sample_h_given_v v0_sample
				h1_mean   = propup v0_sample
				binomial = Shark::RNG::Binomial.new
				binomial.n = 1
				# TODO: simplify Binomials (15th March 2014)
				if h1_mean.is_a? Shark::RealMatrix
					h1_sample = Shark::RealMatrix.new(h1_mean.size1, h1_mean.size2) do |i, j|
						binomial.p = h1_mean[i,j]
						binomial.sample
					end
					[h1_mean, h1_sample]
				else
					h1_sample = Shark::RealVector.new(h1_mean.size) do |i|
						binomial.p = h1_mean[i]
						binomial.sample
					end
					[h1_mean, h1_sample]
				end
			end

			def sample_v_given_h h0_sample
				v1_mean = self.propdown h0_sample
				binomial = Shark::RNG::Binomial.new
				binomial.n = 1
				if v1_mean.is_a? Shark::RealMatrix
					v1_sample = Shark::RealMatrix.new(v1_mean.size1, v1_mean.size2)
					v1_mean.each_cell_with_index do |cell, i, j|
						binomial.p = cell
						v1_sample[i,j] = binomial.sample
					end
					[v1_mean, v1_sample]
				else
					v1_sample = Shark::RealVector.new(v1_mean.size)
					v1_mean.each_with_index do |cell, i|
						binomial.p = cell
						v1_sample[i] = binomial.sample
					end
					[v1_mean, v1_sample]
				end
			end

			def contrastive_divergence opts={}
				if !opts[:input].nil? then @input = opts[:input] end
				opts = {k: 1, learning_rate: 0.1}.merge opts
				k, lr = opts[:k], opts[:learning_rate]
				raise Optimizer::RBM::TrainingError.new "Cannot perform contrastive divergence without data. #contrastive_divergence {:input => <data>}" if @input.nil?
				*, ph_sample = sample_h_given_v @input
				chain_start = ph_sample

				nv_means, nv_samples, nh_means, nh_samples = [nil, nil, nil, nil]

				k.times do |step|
					if step == 0
						nv_means, nv_samples, nh_means, nh_samples = gibbs_hvh chain_start
					else
						nv_means, nv_samples, nh_means, nh_samples = gibbs_hvh nh_samples
					end
				end
				
				self.weight_matrix   += lr * ~(
						((~@input) * ph_sample) -
						((~nv_samples) * nh_means)
					)
				visible_neurons.bias += lr * (@input - nv_samples).mean(axis:0)
				hidden_neurons.bias  += lr * (ph_sample - nh_means).mean(axis:0)
				if opts[:verbose] then puts "Pre-training layer, cost = #{get_reconstruction_cross_entropy input}" end
			end

			def propdown h
				pre_sigmoid_activation = h * weight_matrix + visible_neurons.bias
				sigmoid(pre_sigmoid_activation)
			end

			def gibbs_hvh h0_sample
				v1_mean, v1_sample = sample_v_given_h h0_sample
				h1_mean, h1_sample = sample_h_given_v v1_sample

				[
					v1_mean,
					v1_sample,
					h1_mean,
					h1_sample
				]
			end

			def reconstruct v
				h = sigmoid( v * ~weight_matrix + hidden_neurons.bias)
				reconstructed_v = sigmoid(h * weight_matrix + visible_neurons.bias)
				return reconstructed_v
			end
		end
		
		class BinaryRBM
			include RBMMethods
		end

		class TrainingError < ArgumentError
		end
	end
end