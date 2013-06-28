function Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {
    // This is a very simple and unsafe constructor. All we're doing is checking if the values exist.
    // "x || 0" just means "if there is a value for x, use that. Otherwise use 0."
    // But we aren't checking anything else! We could put "Lalala" for the value of x 
    this.x = x || 0;
    this.y = y || 0;
    this.w = w || 1;
    this.h = h || 1;
    this.r = r || 0;
    this.o = o || 1;
    this.xs = xs || 1;
    this.ys = ys || 1;
    this.fill = fill || '#AAAAAA';
    this.selected = false;
    this.type = type || "";
    this.num = num || 0;
    this.selectionBoxColor = 'darkred'; // New for selection boxes
    this.selectionBoxSize = 20;
    this.selectionColor = '#CC0000';
    this.selectionWidth = 2;  
    
    this.PCView = type == 'videoport' 
     ? new PerspectiveCorrectionView(0,0,this.w,0,0,this.h,this.w, this.h) 
     : null;   
}

Shape.prototype.setValue = function(name, value) {
    if (name == "x") {
	      this.x = value;
    } else if (name == "y") {
	      this.y = value;
    } else if (name == "xscale") {
	      this.xs = value;
    } else if (name == "yscale") {
	      this.ys = value;
    } else if (name == "rotation") {
	      this.r = Math.PI / 180.0 * value;
    } else if (name == "opacity") {
	      this.o = value;
    } else if (name == "view_top_left_x") {
        this.PCView.handles[0].x = value;
    } else if (name == "view_top_left_y") {
        this.PCView.handles[0].y = value;
    } else if (name == "view_top_right_x") {
        this.PCView.handles[1].x = value;
    } else if (name == "view_top_right_y") {
        this.PCView.handles[1].y = value;
    } else if (name == "view_bottom_left_x") {
        this.PCView.handles[2].x = value;
    } else if (name == "view_bottom_left_y") {
        this.PCView.handles[2].y = value;
    } else if (name == "view_bottom_right_x") {
        this.PCView.handles[3].x = value;
    } else if (name == "view_bottom_right_y") {
        this.PCView.handles[3].y = value;
    }
}


function Kradradio() {
  this.sysname = "";
  this.sample_rate = 0;
  this.tags = new Array();
  this.decklink_devices = new Array();
  this.v4l2_devices = new Array();
  this.bools = new Object();  
  this.peaks = new Array();
  this.peaks_max = new Array();
  this.showing_snapshot_buttons = 0;
  this.system_cpu_usage = 0;
  this.admin = 0;
  this.portgroup_count = 0;
  this.xmms2_portgroup_count = 0;
}

Kradradio.prototype.destroy = function() {
  $('#' + this.sysname).remove();
}

Kradradio.prototype.set_mixer_params = function(mixer_params) {
  this.sample_rate = mixer_params.sample_rate;
  $('.kradradio_mixer_info').append("<h2 class='sysinfo'>Sample Rate: "
   + this.sample_rate + " </h2>"); 
}

Kradradio.prototype.got_sysname = function(sysname) {
  this.sysname = sysname;

  $('#kradradio').append("<div class='kradradio_station' id='"
   + this.sysname + "'>\
<div class='kradradio_station_info' id='Station_Info'>\
<h1 class='info_title'>Station</h1>\
<h2 class='sysinfo'>Sysname: " + this.sysname + "</h2>\
</div>\
<div class='kradradio_mixer_info' id='Mixer_Info'>\
<h1 class='info_title'>Mixer</h1>\
<ul class='mixer_subunit_list' id='mixer_subunits'></ul>\
</div>\
<div class='kradradio_compositor_info'>\
<h1 class='info_title'>Compositor</h1>\
<ul class='compositor_subunit_list' id='compositor_subunits'></ul>\
<br clear='both'>\
</div>\
<div class='kradradio_transponder_info'>\
<h1 class='info_title'>Transponder</h1>\
<br clear='both'>\
</div>\
</div>");    
}

