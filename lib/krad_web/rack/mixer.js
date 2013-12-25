  $(document).ready(function() {
  rack_units.push({'constructor': Mixer, 'aspect': [32,6]});
});

function Mixer(info_object) {
  this.title = "Mixer";
  this.description = "Adjust volumes, fool.";
  this.aspect_ratio = [32,6];
  this.sample_rate = 0;
  this.x = 0;
  this.y = 10;
  this.width = 0;
  this.height = 0;
  this.address_masks = ["kradmixer"];
  this.div_text = "<div id='kradmixer' class='ultrablue RCU kradmixer'>"
    + "<div id='mixers'><h2 style='display: none' id='sample_rate'>"
    + "Sample Rate: " + this.sample_rate 
    + " Hz</h2>"
    + "<div class='analog_meter_wrapper' style='float: left'>"
    + "<canvas class='meter' id='analog_meter_canvas'>"
    + "</canvas></div><div class='maximized_portgroup_wraps'></div>"
    + "<div style='float: right' class='minimized_portgroup_wraps'></div>"
    + "</div>";
  this.sel = '#kradmixer';
  info_object['parent_div'].append(this.div_text);
  this.peaks = new Object();
  this.peaks_max = new Object();
  this.portgroup_count = 0;
  this.xmms2_portgroup_count = 0;
  this.background_color_backups = new Array();
  this.crossfades = new Array();
  this.portgroup_values = new Object();
  this.portgroup_volume_sliders = new Object();
  this.portgroup_crossfade_sliders = new Object();
  this.portgroup_volume_toggle = new Object();
}

Mixer.prototype.shared = function(key, shared_object) {
}

Mixer.prototype.update = function(crate) {
  if (crate.ctrl == "peak_portgroup") { 
    this.got_peak_portgroup(crate.portgroup_name, crate.value);
  }
  if (crate.ctrl == "control_portgroup") {
    this.got_control_portgroup(crate.portgroup_name, crate.control_name, crate.value);
  } 
  if (crate.ctrl == "add_portgroup") {
    this.got_add_portgroup(crate);
  }
  if (crate.ctrl == "remove_portgroup") {
    this.got_remove_portgroup(crate.portgroup_name);
  }
  if (crate.ctrl == "set_mixer_params") {
    this.got_set_mixer_params(crate);
  }
}

Mixer.prototype.got_set_mixer_params = function(crate) {
  this.sample_rate = crate.sample_rate;
  $('#sample_rate').html("Sample Rate: " + this.sample_rate + " Hz");
}

