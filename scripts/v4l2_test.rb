#!/usr/bin/env ruby

require_relative "kradradio_client.rb"

station_name = "v4l2test"

width = 640
height = 360
fps = "30000 1001"
device = "/dev/video0"

station = KradStation.new(station_name)

dir = "~/Videos/#{station_name}"

`mkdir -p #{dir}`
station.cmd("setdir #{dir}")

#station.cmd("res #{width} #{height}")
#station.cmd("fps #{fps}")

puts station.cmd("info")
puts station.cmd("adapters")
puts station.cmd("comp")

station.cmd("capture v4l2 #{device} #{width} #{height}")
#station.cmd("transmitter_on 3030")
#station.cmd("display")

puts station.cmd("comp")
puts station.cmd("lc")
station.cmd("snap")

sleep 2

puts station.cmd("info")
puts station.cmd("adapters")
puts station.cmd("comp")
puts station.cmd("lc")
station.cmd("snap")

sleep 2

logname = station.cmd("logname")

station.cmd("destroy")

puts `tail -n 1000 #{logname}`

puts dir
cmd = "ls -ltrh #{dir}"
puts `#{cmd}`
