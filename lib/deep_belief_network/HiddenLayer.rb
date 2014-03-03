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
				@input             = opts[:input]
				@rng = opts[:rng] || Shark::RNG.new # and set state...
				@parameters = Shark::RealMatrix.new(@number_of_inputs, @number_of_outputs)
				@bias       = Shark::RealVector.new(@number_of_outputs)
			end

			def sigmoid x
				# (x.exp + 1.0)**(-1) works also
				(x.exp + 1.0).inverse
			end

			def output input=nil
				# sigmoid needs to be expressed in a shark fashion.
				# print "@input.size      => ",(input ? input : @input).size, "\n"
				# print "@input[0].to_a   => ", @input[0].to_a, "\n"
				# print "@parameters.size => ", @parameters.size, "\n"
				sigmoid((input ? input : @input) * ~@parameters + @bias)
			end

			def sample_h_given_v inputlayer=nil
				mean = nil
				if inputlayer
					mean = output inputlayer
				else
					mean = output
				end
				binomial = Shark::RNG::Binomial.new
				# binomial.p = mean
				binomial.n = 1
				# binomial.sample

				 # :p => mean,
					# 					 :n => 1,
					# 					 :size => inputlayer.size # will either be a tuple, or a scalar.
				# TODO: create this function according to https://github.com/yusugomori/DeepLearning/blob/master/cpp/DBN.cpp
				h_sample = nil
				if mean.size.is_a? Array
					h_sample = Shark::RealMatrix.new mean.size[0], mean.size[1] do |i,j|
						binomial.p = mean[i,j]
						binomial.sample
					end
				else
					h_sample = Shark::RealVector.new mean.size do |i,j|
						binomial.p = mean[i]
						binomial.sample
					end
				end
				# print "h_sample.size => ", h_sample.size, "\n"
				h_sample
			end
		end
	end
end