module HeaderFileGenerator
	class HeaderFile
		class Method
			class CppClass
				attr_reader :type
				
				class ClassTest
					def initialize opts={}
						@success = opts[:success].nil? ? true : opts[:success]
						@name = opts[:name]
						@comparator = opts[:comparator]
						@tester = opts[:tester] || "CLASS_OF"
					end
					def to_s
						"#{@tester}(#{@name}) #{@success ? "==" : "!="} #{@comparator.rb_class}"
					end
				end

				ArrayTypes   = [:array, :"1darray", :vector]
				MatrixTypes  = [:matrix, :"2darray"]
				IntegerTypes = [:integer, :int]

				def self.guess_from_type type
					type.match(/([^\*]+)(\*)?/)
					case $1.downcase.to_sym
					when *MatrixTypes
						CppClass.new("RealMatrix", :pointer => !$2.nil?)
					when *ArrayTypes
						CppClass.new("RealVector", :pointer => !$2.nil?)
					when *IntegerTypes
						CppClass.new("int", :pointer => !$2.nil?)
					when :double
						CppClass.new("double", :pointer => !$2.nil?)
					when :"std::vector<double>"
						CppClass.new("std::vector<double>", :pointer => !$2.nil?)
					else
						raise NotImplementedError.new "The type \"#{$1}\"#{!$2.nil? ? " (pointer)" : ""} has no equivalent C++ class yet."
					end
				end

				def self.can_convert_to cpp_class
					AllClasses.select {|i| Converter.can_convert(i).to cpp_class}
				end

				def initialize(typeName, opts={})
					@pointer = opts[:pointer].nil? ? false : opts[:pointer]
					@type = typeName
				end

				MatrixClasses = [
					CppClass.new("rb_RealMatrix"),
					CppClass.new("rb_RealMatrixReference")
				]

				DoubleClasses = [
					CppClass.new("double")
				]

				IntegerClasses = [
					CppClass.new("int")
				]

				RubyIntegerClasses = [
					CppClass.new("Fixnum")
				]

				RubyDoubleClasses = [
					CppClass.new("Float")
				]

				ArrayClasses = [
					CppClass.new("rb_RealVector"),
					CppClass.new("rb_RealVectorReference"),
					CppClass.new("rb_RealMatrixColumn"),
					CppClass.new("rb_RealMatrixRow")
				]

				AllClasses = (ArrayClasses + IntegerClasses + DoubleClasses + MatrixClasses + RubyIntegerClasses + RubyDoubleClasses)

				def ===(other)
					@type == other.type and @pointer == other.pointer?
				end

				def ==(other)
					@type == other.type and @pointer == other.pointer?
				end

				def self.sample typeName
					cpp_class = typeName.is_a?(CppClass) ? typeName : CppClass.new(typeName)
					case cpp_class
					when *IntegerClasses
						"#{cpp_class}(#{Random.rand(11)})"
					when *RubyIntegerClasses
						"INT2FIX(#{Random.rand(11)})"
					when *RubyDoubleClasses
						"rb_float_new(#{Random.rand(5.0).round 3})"
					when *DoubleClasses
						"#{cpp_class}(#{Random.rand(5.0).round 3})"
					when *(ArrayClasses.map {|i| CppClass.new(i.wrapped_class)})
						"new #{cpp_class}()"
					when *(MatrixClasses.map {|i| CppClass.new(i.wrapped_class)})
						"new #{cpp_class}()"
					else
						raise NotImplementedError.new "#{typeName} does not have a sample element (yet)."
					end
				end

				def rb_class
					check_ruby!
					@type + "::rb_class()"
				end

				def pointer?
					@pointer
				end

				def pointer
					@type + " *"
				end

				def ruby?
					@type =~ /rb_(.+)/
				end

				def check_ruby!
					if !ruby? then raise NotImplementedError.new "#{@type} does not have a Ruby class (yet)." end
				end

				def wrapped_class
					if @type =~ /rb_(.+)/ and @type != "rb_RealVectorReference"
						$1
					elsif @type == "rb_RealVectorReference"
						"RealVector"
					else
						cpp_class
					end
				end

				def to_rb varName=""
					check_ruby!
					if @pointer
						"wrap_pointer<#{cpp_class}>(#{rb_class}, new #{cpp_class}(&(#{varName})))"
					else
						"wrap_pointer<#{cpp_class}>(#{rb_class}, new #{cpp_class}(#{varName}))"
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

				def class_tester
					check_ruby!
					"CLASS_OF"
				end

				def test_class variable_name, success=true
					ClassTest.new(:name => variable_name, :success => success, :comparator => self, :tester => class_tester)
				end

				def converted_parameter_pointer variableName
					"*(#{variableName}->#{wrapped_class_pointer})"
				end

				def cpp_class
					@type
				end

				def convert_into_class input, indent=0
					if ruby?
						begin
							Converter::Conversions["VALUE"].fetch(self.to_s).call(input.parameter_name,input.converted_parameter_name,indent)
						rescue KeyError
							raise NotImplementedError.new "No conversion from VALUE to #{self}."
						end
					else
						# use other methodology here.
						(
							"\n#{"\t"*indent}#{pointer} #{input.converted_parameter_name} = " +
							Converter.convert(input.parameter_name).from(input).to(input.output_class) +
							";\n"
						)
					end
				end
			end
			class RubyArray < CppClass

				def initialize type="RealVector"
					@type = type
				end

				def class_tester
					"TYPE"
				end

				def to_rb varName=""
					varName
				end

				def convert_into_class input, indent=0
					begin
						Converter::Conversions["Array"].fetch(cpp_class).call(input.parameter_name,input.converted_parameter_name,indent)
					rescue KeyError
						raise NotImplementedError.new "No conversion from Array to #{cpp_class}."
					end
				end

				def rb_class
					"T_ARRAY"
				end

				def pointer
					raise NotImplementedError.new "#{@type} does not use pointers (often)."
				end

				def converted_parameter_pointer variableName
					"#{variableName}" # no need to use a pointer with a RubyArray.
				end

				def wrapped_class_pointer
					raise NotImplementedError.new "#{@type} is a Ruby C Object and has no class methods."
				end
			end
			CppClass::RubyArray  = RubyArray.new
			CppClass::Ruby2DArray = RubyArray.new "RealMatrix"
			CppClass::AllClasses << CppClass::RubyArray
			CppClass::AllClasses << CppClass::Ruby2DArray
		end
	end
end