Mixer.prototype.got_add_portgroup = function(crate) {
  this.peaks[crate.portgroup_name] = -1;
  this.peaks_max[crate.portgroup_name] = -1;
  this.portgroup_count += 1;
  if (crate.xmms2 == 1) {
    this.xmms2_portgroup_count += 1;
  }

  volume_div = "<div class='kradmixer_control volume_control' id='portgroup_"
   + crate.portgroup_name + "_wrap'><div class='button_wrap'>"
   + "<div class='krad_button_very_small close'>-</div></div>"
   + "<h6>" + crate.portgroup_name
   + "</h6><div class='meter_wrapper'>"
   + "<canvas class='meter' id='"
   + crate.portgroup_name + "_meter_canvas'></canvas></div><div id='"
   + crate.portgroup_name + "'><div class='volume'>"
   + "<div class='handle'></div></div></div>"
   + "<div class='button_wrap'>"
   + "<div class='krad_button_very_small' id='fx_"
   + crate.portgroup_name + "'>FX</div></div>"
   + "<div class='button_wrap'><div class='krad_button_very_small' id='fade_"
   + crate.portgroup_name + "'>FADE</div></div></div>";

  $('#mixers .maximized_portgroup_wraps').append(volume_div);
  $('#mixers .minimized_portgroup_wraps').append("<div "
   + "class='minimized_portgroup_wrap' style='display: none'"
   + "id='portgroup_" + crate.portgroup_name + "_minimized_wrap'><h3>"
   + crate.portgroup_name + "</h3></div>");

  $("#portgroup_" + crate.portgroup_name + "_wrap .close").bind('mouseenter', {'crate': crate}, function(event) {
    var selectors = this.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    for (var i = 0; i < selectors.length; i++) {
      this.background_color_backups[selectors[i]] = $(selectors[i]).css('background-color');
      $(selectors[i]).css('background-color', '#333');
    }
  });

   $("#portgroup_" + crate.portgroup_name + "_wrap .close").bind('mouseleave', {'crate': crate}, function(event) {
    var selectors = this.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    for (var i = 0; i < selectors.length; i++) {
      $(selectors[i]).css('background-color', this.background_color_backups[selectors[i]]);
    }
  });

  $("#portgroup_" + crate.portgroup_name + "_wrap .close").bind('click', {'crate': crate}, function(event) {

    var selectors = this.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    var min_selectors = this.minimized_crossfader_group_selectors(event.data.crate.portgroup_name);
    for (var i = 0; i < selectors.length; i++) {
      $(selectors[i]).hide(200);
    }

    kr.debug(min_selectors, selectors);
    for(var i = 0; i < min_selectors.length; i++) {
      $(min_selectors[i]).show(200);
    }

    if (min_selectors.length == 1) {
      $("#portgroup_" + event.data.crate.portgroup_name +"_minimized_wrap").append($("#"
       + event.data.crate.portgroup_name + "_meter_canvas"));

      $("#" + event.data.crate.portgroup_name + "_meter_canvas").css({'position': 'relative',
         'display': 'inline'});

      $('#portgroup_' + event.data.crate.portgroup_name  + '_minimized_wrap').bind('click', {'crate': event.data.crate}, function(e) {

        $('#portgroup_' + e.data.crate.portgroup_name  + '_wrap').show(200);

        $("#portgroup_" + e.data.crate.portgroup_name + "_wrap .meter_wrapper" ).append(
         $("#" + e.data.crate.portgroup_name + "_meter_canvas"));

        $("#" + e.data.crate.portgroup_name + "_meter_canvas").css({'top': '0px'});

        $(this).hide(200);

      });
    } else {
      for (var i = 0; i < 2; i++) {
        $("#portgroup_" + this.crossfades[event.data.crate.portgroup_name][i] +"_minimized_wrap").append($("#"
         + this.crossfades[event.data.crate.portgroup_name][i] + "_meter_canvas"));

        $("#" + this.crossfades[event.data.crate.portgroup_name][i] + "_meter_canvas").css({'position': 'relative',
          'display': 'inline'});

        $('#portgroup_' + this.crossfades[event.data.crate.portgroup_name][i]  + '_minimized_wrap').bind('click', {'i': i, 'crate': event.data.crate}, function(e) {
          var selectors = this.maximized_crossfader_group_selectors(e.data.crate.portgroup_name);
          var min_selectors = this.minimized_crossfader_group_selectors(e.data.crate.portgroup_name);
          for (var j = 0; j < selectors.length; j++) {
            $(selectors[j]).show(200);
          }
          for (var j = 0; j < 2; j++) {
            $("#portgroup_" + this.crossfades[e.data.crate.portgroup_name][j] + "_wrap .meter_wrapper" ).append(
             $("#" +  this.crossfades[e.data.crate.portgroup_name][j] + "_meter_canvas"));
            $("#" + this.crossfades[e.data.crate.portgroup_name][j] + "_meter_canvas").css({'top': '0px'});
          }
          for (var j = 0; j < min_selectors.length; j++) {
            $(min_selectors[j]).hide(200);
           }
        });
      }
    }
  });


  $('#fx_' + crate.portgroup_name).bind("click", { 'name': crate.portgroup_name },
   function(event) {
     kr.ux.share('effects_display', event.data.name);
   });

  var me = this;
  $('#fade_' + crate.portgroup_name).bind("click", {'me': me},
    function(event) {
     var new_value = 0;
     if (event.data.me.portgroup_volume_toggle[crate.portgroup_name]) {
       new_value = event.data.me.portgroup_volume_toggle[crate.portgroup_name];
       event.data.me.portgroup_volume_toggle[crate.portgroup_name] = 0;
     } else {
       new_value = 0;
       event.data.me.portgroup_volume_toggle[crate.portgroup_name]
        = event.data.me.portgroup_values[crate.portgroup_name];
     }
     kr.ctrl("m/" + crate.portgroup_name + "/v", new_value, 1500);
  });

  var name = crate.portgroup_name;
  var volume_element = document.id("portgroup_" + crate.portgroup_name + "_wrap");
  var volume_slider = volume_element.getElement('.volume');

  this.portgroup_values[crate.portgroup_name] = crate.volume;
  this.portgroup_volume_sliders[name] = new Slider(volume_slider,
   volume_slider.getElement('.handle'), {
     range: [100, 0],
     steps: 200,
     wheel: true,
     mode: "vertical",
     initialStep: crate.volume,
     onChange: function (value) {
       kr.ctrl("m/" + crate.portgroup_name + "/v", value)
     }
  });

  this.draw_peak_meter(crate.portgroup_name, 0);
}

Mixer.prototype.got_remove_portgroup = function(name) {
  $('#portgroup_' + name + '_wrap').remove();
}

Mixer.prototype.got_control_portgroup = function(portgroup_name,
 control_name, value) {
  if ($('#' + portgroup_name)) {
    if (control_name == "volume") {
      this.portgroup_volume_sliders[portgroup_name].set(value);
    } else {
      this.portgroup_crossfade_sliders[portgroup_name].set(value);
    }
  }
}

Mixer.prototype.got_update_portgroup = function (portgroup_name,
 control_name, value) {
  if (control_name == "crossfade_group") {
    this.portgroup_handle_crossfade_group(portgroup_name, value, -100.0);
  }
}

