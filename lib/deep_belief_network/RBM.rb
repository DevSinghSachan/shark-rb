class Optimizer
	module RBM
		class RBM
			include Optimizer::RBM::RBMMethods
			attr_accessor :weight_matrix
			attr_accessor :hbias
			attr_accessor :vbias

			def initialize opts={}
				@visible = opts[:visible]
				@hidden = opts[:hidden]
				a = 1.0 / opts[:visible]
				@weight_matrix = opts[:weight_matrix] || Shark::RealMatrix.new(opts[:visible], opts[:hidden]) {|i| Random.rand(2*a)-a}
				@hbias = opts[:hidden_bias]  || Shark::RealVector.new(opts[:hidden])
				@vbias = opts[:visible_bias] || Shark::RealVector.new(opts[:visible])
				@input = opts[:input]
			end

			def hbias=(val)
				@hbias = val.to_blas_type
			end

			def hbias
				@hbias
			end

			def vbias
				@vbias
			end

			def vbias=(val)
				@vbias = val.to_blas_type
			end

			def sigmoid_activation_hidden input
				pre_sigmoid_activation_h = input * weight_matrix + @hbias
				sigmoid(pre_sigmoid_activation_h)
			end

			def sigmoid_activation_visible input
				pre_sigmoid_activation_v = input * ~weight_matrix + @vbias
				sigmoid(pre_sigmoid_activation_v)
			end

			def reconstruct v
				h = sigmoid( v * weight_matrix + hbias)
				reconstructed_v = sigmoid(h * ~weight_matrix + vbias)
				return reconstructed_v
			end

			def propup v
				pre_sigmoid_activation = v * (weight_matrix) + hbias
				return sigmoid(pre_sigmoid_activation)
			end

			def number_of_parameters
				@visible * @hidden + @hidden + @visible
			end

			def propdown h
				pre_sigmoid_activation = h * (~weight_matrix) + vbias
				return sigmoid(pre_sigmoid_activation)
			end

			def contrastive_divergence opts={}
				if opts[:input] then @input = opts[:input] end
				opts = {k: 1, learning_rate: 0.1}.merge opts
				k, lr = opts[:k], opts[:learning_rate]
				raise TrainingError.new "Cannot perform contrastive divergence without data. #contrastive_divergence {:input => <data>}" if @input.nil?
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
				
				self.weight_matrix += lr * (
						((~@input) * ph_sample) -
						((~nv_samples) * nh_means)
					)
				@vbias += lr * (@input - nv_samples).mean(axis:0)
				@hbias += lr * (ph_sample - nh_means).mean(axis:0)
				if opts[:verbose] then puts "Pre-training layer, cost = #{get_reconstruction_cross_entropy input}" end
			end
		end
	end
end