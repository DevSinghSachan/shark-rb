module HeaderFileGenerator
	class HeaderFile

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
					#puts "@header_file.cpp_class_name = #{@header_file.cpp_class_name}\n@method_name  = #{@method_name}\nopts[\"types\"] = #{opts["types"]}\n\n"
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
				cpp += "\t// Converting parameters #{"\""+@parameters.select {|i| i.requires_conversion?}.map {|i| i.parameter_name}.join("\", \"")+"\""} before they can be used.\n"
				cpp += @parameters.first.convert_and_embed(
					@parameters,
					@parameters[1..(@parameters.length - 1)],
					->(i=0) {self.return_methodology(i)},
					1)
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
#{parameter_conversions}
	return self; // cpp functions require return variable, so if all tests fail \"self\" is returned.
"""
			end

			def call_methodology indent=0
				if @cpp_method_name =~ /operator(.+)/
					if $1 == "()"
						"#{"\t"*indent}(*#{symbol}->#{@header_file.pointer_acquirer.first}())(#{parameters})"
					else
						"#{"\t"*indent}(*#{symbol}->#{@header_file.pointer_acquirer.first}())#{$1}(#{parameters})"
					end
				else
						"#{"\t"*indent}#{symbol}->#{@header_file.pointer_acquirer.first}()->#{@cpp_method_name}(#{parameters})"
				end
			end

			def return_methodology indent=0
				case @return_type
				when *([:double] + Output::IntegerTypes + Output::ArrayTypes)
					"\t"*indent + "return " + Output.new(call_methodology, :type => @return_type).to_s+";"
				else
					"#{call_methodology(indent)};\n#{"\t"*indent}return self"
				end
			end

			def to_cpp_function_definition
				cpp = """
VALUE #{function_name} (#{input_parameters}) {
	#{className} *#{symbol};
	#{checking_methodology}
"""
				if @requires_conversion
					cpp += "#{call_methodology_with_conversion}}\n\n"
				else
					cpp += "#{return_methodology(1)}\n}\n\n"
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
	return #{CppClass.new(className).to_rb};
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