Mixer.prototype.portgroup_handle_crossfade_group = function(portgroup_name,
 crossfade_name, crossfade) {
  if ($('#portgroup_' + portgroup_name + '_crossfade_wrap')) {
    $('#portgroup_' + portgroup_name + '_crossfade_wrap').remove();
  }

  if (crossfade_name.length > 0) {

    $('#mixers .maximized_portgroup_wraps').append("<div id='"
     + portgroup_name + "_crossfade_wrap' "
     + "class='kradmixer_control crossfade_control'>"
     + "<div id='" + portgroup_name + "_crossfade' class='crossfade'>"
     + "<div class='fade'><div class='handle'></div></div></div></div>");

    this.crossfades[portgroup_name] = [portgroup_name, crossfade_name];
    this.crossfades[crossfade_name] = [portgroup_name, crossfade_name];

    var crossfade_element = document.id(portgroup_name + "_crossfade");
    var crossfade_slider = crossfade_element.getElement('.fade');

    this.portgroup_crossfade_sliders[portgroup_name] =
     new Slider(crossfade_slider,
     crossfade_slider.getElement('.handle'), {
     range: [-100, 100],
     steps: 400,
     wheel: true,
     mode: "horizontal",
     initialStep: crossfade,
     onChange: function (value) {
        kr.ctrl("m/" + portgroup_name + "/c", value)
     }
     });
  }
}

Mixer.prototype.maximized_crossfader_group_selectors = function(input) {
  var selectors = new Array();
  selectors.push('#portgroup_' + input + '_wrap');
  if (this.crossfades[input]) {
    selectors.push('#' + this.crossfades[input][0] + '_crossfade_wrap');
    if (this.crossfades[input][0] == input) {
      selectors.push('#portgroup_' + this.crossfades[input][1] + '_wrap');
    } else {
      selectors.push('#portgroup_' + this.crossfades[input][0] + '_wrap');
    }
  }
  return selectors;
}

Mixer.prototype.minimized_crossfader_group_selectors = function(input) {
  var selectors = new Array();
  selectors.push('#portgroup_' + input + '_minimized_wrap');
  if (this.crossfades[input]) {
    if (this.crossfades[input][0] == input) {
      selectors.push('#portgroup_' + this.crossfades[input][1] + '_minimized_wrap');
    } else {
      selectors.push('#portgroup_' + this.crossfades[input][0] + '_minimized_wrap');
    }
  }
  return selectors;
}

Mixer.prototype.got_peak_portgroup = function(portgroup_name, value) {
  value = Math.round(value);
  if (this.peaks_max[portgroup_name] < this.peaks[portgroup_name]) {
    this.peaks_max[portgroup_name] = this.peaks[portgroup_name];
  }
  if (this.peaks[portgroup_name] == value) {
    return;
  } else {
    this.peaks[portgroup_name] = value;
  }
  this.draw_peak_meter(portgroup_name, value);
}

Mixer.prototype.alt_draw_peak_meter = function (portgroup_name, value) {

  var size = 100;
  var pos = value * 1.8 - 90.0;
  var canvas = document.getElementById('analog_meter_canvas');
  if (!canvas) {
    return;
  }

  var ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, 300, 200);
  ctx.save();
  ctx.translate(150, 100);
  ctx.lineCap = "butt";
  ctx.lineWidth = 0.05*size;
  ctx.strokeStyle = "rgb(0,0,0)";
  ctx.beginPath();
  ctx.arc(0, 0, 0.8 *size, Math.PI, 0);
  ctx.stroke();

  ctx.strokeStyle = "rgba(242, 150, 12, 1.0)";
  ctx.beginPath();
  ctx.arc(0, 0, 0.65 * size,  Math.PI, 0);
  ctx.stroke();

  ctx.beginPath();
  ctx.arc(size -0.56*size, -0.15*size, 0.07 *size, 0, 2*Math.PI);
  ctx.fill();

  ctx.rotate(pos * Math.PI / 180.0);
  ctx.strokeRect(-0.055*size, 0, 0.11*size, -size);
  ctx.restore();
}

Mixer.prototype.draw_peak_meter = function(portgroup_name, value) {

    if (portgroup_name == "MasterBUS") {
      this.alt_draw_peak_meter(portgroup_name, value);
    }
  canvas = document.getElementById(portgroup_name  + '_meter_canvas');
  if (!canvas) {
	  return;
  }

  ctx = canvas.getContext('2d');
  value = value * 2;
  inc = 5;
  dink = -4;
  width = 300;
  height = 200;
  ctx.fillStyle = '#EC018D';
  x = 0;
  while (x <= value) {
	  ctx.fillRect(0, height - x, width, dink);
	  x = x + inc;
  }
  ctx.fillStyle = '#AC014D';
  x = 0;
  while (x <= (height - value)) {
	  ctx.fillRect(0, 0 + x, width,  dink);
	  x = x + inc;
  }
}

