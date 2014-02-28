module HeaderFileGenerator
	class HeaderFile
		class Method
			class Input

				attr_reader :type
				attr_reader :converted
				attr_accessor :input_class
				attr_accessor :output_class
				attr_reader :position

				ArrayTypes   = [:array, :"1darray", :vector, :"std::vector<double>"]
				MatrixTypes  = [:matrix, :"2darray"]
				IntegerTypes = [:integer, :int]

				def initialize(opts={})
					@type = (opts[:type].is_a? Array) ? opts[:type] : opts[:type].to_sym
					@position = opts[:position]
					@converted = false
					@method = opts[:method]
					@output_class = opts[:output_class]
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

				def conversion_to_correct_type param
					Converter.convert(param).from(@input_class.wrapped_class).to(@output_class.wrapped_class)
				end

				def converted_parameter_object
					conversion_to_correct_type @input_class.converted_parameter_pointer(converted_parameter_name)
				end

				def differs_from_classes classes
					classes.map {|i| i.fail_class_test parameter_name}.join(" && ")
				end

				def compatible_classes
					classes = CppClass.can_convert_to(@output_class)
					if classes.empty? then raise NotImplementedError.new "No compatible conversion classes for #{@output_class}."
					else classes end
					# case @type.downcase
					# when :double
					# 	CppClass::DoubleClasses
					# when *IntegerTypes
					# 	CppClass::IntegerClasses
					# when *ArrayTypes
					# 	[CppClass::RubyArray] + CppClass::ArrayClasses
					# when *MatrixTypes
					# 	[CppClass::Ruby2DArray] + CppClass::MatrixClasses
					# else
					# 	[]
					# end
				end

				def convert_and_embed(all_params, remaining_params, calling_methodology, indent=0)
					if requires_conversion?
						cpp = ""
						compatible_classes.each_with_index do |cpp_class, k|
							cpp += (k==0 ? "#{"\t"*indent}if" : " else if") + " (" + (matches_classes [cpp_class] ) + ") " + "{"
							cpp += cpp_class.convert_into_class self, indent+1
							self.input_class = cpp_class
							if remaining_params.length > 0
								cpp += remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology, indent+1
							else
								cpp += (calling_methodology.call(indent+1) + "\n")
							end
							cpp += ("\t"*indent+"}")
						end
						cpp += "\n"
						cpp
					else
						if remaining_params.length > 0
							remaining_params.first.convert_and_embed all_params, remaining_params[1..(remaining_params.length-1)], calling_methodology, indent
						else
							calling_methodology.call(indent) + "\n"
						end
					end
				end

				def requires_conversion?
					(ArrayTypes + MatrixTypes).include? @type
				end

				def matches_classes classes
					classes.map {|i| i.test_class parameter_name}.join(" || ")
				end

				def test_if_not_1darray
					"TYPE(#{parameter_name}) != T_ARRAY || (RARRAY_LEN(#{parameter_name}) > 0 && TYPE(rb_ary_entry(#{parameter_name}, 0)) != T_FLOAT && TYPE(rb_ary_entry(#{parameter_name}, 0)) != T_FIXNUM)"
				end

				def test_if_not_2darray
					"TYPE(#{parameter_name}) != T_ARRAY || (RARRAY_LEN(#{parameter_name}) > 0 && TYPE(rb_ary_entry(ary, 0))) != T_ARRAY || (RARRAY_LEN(#{parameter_name}) > 0 && RARRAY_LEN(rb_ary_entry(#{parameter_name}, 0)) > 0 && TYPE(rb_ary_entry(rb_ary_entry(#{parameter_name}, 0), 0)) != T_FLOAT && TYPE(rb_ary_entry(rb_ary_entry(#{parameter_name}, 0), 0)) != T_FIXNUM)"
				end

				def comment_about_check
					"// Checking whether #{parameter_name} is a#{@type.to_s =~ /^[aeiou]/ ? "n" : ""} \"#{@type}\""
				end

				def check
					case @type
					when :double
"""
	#{comment_about_check}
	if (TYPE(#{parameter_name}) != T_FIXNUM && TYPE(#{parameter_name}) != T_FLOAT)
		rb_raise(rb_eArgError, \"Argument #{@position+1} must be a Float.\");"""
					when *IntegerTypes
"""
	#{comment_about_check}
	if (TYPE(#{parameter_name}) != T_FIXNUM && TYPE(#{parameter_name}) != T_FLOAT)
		rb_raise(rb_eArgError, \"Argument #{@position+1} must be an Integer.\");"""
					when *ArrayTypes
"""
	#{comment_about_check}
	if (#{test_if_not_1darray} && #{differs_from_classes CppClass::ArrayClasses})
		rb_raise(rb_eArgError, \"Argument #{@position+1} must be an ArrayType (\\\"#{(CppClass::ArrayClasses.map {|i| i.wrapped_class} + ["Array"]).join("\\\", \\\"")}\\\").\");"""
					when *MatrixTypes
"""
	#{comment_about_check}
	if (#{test_if_not_2darray} && #{differs_from_classes CppClass::MatrixClasses})
		rb_raise(rb_eArgError, \"Argument #{@position+1} must be an MatrixType (\\\"#{(CppClass::MatrixClasses.map {|i| i.wrapped_class} + ["Array< Array< Float > >"]).join("\\\", \\\"")}\\\").\");"""
					else# implement other checks as needs be.
						""
					end
				end

				def to_converted_form
					case @type
					when :double
						Converter.convert(parameter_name).from(Float).to("double").to_s
						#convert_from_double parameter_name
					when *IntegerTypes
						Converter.convert(parameter_name).from(Fixnum).to("int").to_s
						#convert_from_int parameter_name
					when *(MatrixTypes+ArrayTypes)
						if @input_class.nil? then raise RuntimeError.new "#{parameter_name} of type #{@type} was used in method \"#{@method.method_name}\" accessed via \"#{@method.cpp_method_name}\" before an input class was determined. (#{@method.header_file.inspect})" end
						Converter.convert(@input_class.converted_parameter_pointer(converted_parameter_name))
								 .from(@input_class.wrapped_class)
								 .to(@output_class.wrapped_class)
						#converted_parameter_object
					else # must've been converted beforehand...
						parameter_name
					end
				end

				def to_s
					to_converted_form
				end
			end
		end
	end
end