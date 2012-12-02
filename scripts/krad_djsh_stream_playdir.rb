#!/usr/bin/env ruby

require_relative "kradradio_client.rb"

width   = 640
height  = 360
bitrate = 900
fps     = 25
port    = 3030


station_name = "djsh"
station = KradStation.new (station_name)

puts station.info
puts station.uptime

station.set_resolution(width, height)
station.cmd("fps #{fps}")

station.cmd("transmitter_on #{port}")
#krad_radio $STATION transmit av transmitter 3030 /krad.webm nopass "vp8 vorbis" $width $height $bitrate
station.cmd("transmit av transmitter #{port} /krad_movie_stream.webm nopass \"vp8 vorbis\" #{width} #{height} #{bitrate}")

station.play_dir("#{File.expand_path('~')}/Videos/Playlist")



