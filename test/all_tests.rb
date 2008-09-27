#!/usr/bin/ruby -w
#
require 'test/unit'

Dir.glob("t_*rb") do |f|
  require "#{f.sub(/\.rb/, '')}"
end
