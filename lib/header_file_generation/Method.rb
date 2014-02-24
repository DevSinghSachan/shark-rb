module HeaderFileGenerator
	class HeaderFile

		class Method
			class Input
				class InputClass

					def initialize(typeName)
						@type = typeName
					end

					MatrixClasses = [
						InputClass.new("rb_RealMatrix"),
						InputClass.new("rb_RealMatrixReference")
					]

					ArrayClasses = [
						InputClass.new("rb_RealVector"),
						InputClass.new("rb_RealVectorReference"),
						InputClass.new("rb_RealMatrixColumn"),
						InputClass.new("rb_RealMatrixRow")
					]

					def rb_class
						@type + "::rb_class()"
					end

					def pointer
						@type + " *"
					end

					def wrapped_class
						if @type =~ /rb_(.+)/
							$1
						else
							cpp_class
						end
					end

					def wrapped_class_pointer
						case @type
						when *((ArrayClasses + MatrixClasses).map {|i| i.to_s})
							"getData()"
						else
							cpp_class
						end
					end

					def to_s
						cpp_class
					end

					def cpp_class
						@type
					end
				end


				attr_reader :type
				attr_reader :converted
				attr_accessor :input_class

				def initialize(opts={})
					@type = (opts[:type].is_a? Array) ? opts[:type] : opts[:type].to_sym
					@position = opts[:position]
					@converted = false
				end

				def parameter_name
					"parameter_#{@position+1}"
				end

				def converted_parameter_name
					# could also test whether the detected class requires a pointer...
					"parameter_#{@position+1}_converted"
				end

				def converted_parameter_pointer
					converted_parameter_name + "->#{@input_class.wrapped_class_pointer}"
				end

				def converted_parameter_object
					"(* #{converted_parameter_pointer})"
				end

				def converted_parameter_class
					case @type
					when :array
						InputClass.new "RealVector"
					when :"2darray"
						InputClass.new "RealMatrix"
					else
						InputClass.new @type.to_s
					end
				end

				def convert_from_double val
					"NUM2DBL(#{val})"
				end

				def convert_from_int val
					"NUM2FIX(#{val})"
				end

				def differs_from_classes classes
					classes.map {|i| "CLASS_OF(#{parameter_name}) != #{i.rb_class}"}.join(" && ")
				end

				def compatible_classes
					case @type
					when :array
						InputClass::ArrayClasses
					when :"2darray"
						InputClass::MatrixClasses
					else
						[]
					end
				end

				def convert_into_class inputClass
