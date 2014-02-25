module HeaderFileGenerator
	class HeaderFile
		attr_reader :getters
		attr_reader :setters
		attr_reader :cpp_class
		attr_reader :pointer_acquirer
		attr_reader :pointer_name

		def define_getters getter_list=[]
			if !getter_list.nil?
				getter_list.each do |s|
					@getters << Getter.new(s.merge({"hf" => self}))
				end
			end
		end

		def define_setters setter_list=[]
			if !setter_list.nil?
				setter_list.each do |s|
					@setters << Setter.new(s.merge({"hf" => self}))
				end
			end
		end

		def define_methods method_list=[]
			if !method_list.nil?
				method_list.each do |s|
					@methods << Method.new(s.merge({"hf" => self}))
				end
			end
		end

		def initialize(opts={})
			@setters, @methods, @getters   = [], [], []
			@wrapped_class                 = opts["wrapped_class"]
			@dependencies                  = opts["dependencies"]
			@pointer_acquirer              = [(opts["pointer_getter"] || "getModel")].flatten
			@pointer_name                  = opts["pointer_name"] || "model"
			raise StandardError.new "pointer_name: \"#{opts["pointer_name"] || "model"}\" cannot have the same name as one of the class methods (pointer_getter) : \"#{[(opts["pointer_getter"] || "getModel")].flatten.join("\", \"")}\"" if [(opts["pointer_getter"] || "getModel")].flatten.include?(opts["pointer_name"] || "model")
			@cpp_class                     = Method::CppClass.new opts["class"]
			@rb_class_name                 = opts["rb_class"]
			@default_constructor_arguments = opts["constructor_arguments"] || []
			# this is to define the function that returns the class and singleton methods too
			@methods << Allocator.new("hf" => self)
			@methods << Initializer.new("hf" => self)
			define_setters opts["setters"]
			define_getters opts["getters"]
			define_methods opts["methods"]
		end

		def init_function_name
			"Init_#{@cpp_class}"
		end

		def generate_init_function
			rb_function_definitions = []
			@setters.each do |setter|
				rb_function_definitions << ("\t"+setter.to_rb_function_definition)
			end
			@getters.each do |getter|
				rb_function_definitions << ("\t"+getter.to_rb_function_definition)
			end
			@methods.each do |method|
				rb_function_definitions << ("\t"+method.to_rb_function_definition)
			end
			func_definition = rb_function_definitions.join("\n")
"""
void #{init_function_name} () {
#{func_definition}
}"""
		end

		def include_header_file
			"#include \"#{@cpp_class}.h\""
		end

		def include_pointer_wrapper_extras
			"#include \"extras/utils/rb_pointer_wrapping.extras\""
		end

		def generate_cpp_pointer_acquirer_functions
			cpp = ""
			@pointer_acquirer.each do |pointer_acq|
cpp+= """
#{pointer_acquirer_return_type} #{@cpp_class}::#{pointer_acq}() {
	return &#{@pointer_name};
}
"""
			end
			cpp
		end

		def generate_cpp_constructor_function
			if @default_constructor_arguments
"""
#{@cpp_class}::#{@cpp_class}() : model(#{@default_constructor_arguments.join(", ")}) {}
"""	
			else
"""
#{@cpp_class}::#{@cpp_class}() {}
"""
			end
		end

		def generate_rb_class_function
"""
extern VALUE #{@rb_class_name};

VALUE #{@cpp_class.rb_class} {
	return #{@rb_class_name};
}
"""
		end

		def to_cpp_file
			cpp = ""
			cpp += include_header_file
			cpp += "\n"
			cpp += include_pointer_wrapper_extras
			cpp += "\n"
			cpp += generate_cpp_constructor_function
			cpp += generate_cpp_pointer_acquirer_functions
			cpp += generate_rb_class_function
			@setters.each do |setter|
				cpp += setter.to_cpp_function_definition
			end
			@getters.each do |getter|
				cpp += getter.to_cpp_function_definition
			end
			@methods.each do |method|
				cpp += method.to_cpp_function_definition
			end
			cpp += generate_init_function
			cpp
		end

		def h_file_dependencies
			@dependencies.map {|i| "#include "+i}.join("\n")
		end

		def pointer_acquirer_return_type
			"#{@wrapped_class} *"
		end

		def pointer_acquirer_h_definition
			cpp = ""
			@pointer_acquirer.each do |pointer_acq|
				cpp+="\t\t#{pointer_acquirer_return_type} #{pointer_acq}();\n"
			end
			cpp
		end

		def to_h_file
"""#ifndef #{@cpp_class.cpp_class.upcase}_H
#define #{@cpp_class.cpp_class.upcase}_H

#{h_file_dependencies}

class #{@cpp_class} {

	public:
		static VALUE rb_class();
		#{@wrapped_class}   #{@pointer_name};
#{pointer_acquirer_h_definition}
		#{@cpp_class}();
};

void #{init_function_name}();

#endif
"""
		end
	end
end