require_relative 'HeaderFile'
require_relative 'InputClass'
require_relative 'Input'
require_relative 'Method'

require 'json'
module HeaderFileGenerator
	# TODO: add shark error catching.

	def self.generate_header_files

		headers = []
		header_init_functions = []

		Dir.glob(File.dirname(__FILE__) + "/header_file_specs/*.json") do |file|
			begin
				hf = HeaderFile.new JSON.parse(File.read(file))
			rescue JSON::ParserError => e
				raise StandardError.new "Error parsing JSON \"#{file}\"\n\n=>\t"+e.message+"\n\n"
			end
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
			f << "void Init_Generated_Headers() {\n"
			f << ("\t" + header_init_functions.map {|i| i + "();"}.join("\n\t") + "\n")
			f << "}"
		end

		headers
	end
end