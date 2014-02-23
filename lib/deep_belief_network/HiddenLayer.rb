class Optimizer
	module RBM
		class HiddenLayer
			# TODO: implement this in C++ perhaps?
			attr_accessor :parameters
			attr_reader   :number_of_inputs
			attr_reader   :number_of_outputs

			def initialize opts={}
				@number_of_inputs  = opts[:input_size]
				@number_of_outputs = opts[:output_size]
				@rng = opts[:rng] || Shark::RNG.new # and set state...
				@parameters = Shark::RealMatrix.new(@number_of_inputs, @number_of_outputs)
			end

			def sigmoid x
				1.0 / (1.0 + Math.exp(x))
			end

			def output inputlayer, weights, bias
				# sigmoid needs to be expressed in a shark fashion.
				sigmoid(inputlayer*weights + bias)
			end

			def biasvalue index
				## some special Shark operation
				0.0
			end

			def sample_h_given_v inputlayer
				mean = nil
				if inputlayer
					mean = output inputlayer
				else
					mean = output
				end
				binomial = @rng.binomial :p => mean,
										 :n => 1,
										 :size => inputlayer.size # will either be a tuple, or a scalar.
				# TODO: create this function according to https://github.com/yusugomori/DeepLearning/blob/master/cpp/DBN.cpp
				Shark::UnlabeledData.new([])
			end
		end
	end
end