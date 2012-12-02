#!/usr/bin/env ruby

require_relative "kradradio_client.rb"

width  = 1280
height = 720
fps    = 15
bitrate = 1200
codec = "vp8vorbis"

station_name = "djsh2"
station = KradStation.new (station_name)

station.cmd("setdir \"#{File.expand_path('~')}/krad_stations\"")
sleep 2

system("xterm -e tail -n1000 -f `krad_radio #{station_name} logname` 2>&1 >/dev/null &")

puts station.info
puts station.uptime

station.set_resolution(width, height)
#station.cmd("fps #{fps}")


station.cmd("record av \"#{File.expand_path('~')}/PublicHtml/krad_live.webm\" #{codec} #{width} #{height} #{bitrate} 48000")

#sleep 3
#station.cmd("transmitter_on 3030")
#sleep 1

#station.cmd("input Mic")
#station.cmd("transmit av transmitter 3030 /video.webm nopass #{codec} #{width} #{height} #{bitrate} 48000");
#sleep 3


station.play("#{File.expand_path('~')}/Development/krad_radio_oneman/epII-veritas-proof.ogv")
sleep 0.25
station.cmd("setport 1 0 80 320 240 0 0 320 240 1 0")

#krad_ipc_compositor_set_port_mode (krad_ipc_client_t *client, int number, uint32_t x, uint32_t y,
#				    uint32_t width, uint32_t height, uint32_t crop_x, uint32_t crop_y,
#				    uint32_t crop_width, uint32_t crop_height, float opacity, float rotation)
#station.cmd("capture v4l2") 


station.play("#{File.expand_path('~')}/Videos/holy_crap.ogv")
sleep 0.25
station.cmd("setport 2 360 80 320 240 0 0 320 240 1 0")

station.cmd("hex 340 220 20")


#station.cmd("bug 170 160 \"#{File.expand_path('~')}/Pictures/six600110.png\"")
sleep 10

station.cmd("rm 1")
station.cmd("rm 2")
station.cmd("destroy")
exit 0


#	def sprite(filename, x=0, y=0, rate=5, scale=1, opacity=1.0, rotation=0.0)
#		self.cmd("addsprite \"#{filename}\" #{x} #{y} #{rate} #{scale} #{opacity} #{rotation}")
#	end
	
#	def set_sprite(num=0, x=0, y=0, rate=5, scale=1, opacity=1.0, rotation=0.0)
#		self.cmd("setsprite #{num} #{x} #{y} #{rate} #{scale} #{opacity} #{rotation}")
#	end
	
#	def rm_sprite(num=0)
#		self.cmd("rmsprite #{num}")
#	end