Kradradio.prototype.got_tag = function (tag_item, tag_name, tag_value) {

    for (t in this.tags) {
	      if ((this.tags[t].tag_item == tag_item) && (this.tags[t].tag_name == tag_name)) {
	          this.tags[t].update_value (tag_value);
	          return;
	      }
    }
    
    tag = new Tag (tag_item, tag_name, tag_value);
    
    this.tags.push (tag);
    
}

Kradradio.prototype.got_peak_portgroup = function (portgroup_name, value) {

    //kradwebsocket.debug(portgroup_name);
    
    value = Math.round(value);

    if (this.peaks_max[portgroup_name] < this.peaks[portgroup_name]) {
	      this.peaks_max[portgroup_name] = this.peaks[portgroup_name];
    }

    if (this.peaks[portgroup_name] == value) {
	      return;
    } else {

	      this.peaks[portgroup_name] = value;
    }

    this.portgroup_draw_peak_meter (portgroup_name, value);
}


Kradradio.prototype.got_messages = function (msgs) {

    kradwebsocket.debug(msgs);

    var msg_arr = JSON.parse (msgs);

    for (m in msg_arr) {
	      kradwebsocket.debug(msg_arr[m]);  
	      this.crate_handler(msg_arr[m]);
    }
}



Kradradio.prototype.got_update_subunit = function (num, type,
 control_name, value) {
}


Kradradio.prototype.control_eq_effect = function (portgroup_name, effect_name, effect_num, control_name, value) {

    var cmd = {};  
    cmd.com = "kradmixer";  
    cmd.cmd = "control_eq_effect";
    cmd.portgroup_name = portgroup_name;
    cmd.effect_name = effect_name;
    cmd.effect_num = effect_num;
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}


Kradradio.prototype.clear_max_peak = function (value) {
    this.peaks_max[value] = -1;
    this.portgroup_draw_peak_meter (value, -1);
}

Kradradio.prototype.got_control_portgroup = function (portgroup_name, control_name, value) {

    //kradwebsocket.debug("control portgroup " + portgroup_name + " " + value);

    if ($('#' + portgroup_name)) {
	      if (control_name == "volume") {
	          $('#' + portgroup_name).slider( "value" , value );
	      } else {
	          $('#' + portgroup_name + '_crossfade').slider( "value" , value );
	      }
    }
}

Kradradio.prototype.got_effect_control = function (crate) {

    //kradwebsocket.debug(crate);

    if (crate.effect_name == "eq") {
	      if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.effect_num + '_' + crate.control_name)) {
	          $('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.effect_num + '_' + crate.control_name).slider( "value" , crate.value );
	      }
    } else {
	      if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.control_name)) {
	          $('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.control_name).slider( "value" , crate.value );
	      }
    }
}

Kradradio.prototype.got_update_portgroup = function (portgroup_name, control_name, value) {

    //    kradwebsocket.debug("got_update_portgroup " + portgroup_name + " " + control_name + " " + value);

    if (control_name == "crossfade_group") {
	      kradradio.portgroup_handle_crossfade_group(portgroup_name, value, -100.0);
    }
    
    if (control_name == "xmms2_ipc_path") {
	      if (value.length > 0) {
	          kradradio.portgroup_handle_xmms2(portgroup_name, 1);
	      } else {
	          kradradio.portgroup_handle_xmms2(portgroup_name, 0);
	      }
    }
}

