module HeaderFileGenerator
	class HeaderFile

		class Method
			class Input
				attr_reader :type

				def initialize(opts={})
					@type = opts[:type].to_sym
					@position = opts[:position]
				end

				def parameter_name
					"parameter_#{@position+1}"
				end

				def convert_from_double val
					"NUM2DBL(#{val})"
				end

				def check
					case @type
					when :double
"""
	if (TYPE(#{parameter_name}) != T_FIXNUM && TYPE(#{parameter_name}) != T_FLOAT)
		rb_raise(rb_eArgError, \"Argument #{@position+1} must be a Float.\");
"""
					else# implement other checks as needs be.
						""
					end
				end

				def to_converted_form
					case @type
					when :double
						convert_from_double parameter_name
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
				@parameters       = []
				@number_of_inputs.times do |i|
					@parameters << Input.new(:type => (@input_type[i] || @input_type[0]), :position => i)
				end
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

			def checking_methodology
				checks = []
				@parameters.each do |param|
					checks << param.check
				end
				checks.join("\n")
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
				else
					"\t#{call_methodology};\n\treturn self"
				end
			end

			def to_cpp_function_definition
"""
VALUE #{function_name} (#{input_parameters}) {
	#{className} *#{symbol};
	Data_Get_Struct(self, #{className}, #{symbol});
#{checking_methodology}
#{return_methodology};
}
"""
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