require 'rake/hooks'
require 'git'

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

before 'install' do
	g = Git.open(File.dirname(__FILE__))
	modified_files = g.status.changed.keys + g.status.untracked.keys
	json_modified_files = modified_files.reject {|i| !(i.match(/header_file_specs\/[^\.]+.json/))}.map {|i| i.match(/header_file_specs\/([^\.]+.json)/)[1]}
	if json_modified_files.length > 0
		raise RuntimeError.new "Warning: Some new headers were not added to the git, and may not be ready:\n\t- \"#{json_modified_files.join("\",\n\t- \"")}\"\nRun \"rake header\" to confirm these new additions, or delete these files.\n\n"
	end
end



desc 'Generate Header Files'
task :header do
	require './lib/header_file_generation/header_file_generation.rb'
	hfiles = HeaderFileGenerator.generate_header_files.map {|i| File.dirname(__FILE__) + "/ext/" + i}

	json_spec_files = Dir.glob(File.dirname(__FILE__)+"/lib/header_file_generation/header_file_specs/*.json")
	if hfiles.length > 0 or json_spec_files.length > 0
		g = Git.open(File.dirname(__FILE__))
		if (g.status.untracked.keys.map {|i| File.dirname(__FILE__) + "/" + i} & hfiles).length > 0
			# add json descriptions
			g.add(json_spec_files)
			# add header files:
			g.add(hfiles)
			# add cpp files:
			g.add(hfiles.map {|i| i.match(/(.+\.)h/)[1] + "cpp"})
			begin
				g.commit_all("new header files")
			rescue Git::GitExecuteError => e
				if e.message["nothing to commit, working directory clean"]
					puts "No changes to commit."
				else
					raise e
				end
			end
			`cd #{File.dirname(__FILE__) + "/ext/"} && ruby extconf.rb && cd ..`
			puts "new header files, rebuilding"
			Rake::Task["clean"]
			Rake::Task["build"]
			Rake::Task["gemspec:generate"].reenable
			Rake::Task["gemspec:generate"].invoke
		elsif ((g.status.changed.keys + g.status.untracked.keys).map {|i| File.dirname(__FILE__) + "/" + i} & hfiles).length > 0
			# add json descriptions
			g.add(json_spec_files)
			# add header files:
			g.add(hfiles)
			# add cpp files:
			g.add(hfiles.map {|i| i.match(/(.+\.)h/)[1] + "cpp"})
			begin
				g.commit_all("modified json + header files")
			rescue Git::GitExecuteError => e
				if e.message["nothing to commit, working directory clean"] or e.message["nothing added to commit"]
					puts "No changes to commit."
				else
					raise e
				end
			end
			`cd #{File.dirname(__FILE__) + "/ext/"} && ruby extconf.rb && cd ..`
			Rake::Task["gemspec:generate"].reenable
			Rake::Task["gemspec:generate"].invoke
		end
	end
	
end
