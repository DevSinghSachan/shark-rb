class Optimizer
	module RBM
		class HiddenLayer
			# TODO: implement this in C++ perhaps?
			attr_accessor :parameters
			attr_reader   :number_of_inputs
			attr_reader   :number_of_outputs

			def initialize n_inputs, n_outputs
				@number_of_inputs = inputs
				@number_of_outputs = outputs
				@parameters = Shark::RealVector.new(0)
			end

			def sigmoid x
				1.0 / (1.0 + Math.exp(x))
			end

			def output inputlayer, weights, bias
				sigmoid(inputlayer*weights + bias)
			end

			def biasvalue index
				## some special Shark operation
				0.0
			end

			def sample_h_given_v inputlayer
				binomial = Shark::Binomial.new
				solution = []
				@number_of_outputs.times do |i|
					solution = binomial.sample 1, output(input, @parameters[i], @parameters[biasvalue(i)])
				end

				# TODO: create this function according to https://github.com/yusugomori/DeepLearning/blob/master/cpp/DBN.cpp
				Shark::UnlabeledData.new([])
			end
		end
	end
end