Kradradio.prototype.portgroup_handle_crossfade_group = function (portgroup_name, crossfade_name, crossfade) {

    if ($('#portgroup_' + portgroup_name + '_crossfade_wrap')) {
	      $('#portgroup_' + portgroup_name + '_crossfade_wrap').remove();
    }

    if (crossfade_name.length > 0) {
	      $('#portgroup_' + portgroup_name + '_wrap').after("<div id='portgroup_" + portgroup_name + "_crossfade_wrap' class='kradmixer_control crossfade_control'> <div id='" + portgroup_name + "_crossfade'></div> <h2>" + portgroup_name + " - " + crossfade_name + "</h2></div>");

	      $('#' + portgroup_name + '_crossfade').slider({orientation: 'horizontal', value: crossfade, min: -100, max: 100 });

	      $( '#' + portgroup_name + '_crossfade' ).bind( "slide", function(event, ui) {
	          kradradio.update_portgroup (portgroup_name, "crossfade", ui.value);
	      });
    }
}

Kradradio.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {
}

Kradradio.prototype.got_add_subunit = function (crate) {
  $('#compositor_subunits').append("<li id='" + crate.subunit_type 
   + crate.subunit_id + "_li'>" + crate.subunit_type
   + "_"  + crate.subunit_id);

  if (crate.subunit_type == 'sprite') {
    $('#' + crate.subunit_type + crate.subunit_id + '_li').append(": '" 
     + crate.filename + "' rate: " + crate.rate); 
  } else if (crate.subunit_type == 'videoport') {
    $('#' + crate.subunit_type + crate.subunit_id + '_li').append(
     "direction: " + crate.direction + "port_name: " + crate.port_name); 
  }

  $('#' + crate.subunit_type + crate.subunit_id + '_li').append(
   ' x: ' + crate.x + ' y: ' + crate.y + ' width: ' + crate.width 
   + ' height: ' + crate.height
   + ' rotation: ' +  crate.r  + ' opacity: ' 
   + crate.o + ' xscale: ' +  crate.xscale + ' yscale: ' + crate.yscale
   + "</li>");
}



Kradradio.prototype.got_remove_subunit = function(crate) {
  $('#' + crate.subunit_type + '_' + crate.subunit_id).remove();
  $('li.subunit:even').removeClass("alt");
  $('li.subunit:odd').addClass("alt");
}

