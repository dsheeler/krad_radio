#!/usr/bin/env ruby

require_relative "kradradio_client.rb"

width = 1280
height = 720
bitrate = 1200


station_name = "djsh"
station = KradStation.new (station_name)

puts station.info
puts station.uptime

station.set_resolution(width, height)
station.cmd("fps 30")

#void krad_ipc_create_record_link (krad_ipc_client_t *client, krad_link_av_mode_t av_mode, char *filename, char *codecs,
#			           int video_width, int video_height, int video_bitrate, char *audio_bitrate) {
#
#if (argc == 10) {
#  krad_ipc_create_record_link (client, krad_link_string_to_av_mode (argv[3]), argv[4], argv[5],
#                               atoi(argv[6]), atoi(argv[7]), atoi(argv[8]), argv[9]);					
#}			
#
#Command: record:
# record av_mode "filename" codecs width height bitrate audio_bitrate 
#   av_mode: ( av, audiovideo, audio and video, audio_and_video ) or ( video, videoonly, video only, video_only ) or 
#            ( anything else defaults to audio )
#   codecs: vp8, vorbis, ...
#													
#station.cmd("record video \"#{File.expand_path('~')}/Videos/krad_title_sequence.webm\" \"vp8\" #{width} #{height} #{bitrate}")

#station.record("#{File.expand_path('~')}/PublicHtml/krad_title_sequence.webm", {"codec" => "vp8vorbis"})#, 'width' => "#{width}", 'height' => "#{height}", 'bitrate' => "#{bitrate}", 'audiobitrate' => "48000"})

station.cmd("record av \"#{File.expand_path('~')}/PublicHtml/krad_title_sequence.webm\" \"vp8 vorbis\" #{width} #{height} #{bitrate} 48000")

station.cmd("xmms2 XMMS2 play")

#text(thetext, x=0, y=0, rate=5, scale=32, opacity=1.0, rotation=0.0, red=0, green=0, blue=0, font="sans")
station.text("KRAD RADIO", 2000, 355, 5, 174, 1, 0, 255, 11, 2)
sleep 0.25

#set_text(num=0, x=0, y=0, rate=5, scale=32, opacity=1.0, rotation=0.0, red=0, green=0, blue=0)
station.set_text(0, 25, 355, 150, 174, 1, 0, 255, 11, 2)
sleep 2.5
station.set_text(0, 25, 355, 5, 174, 0, 0, 255, 11, 2)
sleep 2.5
station.rm_text(0)

station.text("Written, Directed, Produced by, and Starring...", 2000, 155, 5, 50, 1, 0, 250, 250, 250)
sleep 1.0

station.set_text(0, 25, 155, 150, 50, 1, 0, 250, 250, 250)
sleep 2.0

station.text("oneman", 55, 455, 5, 120, 1, 0, 250, 250, 250)
sleep 0.5
	
station.sprite("#{File.expand_path('~')}/Pictures/oneman.png", 600, 255, 10, 1, 1.0, 3600.0)
sleep 2.0
station.set_sprite(0, 600, 255, 50, 1, 1.0, 0.0)
sleep 4.0
station.throw_sprite(0)
station.rm_text(1)
station.rm_text(0)
sleep 2.5

station.rm_sprite(0)



station.text("Produced by and Featuring...", 2000, 155, 5, 50, 1, 0, 250, 250, 250)
sleep 1.0

station.set_text(0, 25, 155, 150, 50, 1, 0, 250, 250, 250)
sleep 2.0

station.text("six600110", 55, 455, 5, 80, 1, 0, 250, 250, 250)
sleep 0.5

station.sprite("#{File.expand_path('~')}/Pictures/six600110.png", 600, 215, 10, 1, 1.0, 3600.0)

sleep 2.0
station.set_sprite(0, 600, 215, 50, 1, 1.0, 0.0)
sleep 4.0
station.throw_sprite(0)

station.rm_text(1)
station.rm_text(0)
sleep 2.5
station.rm_sprite(0)


station.play("#{File.expand_path('~')}/Videos/holy_crap.ogv")

sleep 4.0

station.text("Zooming in with audacity and friends", 2000, 155, 5, 50, 1, 0, 250, 250, 250)
sleep 1.0
station.set_text(0, 25, 155, 150, 50, 1, 0, 250, 250, 250)
sleep 10.0
station.rm_text(0)
sleep 60.0

station.cmd("rm 1")


sleep 4.0
station.text("Thanks for watching", 135, 655, 5, 100, 1, 0, 255, 11, 2, "DroidSans")
sleep 1
station.set_text(0, 1535, -155, 15, 4, 0, -50)
sleep 0.3
station.text("See you next time, space cowboy!", 135, 655, 5, 64, 1, 0, 125, 155, 25, "Georgia")
station.set_text(1, 135, 655, 5, 64, 1, 0, 25, 255, 225)
sleep 1
station.set_text(0, 135, 655, 25, 64, 0, 0)
sleep 1
station.set_text(1, 135, 655, 25, 64, 1, 0, 255, 1, 1)
sleep 1
station.set_text(1, 135, 655, 25, 64, 1, 0, 255, 1, 1)
station.rm_text(0)
sleep 1
station.set_text(1, 135, 655, 25, 4, 0, 0, 255, 1, 1)
sleep 1
station.rm_text(1)
station.cmd("xmms2 XMMS2 stop")
#station.cmd("rm 0")
#station.cmd("destroy")
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


