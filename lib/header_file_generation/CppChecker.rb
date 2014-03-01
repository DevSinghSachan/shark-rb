class String
	def red
		"\e[0;31;49m#{self}\e[0m\n"
	end

	def green
		"\e[0;32;49m#{self}\e[0m\n"
	end

	def yellow
		"\e[0;33;49m#{self}\e[0m\n"
	end

	def white
		"\e[0;97;49m#{self}\e[0m"
	end

	def grey
		"\e[37m#{self}\e[39;49;00m"
	end
end

module HeaderFileGenerator
	class HeaderFile
		module CppChecker

			def test_cpp_methods
				require 'mkmf'
				require 'pygments'
				opts = ""
				logfile = File.join(File.dirname(__FILE__) + "/../../mkmf.log")
				File.open(File.dirname(__FILE__) +"/../../ext/Makefile").each_line do |line|
					if line =~ /^LIBS = (?:\$\([^\)]+\))? (.+)/
						opts = $1
						break
					end
				end

				old_cpp_config = RbConfig::CONFIG["CPP"]
				
				# -E        will only preprocess file, running without -E means actually
				#           compiling (and thus error-checking)
				# -x <type> specifies whether file is c, c++, objective-c
				#
				# -c        specifies whether to perform linking or not on compiled
				#           objects
				#
				if old_cpp_config =~ /(.+)-E$/
					RbConfig::CONFIG["CPP"] = $1
				end


				opts += " -x c++ -I./ext -c"

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
										relevant_lines << ($1.grey + $2.yellow + $3)
									elsif line =~ /(\s*\d+: )(.*)/
										relevant_lines << ($1.grey + Pygments.highlight($2, :formatter => 'terminal', :lexer => "c++"))
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
				# method.parameters.each do |p|
				# 	p.input_class = Method::CppClass.new(p.compatible_classes.first.wrapped_class)
				# end
"""
#{h_file_dependencies}
#{include_header_file}
#{include_pointer_wrapper_extras}
#{method.to_cpp_function_definition}"""

			end
		end
	end
end