Kradradio.prototype.got_add_portgroup = function (crate) {
  this.peaks[crate.portgroup_name] = -1;
  this.peaks_max[crate.portgroup_name] = -1;
  this.portgroup_count += 1;

    $('#mixer_subunits').append("<li id='" + crate.portgroup_name + "_li'>Portgroup: " + crate.portgroup_name + "</li>");
     
    if (crate.xmms2 == 1) {
	      this.xmms2_portgroup_count += 1;
	      $('#' + crate.portgroup_name + '_li').append("<br>Has xmms2");
    }
 
    if (crate.crossfade_name.length > 0) {
        $('#mixer_subunits').append("<li>Crossfade: " + crate.portgroup_name + "  " + crate.crossfade_name + "</li>");
	      
    }
    
      
    $('li').removeClass("alt");
    $('li:even').addClass("alt");
    if (crate.xmms2 == 1) {
	      kradradio.portgroup_handle_xmms2(crate.portgroup_name, crate.xmms2);
    }

    $('#portgroup_' + crate.portgroup_name + '_wrap').append("<div id='ktags_" + crate.portgroup_name + "'></div>");

    if (crate.direction == 1) {
	      $('.kradmixer_cappa').append("<div class='effect_area'> <div id='" + crate.portgroup_name + "_effects'><h2>Effects for " + crate.portgroup_name + "</h2></div> </div>");

	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_lowpass_hz'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_lowpass_bw'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_highpass_hz'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_highpass_bw'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_analog_drive'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_analog_blend'></div>");
	      
	      $('#' + crate.portgroup_name + '_lowpass_hz').slider({orientation: 'vertical', value: crate.lowpass_hz, step: 1, min: 20.0, max: 12000.0 });
	      $('#' + crate.portgroup_name + '_lowpass_bw').slider({orientation: 'vertical', value: crate.lowpass_bw, step: 0.1, min: 0.1, max: 5.0 });
	      $('#' + crate.portgroup_name + '_highpass_hz').slider({orientation: 'vertical', value: crate.highpass_hz, step: 1, min: 20.0, max: 20000.0 });
	      $('#' + crate.portgroup_name + '_highpass_bw').slider({orientation: 'vertical', value: crate.highpass_bw, step: 0.1, min: 0.1, max: 5.0 });
	      $('#' + crate.portgroup_name + '_analog_drive').slider({orientation: 'vertical', value: crate.analog_drive, step: 0.1, min: 0.1, max: 10.0 });
	      $('#' + crate.portgroup_name + '_analog_blend').slider({orientation: 'vertical', value: crate.analog_blend, step: 0.1, min: -10.0, max: 10.0 });
	      
	      $( '#' + crate.portgroup_name + '_lowpass_hz' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "lp", "hz", ui.value);
	      });

	      $( '#' + crate.portgroup_name + '_lowpass_bw' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "lp", "bw", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_highpass_hz' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "hp", "hz", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_highpass_bw' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "hp", "bw", ui.value);
	      });

	      $( '#' + crate.portgroup_name + '_analog_drive' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "a", "drive", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_analog_blend' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "a", "blend", ui.value);
	      });

	      $('.kradmixer_cappa').append("<br clear='both'/><div class='effect_area'> <div id='" + crate.portgroup_name + "_effects_eq'></div> </div>");

	      //for (band in crate.eq.bands) {
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_hz'></div>");
	          
	          var mini;
	          var maxi;
	          
	          mini = crate.eq.bands[i].hz - crate.eq.bands[i].hz;
	          maxi = crate.eq.bands[i].hz + crate.eq.bands[i].hz;
	          
	          if (mini < 20) {
		            mini = 20;
	          }           
	          if (maxi > 20000) {
		            maxi = 20000;
	          }
	          
            
	          $('#' + crate.portgroup_name + '_eq_' + i + '_hz').slider({orientation: 'vertical', value: crate.eq.bands[i].hz, step: 1, min: mini, max: maxi });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_hz' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "hz", ui.value);
	          });
	      }
	      
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_db'></div>");
	          $('#' + crate.portgroup_name + '_eq_' + i + '_db').slider({orientation: 'vertical', value: crate.eq.bands[i].db, step: 0.1, min: -12.0, max: 12.0 });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_db' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "db", ui.value);
	          });
	      }
	      
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_bw'></div>");
	          $('#' + crate.portgroup_name + '_eq_' + i + '_bw').slider({orientation: 'vertical', value: crate.eq.bands[i].bw, step: 0.1, min: 0.1, max: 5.0 });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_bw' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "bw", ui.value);
	          });
	      }
	      
	      
    }


    if ($.browser.webkit) {
	      $('.krad_button_small').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(151,151,151,1) 12%,rgba(171,171,171,1) 100%)');
	      $('.krad_button_small:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 0%,rgba(254,182,69,1) 100%)');
	      $('.krad_button').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 12%,rgba(254,182,69,1) 100%)');
	      $('.krad_button:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 12%,rgba(254,182,69,1) 100%)');
	      $('.krad_button2').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(83,203,241,1) 0%,rgba(5,171,224,1) 100%)');
	      $('.krad_button2:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(83,203,241,1) 0%,rgba(5,171,224,1) 100%)');
    }

}

Kradradio.prototype.got_remove_portgroup = function (name) {

    $('#portgroup_' + name + '_wrap').remove();

}

Kradradio.prototype.remove_link = function (link_num) {
}

Kradradio.prototype.got_update_link = function (link_num, control_name, value) {
}

Kradradio.prototype.update_link = function (link_num, control_name, value) {
}


Kradradio.prototype.got_add_decklink_device = function (decklink_device) {
}


Kradradio.prototype.got_remove_link = function (link) {
}


Kradradio.prototype.got_add_link = function (link) {
}




