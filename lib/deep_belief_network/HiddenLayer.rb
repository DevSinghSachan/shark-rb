class Optimizer
	module RBM
		class HiddenLayer
			# TODO: implement this in C++ perhaps?
			attr_accessor :parameters
			attr_accessor :bias
			attr_accessor :input
			
			attr_reader   :number_of_inputs
			alias :input_size :number_of_inputs
			
			attr_reader   :number_of_outputs
			alias :output_size :number_of_outputs

			alias :weight_matrix :parameters

			def output input=nil
				@input = input ? input : @input
				sigmoid(@input * @parameters + @bias)
			end

			def initialize opts={}
				@number_of_inputs  = opts[:input_size]
				@number_of_outputs = opts[:output_size]
				@input             = opts[:input]
				@transpose         = opts[:transpose]
				#@rng        = opts[:rng] || Shark::RNG.new # and set state...
				@parameters = Shark::RealMatrix.new(@number_of_inputs, @number_of_outputs)
				@bias       = Shark::RealVector.new(@number_of_outputs)
				if @transpose then transpose_output() end
			end

			def transpose_output
				def self.output(input=nil)
					@input = input ? input : @input
					sigmoid(@input * ~@parameters + @bias)
				end
			end

			def untranspose_output
				def self.output(input=nil)
					@input = input ? input : @input
					sigmoid(@input * @parameters + @bias)
				end
			end

			def tranpose?
				@transpose
			end

			def transpose=(val)
				if val
					@transpose = true
					transpose_output
				else
					@transpose = false
					untranspose_output
				end
			end

			def sigmoid x
				((-x).exp + 1.0).inverse
			end

			def sample_h_given_v inputlayer=nil
				@input = inputlayer ? inputlayer : @input
				mean = output()
				binomial   = Shark::RNG::Binomial.new
				binomial.n = 1
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