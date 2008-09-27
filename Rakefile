require 'rubygems'
Gem::manage_gems
require 'rake/gempackagetask'
require 'rake/clean'

CLEAN.include("pkg", "doc")

spec = Gem::Specification.new do |s|
  s.name		= "rcaps"
  s.version		= "0.8"
  s.author		= "Ben Walton"
  s.email		= "bdwalton@gmail.com"
  s.homepage		= "http://rcaps.rubyforge.org/"
  s.rubyforge_project	= "http://rubyforge.org/projects/rcaps"
  s.summary    		= "A library for manipulating capabilities using the POSIX 1003.1e interfaces"
  s.date		= '2008-09-26'
  s.extensions		= ['extconf.rb']
  s.files		= %w{COPYING gpl.txt rcaps.c rcaps.h}
  s.require_path	= 'lib'
  s.has_rdoc		= true
#s.rdoc_options	= %(-x tests/)
#s.test_file		= 'tests/all_tests.rb'
end

task :default => [:package]

Rake::GemPackageTask.new(spec) do |pkg|
    pkg.need_tar = true
end

