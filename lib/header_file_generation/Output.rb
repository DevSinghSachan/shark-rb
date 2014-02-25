module HeaderFileGenerator
	class HeaderFile
		class Method
			class Output

				attr_reader :type
				attr_reader :converted
				attr_accessor :output_class
				attr_reader :output_name

				ArrayTypes   = [:array, :"1darray", :vector, :realvector]
				MatrixTypes  = [:matrix, :"2darray", :realmatrix]
				IntegerTypes = [:integer, :int]
				ColumnTypes  = [:realmatrixcolumn, :matrixcolumn, :column]
				RowTypes     = [:realmatrixrow, :matrixrow, :row]
				VectorReferenceTypes = [:realvectorreference, :vectorreference]
				ObjectTypes  = ArrayTypes + MatrixTypes + ColumnTypes + RowTypes + VectorReferenceTypes

				def initialize(name, opts={})
					@type = opts[:type].to_sym
					@converted = false
					@output_name = name
					@output_class = compatible_class
				end

				def compatible_class
					case @type.downcase
					when *VectorReferenceTypes
						CppClass.new("rb_RealVectorReference", :pointer => true)
					when *RowTypes
						CppClass.new("rb_RealMatrixRow", :pointer => true)
					when *ColumnTypes
						CppClass.new("rb_RealMatrixColumn", :pointer => true)
					when *ArrayTypes
						CppClass.new("rb_RealVector")
					when *MatrixTypes
						CppClass.new("rb_RealMatrix")
					when *([:nil, :double, :rubyarray, :ruby2darray] + IntegerTypes)
						nil
					else
						raise NotImplementedError.new "No compatible class found for #{@type}."
						nil
					end
				end

				def convert_into_double val
					"rb_float_new(#{val})"
				end

				def convert_into_int val
					"NUM2FIX(#{val})"
				end

				def convert_into_rb_object val
					@output_class.to_rb val
				end

				def to_converted_form
					case @type.downcase
					when :double
						Converter.convert(output_name).from("double").to Float
						#convert_into_double output_name
					when *IntegerTypes
						Converter.convert(output_name).from("int").to Fixnum
						#convert_into_int output_name
					when :rubyarray
						Converter.convert(output_name).from("RealVector").to Array
					when :ruby2darray
						Converter.convert(output_name).from("RealMatrix").to Array
						# should check c++ function signature here.
					when *ObjectTypes
						if @output_class.nil? then raise RuntimeError.new "#{output_name} of type #{@type} was used before an output class was determined." end
						convert_into_rb_object output_name
					else # must've been converted beforehand...
						output_name
					end
				end

				def to_s
					to_converted_form
				end

			end
		end
	end
end