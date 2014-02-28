class String
	def red
		"\e[0;31;49m#{self}\e[0m\n"
	end

	def green
		"\e[0;32;49m#{self}\e[0m\n"
	end

	def grey
		"\e[0;97;49m#{self}\e[0m"
	end
end

module HeaderFileGenerator
	class HeaderFile
		module CppChecker

			def test_cpp_methods
				require 'mkmf'
				opts = ""
				logfile = File.join(File.dirname(__FILE__) + "/../../mkmf.log")
				File.open(File.dirname(__FILE__) +"/../../ext/Makefile").each_line do |line|
					if line =~ /^LIBS = (?:\$\([^\)]+\))? (.+)/
						opts = $1
						break
					end
				end

				old_cpp_config = RbConfig::CONFIG["CPP"]

				if old_cpp_config =~ /(.+)-E$/
					RbConfig::CONFIG["CPP"] = $1
				end

				opts += " -x c++ -I./ext"

				(@methods + @setters + @getters).select {|i| i.class != HeaderFile::Allocator and i.class != HeaderFile::Initializer}.each_with_index do |method,k|
					begin
						print "\e[0;95;49mTesting\e[0m (\e[0;34;49m#{method.class.to_s.split("::").last}\e[0m) #{method.cpp_class}##{method.cpp_method_name}(#{method.parameters.map {|i| i.compatible_classes.first.cpp_class}.join(", ")}) "
						t1 = Time.now
						if try_cpp(test_existence_of_method(method), opts)
							print " OK".green
						else
							print " FAIL".red
							if File.exists? logfile
								logcontents = File.open(File.dirname(__FILE__) + "/../../mkmf.log").read
								relevant_lines = []
								logcontents.split("\n").reverse.each do |line|
									if line[opts]
										break
									elsif line =~ /\/\*/
										relevant_lines << line.grey
									elsif line =~ /(.+)(\d+:\d+): (error:)( .+)/
										relevant_lines << ($1.grey+ " " + $2 + " " + $3.red + " " + $4)
									elsif line =~ /(.+ )(warning:)( .+)/
										relevant_lines << "\e[0;97;49m#{$1}\e[0m\e[0;33;49m#{$2}\e[0m#{$3}"
									else
										relevant_lines << line
									end
								end
								puts relevant_lines.reverse
							end
						end
					rescue NoMethodError => e
						missing_cpp_classes = method.parameters.select {|i| i.compatible_classes.length == 0}.map {|i| "#{i.type} (#{i.position+1})"}
						if missing_cpp_classes.empty?
							raise e
						else
							print " \e[0;31;49mFAIL\e[0m\n"
							puts "Missing C++ class for parameters to #{method.cpp_class}##{method.cpp_method_name} => #{missing_cpp_classes.join(", ")}"
						end
					end
				end
				RbConfig::CONFIG["CPP"] = old_cpp_config
			
			end

			def test_existence_of_method method

				parameters = method.parameters
				method.parameters.each do |p|
					p.input_class = Method::CppClass.new(p.compatible_classes.first.wrapped_class)
				end
"""
#{h_file_dependencies}
#{include_header_file}
#{include_pointer_wrapper_extras}
#{method.to_cpp_function_definition}"""

# 				cpp = """
# #{h_file_dependencies}

# int checker (#{parameters.map {|i| i.input_class.cpp_class + " "+ i.parameter_name}.join(", ")}) {
# 	#{@wrapped_class} #{method.symbol}(#{@default_constructor_arguments.join(", ")});
# """
# 				# define parameters
# 				if method.cpp_method_name =~ /operator(?:\s*)(.+)/
# 					if $1 == "()"
# 						cpp += "\t#{method.symbol}(#{parameters.map {|i| Converter.convert(i.parameter_name).from(i.input_class).to(i.output_class)}.join(", ")});\n"
# 					else
# 						cpp += "\t#{method.symbol}#{$1} #{parameters.map {|i| Converter.convert(i.parameter_name).from(i.input_class).to(i.output_class)}.join(", ")};\n"
# 					end
# 				else
# 					cpp += "\t#{method.symbol}.#{method.cpp_method_name}(#{parameters.map {|i| Converter.convert(i.parameter_name).from(i.input_class).to(i.output_class)}.join(", ")});\n"
# 				end
# 				cpp += "\treturn 1;\n}\n"
# 				cpp += """
# int main () {
# 	return checker(#{parameters.map {|i| HeaderFileGenerator::HeaderFile::Method::CppClass.sample(i.input_class)}.join(", ")});
# }
# """
# 				cpp

			end
		end
	end
end