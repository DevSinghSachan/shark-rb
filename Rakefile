begin
	require 'jeweler'
	Jeweler::Tasks.new do |gemspec|
		gemspec.name     = "shark-rb"
		gemspec.summary  = "Shark C++ library bindings in Ruby."
		gemspec.email    = "jraiman@mit.edu"
		gemspec.homepage = "http://github.com/JonathanRaiman/shark_rb"
		gemspec.authors  = ["Jonathan Raiman"]
		# gemspec.platform = Gem::Platform::CURRENT
		gemspec.description = "Shark C++ library bindings in Ruby implementing RealVector, LabeledData, UnlabeledData, neural nets and much more!"
		gemspec.license = "MIT"
	end
rescue LoadError
	puts "Jeweler not available. Install it with: gem install jeweler"
end

require './lib/header_file_generation.rb'

desc 'Generate Header Files'
task :header do
	SharkHeaderFileGenerator.generate_header_files
end
