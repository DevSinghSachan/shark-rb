module HeaderFileGenerator
	class HeaderFile
		class Converter
			Conversions = {}

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

			def self.add_conversion_from type_name
				ac = ->(other_type, conversion){
					if Conversions[type_name].nil? then Conversions[type_name] = {} end
					Conversions[type_name][other_type] = conversion
				}
				def ac.to location
					expr = self
					act = ->(conversion){
						expr.call(location, conversion)
					}
					def act.using conversion_type
						if !conversion_type.is_a?(Proc) || conversion_type.parameters.length != 4 then raise ArgumentError.new "A conversion must be a Proc with 4 arguments." end
						self.call(conversion_type)
						true
					end
					act
				end
				ac
			end

			add_conversion_from("Array").to("RealVector").using create_type_conversion("rb_ary_to_1d_realvector", "RealVector")
			add_conversion_from("Array").to("RealMatrix").using create_type_conversion("rb_ary_to_realmatrix", "RealMatrix")
			add_conversion_from("Array").to("std::vector<double>").using create_type_conversion("rb_ary_to_stdvector", "std::vector<double>")

			add_conversion_from("RealVector").to("std::vector<double>").using create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>")
			add_conversion_from("RealVector").to("Array").using create_type_conversion("realvector_to_rb_ary", "VALUE")

			add_conversion_from("RealMatrixRow").to("std::vector<double>").using create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>")
			add_conversion_from("RealMatrixRow").to("Array").using create_type_conversion("realvector_to_rb_ary", "VALUE")

			add_conversion_from("RealMatrixColumn").to("std::vector<double>").using create_type_conversion("realvector_to_stdvectordouble", "std::vector<double>")
			add_conversion_from("RealMatrixColumn").to("Array").using create_type_conversion("realvector_to_rb_ary", "VALUE")
	
			add_conversion_from("RealMatrix").to("Array").using create_type_conversion("realvector_to_rb_ary", "VALUE")

			add_conversion_from("Float").to("double").using create_type_conversion("NUM2DBL","double")
			add_conversion_from("Float").to("int").using create_type_conversion("NUM2INT","int")

			add_conversion_from("Fixnum").to("double").using create_type_conversion("NUM2DBL","double")
			add_conversion_from("Fixnum").to("int").using create_type_conversion("NUM2INT","int")

			add_conversion_from("double").to("Float").using create_type_conversion("rb_float_new", "VALUE")
			add_conversion_from("double").to("Fixnum").using create_type_conversion("INT2FIX", "VALUE")

			add_conversion_from("int").to("Float").using create_type_conversion("rb_float_new", "VALUE")
			add_conversion_from("int").to("Fixnum").using create_type_conversion("INT2FIX", "VALUE")

			add_conversion_from("VALUE").to("rb_RealVectorReference").using create_ruby_conversion("rb_RealVectorReference")
			add_conversion_from("VALUE").to("rb_RealVector").using create_ruby_conversion("rb_RealVector")
			add_conversion_from("VALUE").to("rb_RealMatrixColumn").using create_ruby_conversion("rb_RealMatrixColumn")
			add_conversion_from("VALUE").to("rb_RealMatrixRow").using create_ruby_conversion("rb_RealMatrixRow")

			add_conversion_from("rb_RealVector").to("RealVector").using create_ruby_conversion("rb_RealVector")
			add_conversion_from("rb_RealVector").to("std::vector<double>").using create_combined_conversion(
					create_ruby_conversion("rb_RealVector"),
					Conversions["RealVector"]["std::vector<double>"])

			add_conversion_from("rb_RealVectorReference").to("RealVector").using create_ruby_conversion("rb_RealVectorReference")
			add_conversion_from("rb_RealVectorReference").to("std::vector<double>").using create_combined_conversion(
					create_ruby_conversion("rb_RealVectorReference"),
					Conversions["RealVector"]["std::vector<double>"])

			add_conversion_from("rb_RealMatrixRow").to("RealVector").using create_ruby_conversion("rb_RealMatrixRow")
			add_conversion_from("rb_RealMatrixRow").to("std::vector<double>").using create_combined_conversion(
					create_ruby_conversion("rb_RealMatrixRow"),
					Conversions["RealVector"]["std::vector<double>"])

			add_conversion_from("rb_RealMatrixColumn").to("RealVector").using create_ruby_conversion("rb_RealMatrixColumn")
			add_conversion_from("rb_RealMatrixColumn").to("std::vector<double>").using create_combined_conversion(
					create_ruby_conversion("rb_RealMatrixColumn"),
					Conversions["RealVector"]["std::vector<double>"])

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
				raise CppError.new "No compatible classes for #{typeName}."
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
				raise CppError.new "No conversion from #{@origin} to #{@destination}."
			end

			def to(type)
				if !@converter
					@destination = type
					origin_type = Conversions[@origin.to_s]
					if origin_type
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