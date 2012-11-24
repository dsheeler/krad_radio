#!/usr/bin/env ruby

require "/home/dsheeler/Development/krad_radio/scripts/kradradio_client.rb"

videodev = "/dev/video0"

#`v4l2-ctl --set-ctrl=gain=200 -d #{videodev}`
#`v4l2-ctl --set-ctrl=power_line_frequency=0 -d #{videodev}`
puts `v4l2-ctl --set-ctrl=exposure_auto=1 -d #{videodev}`
puts `v4l2-ctl --set-ctrl=exposure_absolute=225 -d #{videodev}`
puts `v4l2-ctl --set-ctrl=exposure_auto_priority=1 -d #{videodev}`
puts `v4l2-ctl -L -d #{videodev}`

station_name = "video"
station = KradStation.new(station_name)
sleep 1
station.cmd("setdir ~/krad_stations")
station.cmd("webon 13004 13005")
width = 1280
height = 720
fps = 30
codec = "vp8vorbis"
station.cmd("fps #{fps}")
station.cmd("res #{width} #{height}")
sleep 3
station.cmd("transmitter_on 8004")
sleep 1

station.cmd("input Mic")
station.cmd("transmit av transmitter 8004 /video.webm firefox #{codec} #{width} #{height}")
#station.cmd("record video ~/Videos/krad_recording.webm #{codec} #{width} #{height} 55555")
sleep 3
station.cmd("capture v4l2") 
puts station.cmd("logfile")
