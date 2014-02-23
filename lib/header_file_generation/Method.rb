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
				@input_type       = opts["types"] || "nil"
				@number_of_inputs = opts["number_of_inputs"] || 0
				@return_type      = opts["type"] || "nil"
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

			def call_methodology
				params = []
				@number_of_inputs.times do |i|
					params << parameter_name(i)
				end
				if @cpp_method_name =~ /operator(.+)/
					if $1 == "()"
						"(*#{symbol}->#{@header_file.pointer_acquirer}())(#{params.join(", ")})"
					else
						"(*#{symbol}->#{@header_file.pointer_acquirer}())#{$1}(#{params.join(", ")})"
					end
				else
					"#{symbol}->#{@header_file.pointer_acquirer}()->#{@cpp_method_name}(#{params.join(", ")})"
				end
			end

			def return_methodology
				case @return_type
				when "double"
					"\treturn NUM2DBL(#{call_methodology})"
				else
					"\t#{call_methodology};\n\treturn self"
				end
			end

			def to_cpp_function_definition
"""
VALUE #{function_name} (#{input_parameters}) {
	#{className} *#{symbol};
	Data_Get_Struct(self, #{className}, #{symbol});
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
				"rb_define_alloc_func(#{className}::rb_class(), (rb_alloc_func_t) method_#{className}_allocate);"
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