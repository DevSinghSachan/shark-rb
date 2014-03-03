module HeaderFileGenerator
	class HeaderFile

		class Method

			attr_reader :method_name
			attr_reader :input_type
			attr_reader :cpp_method_name
			attr_reader :header_file
			attr_reader :return_type
			attr_reader :parameters

			def initialize(opts={})
				raise StandardError.new "\"name\" cannot contain any non a-Z 0-9 _ characters" if (opts["name"] and opts["name"].match(/[^a-zA-Z0-9_]/))
				@header_file      = opts["hf"]
				if opts["hf"].nil? then raise StandardError.new "Cannot initialize a header file method without providing a Header File." end
				@cpp_method_name  = opts["accessor_name"] || opts["name"]
				@method_name      = opts["name"]
				@input_type       = opts["types"] || ["nil"]
				@number_of_inputs = opts["number_of_inputs"] || 0
				if opts["types"] and opts["types"].length > 0 and (opts["number_of_inputs"].nil? || opts["number_of_inputs"] < opts["types"].length)
					@number_of_inputs = opts["types"].length
				end
				@parameters       = []

				@number_of_inputs.times do |i|
					input_type = @input_type[i] || @input_type[0]
					if input_type.is_a? Array
						@parameters << OverloadedInput.new(
							:type         => input_type,
							:position     => i,
							:method       => self,
							:output_classes => input_type.map {|i| CppClass.guess_from_type(i)}
							)
					else
						@parameters << Input.new(
							:type         => input_type,
							:position     => i,
							:method       => self,
							:output_class => CppClass.guess_from_type(input_type)
							)
					end
				end
				@requires_conversion = @parameters.any? {|i| i.requires_conversion?}
				 # switch this to classes.
				@return_type, @cast_return_type = create_return_type opts["type"]
			end

			def create_return_type type_definition
				if type_definition.is_a? Hash
					[
						(type_definition["name"] || "nil").to_sym,
						(type_definition["cast"].nil? ? false : type_definition["cast"])
					]
				else
					[
						(type_definition || "nil").to_sym,
						false
					]
				end
			end

			def cpp_class
				@header_file.cpp_class
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

			def params some_parameters
				params = []
				some_parameters.each do |param|
					params << param.to_s
				end
				params.join(", ")
			end

			def parameter_conversions some_parameters, indent=0
				parameters_to_convert = some_parameters
				parameters_requiring_conversions = some_parameters.select {|i| i.requires_conversion?}
				cpp = ""
				cpp += "#{"\t"*indent}// Converting parameter#{parameters_requiring_conversions.length == 1 ? "" : "s"} #{"\""+parameters_requiring_conversions.map {|i| i.parameter_name}.join("\", \"")+"\""} before #{parameters_requiring_conversions.length == 1 ? "it" : "they"} can be used.\n"
				cpp += some_parameters.first.convert_and_embed(
					some_parameters,
					some_parameters[1..(some_parameters.length - 1)],
					->(i=0) {self.return_methodology(some_parameters, i)},
					indent)
				cpp
			end

			def checking_methodology some_parameters, indent=0
				checks = []
				some_parameters.each do |param|
					checks << param.check(indent)
				end
				checks.join("\n")
			end

			def call_methodology_with_conversion some_parameters, indent=0
"""
#{parameter_conversions some_parameters, indent}
#{"\t"*indent}return self; // cpp functions require return variable, so if all tests fail \"self\" is returned.
"""
			end

			def current_cpp_method_name
				@cpp_method_name
			end

			def call_methodology some_parameters, indent=0
				if current_cpp_method_name =~ /operator\s*(.+)/
					if $1 == "()"
						"#{"\t"*indent}(*(#{symbol}->#{@header_file.pointer_acquirer.first}()))(#{params(some_parameters)})"
					else
						"#{"\t"*indent}(*(#{symbol}->#{@header_file.pointer_acquirer.first}()))#{$1} #{params(some_parameters)}"
					end
				else
					"#{"\t"*indent}#{symbol}->#{@header_file.pointer_acquirer.first}()->#{current_cpp_method_name}(#{params(some_parameters)})"
				end
			end

			def current_return_type
				@return_type
			end
			def current_cast_return_type_cast
				@cast_return_type
			end

			def return_methodology some_parameters, indent=0
				if current_return_type.downcase == :nil
					"#{call_methodology(some_parameters, indent)};\n#{"\t"*indent}return self;"
				else
					output = Output.new(call_methodology(some_parameters), :type => current_return_type, :cast => current_cast_return_type_cast)
					if output.requires_cast?
						(
							("\t"*indent) + output.cast_variable    + ";\n" +
							("\t"*indent) + "return " + output.to_s + ";"
						)
					else
						("\t"*indent) + "return " + output.to_s + ";"
					end
				end
			end

			def to_cpp_function_definition
				cpp = """
VALUE #{function_name} (#{input_parameters}) {
	#{cpp_class.pointer} #{symbol};
	Data_Get_Struct(self, #{cpp_class}, #{symbol});
#{checking_methodology @parameters, 1}
"""
				if @requires_conversion
					cpp += "#{call_methodology_with_conversion @parameters, 1}}\n"
				else
					cpp += "#{return_methodology(@parameters, 1)}\n}\n"
				end
				cpp
			end

			def function_name
				"method_#{cpp_class}_#{@method_name}"
			end

			def rb_method_name
				@method_name
			end

			def to_rb_function_definition
				"rb_define_method(#{cpp_class.rb_class}, \"#{rb_method_name}\", (rb_method) #{function_name}, #{@number_of_inputs});"
			end

			def to_s
				to_cpp_function_definition
			end
		end

		class HeaderFileGenerator::HeaderFile::Allocator < HeaderFile::Method
			def to_cpp_function_definition
"""
VALUE #{function_name} (VALUE klass) {
	return #{cpp_class.to_rb};
}
"""
			end

			def function_name
				"method_#{cpp_class}_allocate"
			end

			def to_rb_function_definition
				"rb_define_alloc_func(#{cpp_class.rb_class}, (rb_alloc_func_t) #{function_name});"
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
				"method_#{cpp_class}_get_#{@method_name}"
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
				"method_#{cpp_class}_set_#{@method_name}"
			end
		end
	end
end