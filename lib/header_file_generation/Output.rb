module HeaderFileGenerator
	class HeaderFile
		class Method
			class Output

				attr_reader :type
				attr_reader :converted
				attr_accessor :output_class
				attr_reader :output_name

				# centralize these types...
				ArrayTypes   = CppClass::ArrayTypes
				MatrixTypes  = CppClass::MatrixTypes
				IntegerTypes = CppClass::IntegerTypes
				DataTypes    = CppClass::DataTypes
				ColumnTypes  = [:realmatrixcolumn, :matrixcolumn, :column]
				RowTypes     = [:realmatrixrow, :matrixrow, :row]
				VectorReferenceTypes = [:realvectorreference, :vectorreference]
				ObjectTypes  = ArrayTypes + MatrixTypes + ColumnTypes + RowTypes + VectorReferenceTypes + DataTypes

				def initialize(name, opts={})
					@type         = opts[:type].to_sym
					@converted    = false
					@cast         = opts[:cast].nil? ? false : opts[:cast]
					@output_name  = name
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
					when *DataTypes
						CppClass.new("rb_UnlabeledData")
					when *ArrayTypes
						CppClass.new("rb_RealVector")
					when *MatrixTypes
						CppClass.new("rb_RealMatrix")
					when *([:nil, :double, :rubyarray, :ruby2darray] + IntegerTypes)
						nil
					else
						raise CppError.new "No compatible class found for #{@type}."
						nil
					end
				end

				def convert_into_rb_object val
					@output_class.to_rb val
				end

				def requires_cast?
					@cast
				end

				def casted_variable_name
					"casted_output"
				end

				def cast_variable
					# could add a protection here to check if variable was casted...
					@output_class.declare_variable(casted_variable_name).from(output_name)
				end

				def to_converted_form
					out_name = requires_cast? ? casted_variable_name : output_name
					case @type.downcase
					when :double
						Converter.convert(out_name).from("double").to Float
					when *IntegerTypes
						Converter.convert(out_name).from("int").to Fixnum
					when :rubyarray
						Converter.convert(out_name).from("RealVector").to Array
					when :ruby2darray
						Converter.convert(out_name).from("RealMatrix").to Array
						# should check c++ function signature here.
					when *ObjectTypes
						if @output_class.nil? then raise RuntimeError.new "#{output_name} of type #{@type} was used before an output class was determined." end
						convert_into_rb_object out_name
					else # must've been converted beforehand...
						out_name
					end
				end

				def to_s
					to_converted_form
				end

			end
		end
	end
end