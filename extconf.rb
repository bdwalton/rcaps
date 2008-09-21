#!/usr/bin/env ruby

require 'mkmf'

dir_config("rcaps")

if find_library("cap", "cap_init", "/usr/lib")
  create_makefile("rcaps")
else
  puts "No libcap found..."
end