"""
	#{inputClass.pointer} #{converted_parameter_name};
	Data_Get_Struct(#{parameter_name}, #{inputClass}, #{converted_parameter_name});
"""
				end

				def convert_and_embed(all_params, remaining_params, calling_methodology)
					if requires_conversion?
						cpp = ""
						compatible_classes.each_with_index do |cpp_class, k|
							cpp += (k==0 ? "if" : "else if") + " (" + (matches_classes [cpp_class] ) + ") " + "{\n"
							cpp += convert_into_class cpp_class
							self.input_class = cpp_class
							if remaining_params.length > 0
								cpp += remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology
							else
								cpp += (calling_methodology.call() + ";\n")
							end
							cpp += "}"
						end
						cpp += "\n"
						cpp
					else
						if remaining_params.length > 0
							remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology
						else
							calling_methodology.call() + ";\n"
						end
					end
				end

				def requires_conversion?
					[:array].include? @type
				end

				def matches_classes classes
					classes.map {|i| "CLASS_OF(#{parameter_name}) == #{i.rb_class}"}.join(" || ")
				end
	# cpp = 
	# 	#{converted_parameter_class} #{converted_parameter_name};
	# 	if (TYPE(#{parameter_name}) == T_DATA) {
	# """

	# 		if (CLASS_OF(#{parameter_name}) == rb_RealVector::rb_class()) {
	# 		rb_RealVector *d;
	# 		Data_Get_Struct(rb_dataset, rb_RealVector, d);
	# 		std::vector<RealVector> vectors = realvector_to_stdvector(d->data);
	# 		r->getModel()->evaluationType(rb_direction == rb_sym_new("forward"), rb_mean == Qtrue);
	# 		try {
	# 			return wrap_pointer<rb_UnlabeledData>(
	# 				rb_UnlabeledData::rb_class(),
	# 				new rb_UnlabeledData(method_rbm_eval_unlabeleddata(r, shark::createDataFromRange(vectors)))
	# 			);
	# 		} catch (shark::Exception e) {
	# 			rb_raise(rb_eRuntimeError, (std::string(e.what()) + "\nCheck that the the data you are inputting matches the rbm's hidden or visible dimensions.").c_str());
	# 		}

	# 	} else {
	# 		rb_error_rbm_wrong_format<Obtype>();
	# 	}
	# } else if (TYPE(rb_dataset) == T_ARRAY) {

	# 	r->getModel()->evaluationType(rb_direction == rb_sym_new("forward"), rb_mean == Qtrue);
	# 	try {
	# 		return wrap_pointer<rb_UnlabeledData>(
	# 				rb_UnlabeledData::rb_class(),
	# 				new rb_UnlabeledData(method_rbm_eval_unlabeleddata(r, rb_ary_to_unlabeleddata(rb_dataset)))
	# 		);
	# 	} catch (shark::Exception e) {
	# 		rb_raise(rb_eRuntimeError, (std::string(e.what()) + "\nCheck that the the data you are inputting matches the rbm's hidden or visible dimensions.").c_str());
	# 	}

	# }"""
	# 				else
						
	# 				end
	# 			end

				def check
					case @type
					when :double
	"""
		if (TYPE(#{parameter_name}) != T_FIXNUM && TYPE(#{parameter_name}) != T_FLOAT)
			rb_raise(rb_eArgError, \"Argument #{@position+1} must be a Float.\");
	"""
					when :integer, :int
	"""
		if (TYPE(#{parameter_name}) != T_FIXNUM)
			rb_raise(rb_eArgError, \"Argument #{@position+1} must be an Integer.\");
	"""
					when :array
	"""
		if (TYPE(#{parameter_name}) != T_ARRAY && #{differs_from_classes InputClass::ArrayClasses})
			rb_raise(rb_eArgError, \"Argument #{@position+1} must be an ArrayType (\\\"#{(InputClass::ArrayClasses.map {|i| i.wrapped_class} + ["Array"]).join("\\\", \\\"")}\\\").\");
	"""
					else# implement other checks as needs be.
						""
					end
				end

				def to_converted_form
					case @type
					when :double
						convert_from_double parameter_name
					when :integer, :int
						convert_from_int parameter_name
					when :array
						raise RuntimeError.new "#{parameter_name} of type #{@type} was used before an input class was determined." if @input_class.nil?
						converted_parameter_object
					else # must've been converted beforehand...
						parameter_name
					end
				end

				def to_s
					to_converted_form
				end
			end
		end

		class Method

			def className
				@header_file.cpp_class_name
			end

			def initialize(opts={})
				raise StandardError.new "\"name\" cannot contain any non a-Z 0-9 _ characters" if (opts["name"] and opts["name"].match(/[^a-zA-Z0-9_]/))
				@header_file      = opts["hf"]
				@cpp_method_name  = opts["accessor_name"] || opts["name"]
				@method_name      = opts["name"]
				@input_type       = opts["types"] || ["nil"]
				@number_of_inputs = opts["number_of_inputs"] || 0
				if opts["types"] and opts["types"].length > 0 and (opts["number_of_inputs"].nil? || opts["number_of_inputs"] < opts["types"].length)
					puts "@header_file.cpp_class_name = #{@header_file.cpp_class_name}\n@method_name  = #{@method_name}\nopts[\"types\"] = #{opts["types"]}\n\n"
					@number_of_inputs = opts["types"].length
				end
				@parameters       = []
				@number_of_inputs.times do |i|
					@parameters << Input.new(:type => (@input_type[i] || @input_type[0]), :position => i)
				end
				@requires_conversion = @parameters.select {|i| i.requires_conversion?}.length > 0
				@return_type      = (opts["type"] || "nil").to_sym
			end

			def symbol
				"r"
			end

			def parameter_name pos
				"parameter_#{pos+1}"
			end

			def input_parameters
				ip =  "VALUE self"
				@number_of_inputs.times do |i|
					ip += ", VALUE #{parameter_name(i)}"
				end
				ip
			end

			def parameters
				params = []
				@parameters.each do |param|
					params << param.to_s
				end
				params.join(", ")
			end

			def parameter_conversions
				parameters_to_convert = @parameters
				cpp = ""
				cpp += @parameters.first.convert_and_embed(
					@parameters,
					@parameters[1..(@parameters.length - 1)],
					Proc.new {self.call_methodology})
				cpp
			end

			def checking_methodology
				checks = []
				@parameters.each do |param|
					checks << param.check
				end
				checks.join("\n")
			end

			def call_methodology_with_conversion
