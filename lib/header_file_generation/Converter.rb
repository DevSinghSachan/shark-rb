module HeaderFileGenerator
	class HeaderFile
		class Converter

			def self.create_type_conversion conversion_function, type
				cpp_class = Method::CppClass.new(type)
				->(input_param_name, out_param_name=nil, indent=0, pointer=false) {
					if out_param_name
						"#{"\t"*indent}#{cpp_class.cpp_class} #{out_param_name} = #{conversion_function}(#{input_param_name});\n"
					else
						if pointer
							"&(#{conversion_function}(#{input_param_name}))"
						else
							"#{conversion_function}(#{input_param_name})"
						end
					end
				}
			end

			def self.create_cast_conversion type
				cpp_class = Method::CppClass.new(type)
				->(input_param_name, out_param_name=nil, indent=0, pointer=false) {
					if out_param_name
						puts "3"
						"#{"\t"*indent}#{cpp_class.cpp_class} #{out_param_name} = #{input_param_name};"
					else
						if pointer
							"&(#{input_param_name})"
						else
							"#{input_param_name}"
						end
					end
				}
			end

			def self.create_wrapped_class_conversion conversion_function, type
				cpp_class = Method::CppClass.new(type)
				->(input_param_name, out_param_name=nil, indent=0, pointer=false) {
					if out_param_name
						puts "2"
						"#{"\t"*indent}#{cpp_class.pointer} #{out_param_name} = #{input_param_name}->#{conversion_function}();"
					else
						if pointer
							"&(#{input_param_name}->#{conversion_function}())"
						else
							"#{input_param_name}->#{conversion_function}()"
						end
					end
				}
			end

			def self.create_ruby_conversion pointer_name
				rb_cpp_class = Method::CppClass.new(pointer_name)
				->(input_param_name, out_param_name, indent=0, pointer=false) {
"""
#{"\t"*indent}#{rb_cpp_class.pointer} #{out_param_name};
#{"\t"*indent}Data_Get_Struct(#{input_param_name}, #{rb_cpp_class}, #{out_param_name});
"""				
				}
			end

			def self.create_combined_conversion conversion_a, conversion_b
				->(input_param_name, out_param_name, indent=0, pointer=false) {
					puts "using combined conversion"
					(
						conversion_a.call(input_param_name, out_param_name+"_temp", indent) +
						conversion_b.call(out_param_name+"_temp", out_param_name, indent)
					)
				}
			end

			Conversions = {}

			Conversions["Array"] = {
					"RealVector"          => create_type_conversion("rb_ary_to_1d_realvector", "RealVector"),#rb_ary_to_1d_realvector"
					"RealMatrix"          => create_type_conversion("rb_ary_to_realmatrix", "RealMatrix"),#rb_ary_to_realmatrix"
					"std::vector<double>" => create_type_conversion("rb_ary_to_stdvector", "std::vector<double>") #rb_ary_to_stdvector"
				}
			Conversions["RealVector"] = {
					"std::vector<double>" => create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>"),# "realvector_to_stdvectordouble"
					"Array"               => create_type_conversion("realvector_to_rb_ary", "VALUE")#"realvector_to_rb_ary"
				}
			
			Conversions["RealMatrixRow"] = {
					"std::vector<double>" => create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>"),#"realvector_to_stdvectordouble",
					"Array"      => create_type_conversion("realvector_to_rb_ary", "VALUE") #"realvector_to_rb_ary"
				}
			Conversions["RealMatrixColumn"] = {
					"std::vector<double>" => create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>"),#"realvector_to_stdvectordouble",
					"Array"      => create_type_conversion("realvector_to_rb_ary", "VALUE") #"realvector_to_rb_ary"
				}
			Conversions["RealMatrix"] = {
					"Array"      => create_type_conversion("realvector_to_rb_ary", "VALUE") #"realmatrix_to_rb_ary"
				}
			Conversions["Float"] = {
					"double"     => create_type_conversion("NUM2DBL","double"),
					"int"        => create_type_conversion("NUM2INT","int")
				}
			Conversions["Fixnum"] = {
					"double"     => create_type_conversion("NUM2DBL","double"),
					"int"        => create_type_conversion("NUM2INT","int")
				}
			Conversions["double"] = {
					"Float"      => create_type_conversion("rb_float_new", "VALUE"),
					"Fixnum"     => create_type_conversion("INT2FIX", "VALUE")
				}
			Conversions["int"] = {
					"Float"      => create_type_conversion("rb_float_new", "VALUE"),
					"Fixnum"     => create_type_conversion("INT2FIX", "VALUE")
				}
			Conversions["VALUE"] = {
				"rb_RealVectorReference" => create_ruby_conversion("rb_RealVectorReference"),
				"rb_RealVector"          => create_ruby_conversion("rb_RealVector"),
				"rb_RealMatrixColumn"    => create_ruby_conversion("rb_RealMatrixColumn"),
				"rb_RealMatrixRow"       => create_ruby_conversion("rb_RealMatrixRow")
			}
			Conversions["rb_RealVector"] = {
				"RealVector" => create_ruby_conversion("rb_RealVector"),
				"std::vector<double>" => create_combined_conversion(
					create_ruby_conversion("rb_RealVector"),
					Conversions["RealVector"]["std::vector<double>"])
			}
			Conversions["rb_RealVectorReference"] = {
				"RealVector" => create_wrapped_class_conversion("getData","RealVector"),
				"std::vector<double>" => create_combined_conversion(
					create_ruby_conversion("rb_RealVectorReference"),
					Conversions["RealVector"]["std::vector<double>"])
			}
			Conversions["rb_RealMatrixRow"] = {
				"RealVector" => create_wrapped_class_conversion("getData","RealVector"),
				"std::vector<double>" => create_combined_conversion(
					create_ruby_conversion("rb_RealMatrixRow"),
					Conversions["RealVector"]["std::vector<double>"])
			}
			Conversions["rb_RealMatrixColumn"] = {
				"RealVector" => create_wrapped_class_conversion("getData","RealVector"),
				"std::vector<double>" => create_combined_conversion(
					create_ruby_conversion("rb_RealMatrixColumn"),
					Conversions["RealVector"]["std::vector<double>"])
			}

			def self.create_castable_equivalences *equivs
				equivs.each do |equiv|
					equivs.each do |equiv_other|
						Conversions[equiv][equiv_other] = create_cast_conversion(equiv_other)
					end
				end
			end

			create_castable_equivalences "RealVector", "RealMatrix", "RealMatrixColumn", "RealMatrixRow"

			def self.converts_to(typeName)
				Conversions.fetch(typeName).keys
			rescue KeyError
				raise NotImplementedError.new "No compatible classes for #{typeName}."
			end

			def initialize(variable_name="", converter=true)
				if !converter
					@origin = variable_name
					@converter = converter
				else
					@name = variable_name
					@converter = converter
				end
			end

			def self.can_convert(a)
				self.new a, false
			end

			def self.convert(variable_name="")
				self.new variable_name
			end

			def to_s_if_ready!
				if @origin.nil? || @destination.nil? then self
				else to_s end
			end

			def from(type)
				if !@converter then raise ArgumentError.new "Can't since it already has an origin type defined, #{@origin.cpp_class}." end
				@origin = type
				to_s_if_ready!
			end

			def to_s
				if @origin.nil? || @destination.nil? then raise ArgumentError.new "Converter needs an origin and destination type: call #from and #to." end
				if @origin == @destination then @name
				else
					Conversions.fetch(@origin.to_s).fetch(@destination.to_s).call(@name,nil,0,(@destination.is_a? Method::CppClass) ? @destination.pointer? : nil)
				end
			rescue KeyError
				raise NotImplementedError.new "No conversion from #{@origin} to #{@destination}."
			end

			def to(type)
				if !@converter
					@destination = type
					# puts "@origin.to_s => #{@origin.to_s}"
					origin_type = Conversions[@origin.to_s]
					if origin_type
						# puts "@destination.to_s => #{@destination.to_s}"
						if origin_type[@destination.to_s]
							true
						else
							false
						end
					else
						false
					end
				else
					@destination = type
					to_s_if_ready!
				end
			end
		end
	end
end