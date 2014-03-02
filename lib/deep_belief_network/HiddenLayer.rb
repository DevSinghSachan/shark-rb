class Optimizer
	module RBM
		class HiddenLayer
			# TODO: implement this in C++ perhaps?
			attr_accessor :parameters
			attr_accessor :bias
			attr_reader   :number_of_inputs
			attr_reader   :number_of_outputs

			def initialize opts={}
				@number_of_inputs  = opts[:input_size]
				@number_of_outputs = opts[:output_size]
				@rng = opts[:rng] || Shark::RNG.new # and set state...
				@parameters = Shark::RealMatrix.new(@number_of_inputs, @number_of_outputs)
				@bias       = Shark::RealVector.new(@number_of_outputs)
			end

			def sigmoid x
				# (x.exp + 1.0)**(-1) works also
				(x.exp + 1.0).inverse
			end

			def output inputlayer, weights, bias
				# sigmoid needs to be expressed in a shark fashion.
				sigmoid(inputlayer * weights + bias)
			end

			def sample_h_given_v inputlayer
				mean = nil
				if inputlayer
					mean = output inputlayer
				else
					mean = output
				end
				binomial = Shark::RNG::Binomial.new
				puts mean.class, mean.size
				binomial.p = mean
				binomial.n = 1
				binomial.sample

				 # :p => mean,
					# 					 :n => 1,
					# 					 :size => inputlayer.size # will either be a tuple, or a scalar.
				# TODO: create this function according to https://github.com/yusugomori/DeepLearning/blob/master/cpp/DBN.cpp
				if mean.size.is_a? Array
					Shark::RealMatrix.new mean.size[0], mean.size[1] do |i,j|
						binomial.p = mean[i,j]
						binomial.sample
					end
				else
					Shark::RealVector.new mean.size do |i,j|
						binomial.p = mean[i]
						binomial.sample
					end
				end
			end
		end
	end
end