"""
#{parameter_conversions};
"""
			end

			def call_methodology
				if @cpp_method_name =~ /operator(.+)/
					if $1 == "()"
						"(*#{symbol}->#{@header_file.pointer_acquirer.first}())(#{parameters})"
					else
						"(*#{symbol}->#{@header_file.pointer_acquirer.first}())#{$1}(#{parameters})"
					end
				else
						"#{symbol}->#{@header_file.pointer_acquirer.first}()->#{@cpp_method_name}(#{parameters})"
				end
			end

			def return_methodology
				case @return_type
				when :double
					"\treturn rb_float_new(#{call_methodology})"
				when :integer
					"\treturn INT2FIX(#{call_methodology})"
				else
					"\t#{call_methodology};\n\treturn self"
				end
			end

			def to_cpp_function_definition
				cpp = """
VALUE #{function_name} (#{input_parameters}) {
	#{className} *#{symbol};
	#{checking_methodology}
"""
				if @requires_conversion
					cpp += "#{call_methodology_with_conversion};\n\n"
				else
					cpp += "#{return_methodology};\n\n"
				end
				cpp
			end

			def function_name
				"method_#{className}_#{@method_name}"
			end

			def rb_method_name
				@method_name
			end

			def to_rb_function_definition
				"rb_define_method(#{className}::rb_class(), \"#{rb_method_name}\", (rb_method) #{function_name}, #{@number_of_inputs});"
			end

			def to_s
				to_cpp_function_definition
			end
		end

		class HeaderFileGenerator::HeaderFile::Allocator < HeaderFile::Method
			def to_cpp_function_definition
"""
VALUE #{function_name} (VALUE klass) {
	return wrap_pointer<#{className}>(
		#{className}::rb_class(),
		new #{className}()
		);
}
"""
			end

			def function_name
				"method_#{className}_allocate"
			end

			def to_rb_function_definition
				"rb_define_alloc_func(#{className}::rb_class(), (rb_alloc_func_t) #{function_name});"
			end
		end

		class HeaderFileGenerator::HeaderFile::Initializer < HeaderFile::Method
			def initialize(opts={})
				super ({"number_of_inputs" => 0, "name" => "initialize"}.merge(opts))
			end

			def to_cpp_function_definition
"""
VALUE #{function_name} (VALUE self) {
	return self;
}
"""
			end
		end

		class HeaderFileGenerator::HeaderFile::Getter < HeaderFile::Method
			def initialize(opts={})
				super({"number_of_inputs" => 0}.merge(opts))
			end

			def function_name
				"method_#{className}_get_#{@method_name}"
			end
		end

		class HeaderFileGenerator::HeaderFile::Setter < HeaderFile::Method
			def initialize(opts={})
				super({"number_of_inputs" => 1}.merge(opts))
			end

			def rb_method_name
				@method_name + "="
			end

			def function_name
				"method_#{className}_set_#{@method_name}"
			end
		end
	end
end