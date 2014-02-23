require_relative 'HeaderFile'
require_relative 'Method'
require 'json'
module HeaderFileGenerator

	def self.generate_header_files

		headers = []
		header_init_functions = []

		Dir.glob(File.dirname(__FILE__) + "/header_file_specs/*.json") do |file|
			hf = HeaderFile.new JSON.parse(File.read(file))
			File.open(File.dirname(__FILE__) + "/../../ext/" + hf.cpp_class_name+".h", "w") do |f|
				f << hf.to_h_file
			end
			File.open(File.dirname(__FILE__) + "/../../ext/" + hf.cpp_class_name+".cpp", "w") do |f|
				f << hf.to_cpp_file
			end
			headers << (hf.cpp_class_name+".h")
			header_init_functions << hf.init_function_name
		end

		File.open(File.dirname(__FILE__) + "/../../ext/extras/utils/rb_generated_headers.extras", "w") do |f|
			f << (headers.map {|i| "#include \"#{i}\""}.join("\n") + "\n")
			f << "void Init_Generated_Headers() {\n\t"
			f << (header_init_functions.map {|i| i + "();"}.join("\n\t") + "\n")
			f << "}"
		end

	end
end