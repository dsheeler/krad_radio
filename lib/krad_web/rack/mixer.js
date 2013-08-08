Kr.prototype.mixer_got_add_portgroup = function(crate) {
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
    var selectors = kr.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    for (var i = 0; i < selectors.length; i++) {
      kr.ux.background_color_backups[selectors[i]] = $(selectors[i]).css('background-color');
      $(selectors[i]).css('background-color', '#333');
    }
  });

   $("#portgroup_" + crate.portgroup_name + "_wrap .close").bind('mouseleave', {'crate': crate}, function(event) {
    var selectors = kr.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    for (var i = 0; i < selectors.length; i++) {
      $(selectors[i]).css('background-color', kr.ux.background_color_backups[selectors[i]]);
    }
  });

  $("#portgroup_" + crate.portgroup_name + "_wrap .close").bind('click', {'crate': crate}, function(event) {

    var selectors = kr.maximized_crossfader_group_selectors(event.data.crate.portgroup_name);
    var min_selectors = kr.minimized_crossfader_group_selectors(event.data.crate.portgroup_name);
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
        $("#portgroup_" + kr.ux.crossfades[event.data.crate.portgroup_name][i] +"_minimized_wrap").append($("#"
         + kr.ux.crossfades[event.data.crate.portgroup_name][i] + "_meter_canvas"));

        $("#" + kr.ux.crossfades[event.data.crate.portgroup_name][i] + "_meter_canvas").css({'position': 'relative',
          'display': 'inline'});

        $('#portgroup_' + kr.ux.crossfades[event.data.crate.portgroup_name][i]  + '_minimized_wrap').bind('click', {'i': i, 'crate': event.data.crate}, function(e) {
          var selectors = kr.maximized_crossfader_group_selectors(e.data.crate.portgroup_name);
          var min_selectors = kr.minimized_crossfader_group_selectors(e.data.crate.portgroup_name);
          for (var j = 0; j < selectors.length; j++) {
            $(selectors[j]).show(200);
          }
          for (var j = 0; j < 2; j++) {
            $("#portgroup_" + kr.ux.crossfades[e.data.crate.portgroup_name][j] + "_wrap .meter_wrapper" ).append(
             $("#" +  kr.ux.crossfades[e.data.crate.portgroup_name][j] + "_meter_canvas"));
            $("#" + kr.ux.crossfades[e.data.crate.portgroup_name][j] + "_meter_canvas").css({'top': '0px'});
          }
          for (var j = 0; j < min_selectors.length; j++) {
            $(min_selectors[j]).hide(200);
           }
        });
      }
    }
  });

  if (crate.portgroup_name == "DTMF") {

	  $('#' + this.sysname).append("<div id='dtmf_pad' "
     + "class='RCU kradmixer_control dtmf_pad'>"
     + "<div class='button_wrap'><div class='krad_button_very_small close'>-</div></div>"
     + "<div id='dtmf' style='float:  left'</div> </div>");

    $("#dtmf_pad .close").bind('click', function () {
    $('#chooser').append("<div class='chooser_choice' id='"
     + crate.portgroup_name + "_dtmf_pad_min_choice'><h3>"
     + crate.portgroup_name + " Dial Tone Pad</h3></div>");
     $('#dtmf_pad').hide(200);

    $('#' + crate.portgroup_name + '_dtmf_pad_min_choice').bind('click', function() {
      $('#dtmf_pad').show(200);
      $(this).remove();
    });

  });
  
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name 
	  + "_dtmf_1'>1</div></div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_2'>2</div></div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_3'>3</div></div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_a'>F0</div></div>");

	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_4'>4</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_5'>5</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_6'>6</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_b'>F</div></div>");

	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_7'>7</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_8'>8</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_9'>9</div>");
	  $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_c'>I</div></div>");
  
    $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_star'>*</div>");
    $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_0'>0</div>");
    $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_hash'>#</div>");
    $('#dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + crate.portgroup_name
     + "_dtmf_d'>P</div></div>");

  
    $('#' +  crate.portgroup_name + '_dtmf_1').bind("click",
     function(event, ui) {
      kr.push_dtmf ("1");
    });

  	$('#' +  crate.portgroup_name + '_dtmf_2').bind("click",
     function(event, ui) {
  	  kr.push_dtmf ("2");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_3').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("3");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_4').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("4");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_5').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("5");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_6').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("6");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_7').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("7");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_8').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("8");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_9').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("9");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_0').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("0");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_star').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("*");
	  });

    $('#' +  crate.portgroup_name + '_dtmf_hash').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("#");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_a').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("A");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_b').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("B");
	  });

	  $('#' +  crate.portgroup_name + '_dtmf_c').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("C");
	  });
  
	  $('#' +  crate.portgroup_name + '_dtmf_d').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("D");
	  });
    kr.position_RCUs();
  }

  $('#fx_' + crate.portgroup_name).bind("click", { 'name': crate.portgroup_name },
   function(event, ui) {
     kr.toggle_show_effects(event.data.name);
   });

  $('#fade_' + crate.portgroup_name).bind("click",
     function(event, ui) {
      var new_value = 0;
      if (kr.ux.portgroup_volume_toggle[crate.portgroup_name]) {
        new_value = kr.ux.portgroup_volume_toggle[crate.portgroup_name];
        kr.ux.portgroup_volume_toggle[crate.portgroup_name] = 0;
      } else {
        new_value = 0;
        kr.ux.portgroup_volume_toggle[crate.portgroup_name]
          = kr.ux.portgroup_values[crate.portgroup_name];
      }
      kr.ctrl("m/" + crate.portgroup_name + "/v", new_value, 1500);
	  });

  var name = crate.portgroup_name;
  var volume_element = document.id(crate.portgroup_name);
  var volume_slider = volume_element.getElement('.volume');

  kr.ux.portgroup_values[crate.portgroup_name] = crate.volume;
  this.ux.portgroup_volume_sliders[name] = new Slider(volume_slider,
   volume_slider.getElement('.handle'), {
      range: [100, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.volume,
        onChange: function (value) {
         kr.ux.portgroup_values[crate.portgroup_name] = value;
         kr.ctrl("m/" + crate.portgroup_name + "/v", value)
       }
    });

  if (crate.crossfade_name.length > 0) {
	  kr.portgroup_handle_crossfade_group(crate.portgroup_name,
     crate.crossfade_name, crate.crossfade);
  }

  if (crate.xmms2 == 1) {
	  kr.portgroup_handle_xmms2(crate.portgroup_name, crate.xmms2);
  }

  if (crate.direction == 1) {

    $('.effects').append("<div class='effect_area' "
     + "id='" + crate.portgroup_name
     + "_effects' style='float:left'></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_lowpass_hz'><h4 id='lp_hz_label'>LP FREQ</h4>"
     + "<div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_lowpass_bw'><h4 id='lp_bw_label'>BANDWIDTH</h4><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_highpass_hz'><h4 id='hp_hz_label'>HP FREQ</h4><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_highpass_bw'><h4 id='hp_hz_label'>BANDWIDTH</h4><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_analog_drive'><h4 id='hp_hz_label'>DRIVE</h4><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_analog_blend'><h4 id='hp_hz_label'>BLEND</h4><div class='volume'>"
     + "<div class='handle'></div></div></div>");

    var name = crate.portgroup_name + "_lowpass_hz";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider,
     effect_slider.getElement('.handle'), {
       range: [100, 0],
       steps: 200,
       wheel: true,
       mode: "vertical",
       initialStep: 100*Math.log(crate.lowpass_hz/20)/Math.log(600),
       onChange: function (value) {
         kr.ctrl("e/" + crate.portgroup_name + "/lp/hz", 20*Math.pow(2,value*Math.log(600)/Math.log(2)/100));
       }
     });

 	  var name = crate.portgroup_name + "_lowpass_bw";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider,
     effect_slider.getElement('.handle'), {
       range: [100, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: 20*crate.lowpass_bw,
        onChange: function (value) {
          kr.ctrl("e/" + crate.portgroup_name + "/lp/bw", value/20);
        }
     });

	  var name = crate.portgroup_name + "_highpass_hz";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [100, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: 100*Math.log(crate.highpass_hz/20)/Math.log(1000),
        onChange: function (value) {
          kr.ctrl("e/" + crate.portgroup_name + "/hp/hz",  20*Math.pow(2,value*Math.log(1000)/Math.log(2)/100));
        }
     });

	 	var name = crate.portgroup_name + "_highpass_bw";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [100, 0],
        steps: 101,
        wheel: true,
        mode: "vertical",
        initialStep: 20*crate.highpass_bw,
        onChange: function (value) {
          kr.ctrl("e/" + crate.portgroup_name + "/hp/bw", value/20);
        }
     });

    var name = crate.portgroup_name + "_analog_drive";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [100, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: 10 * crate.analog_drive,
        onChange: function (value) {
          kr.ctrl("e/" + crate.portgroup_name + "/a/drive", 0.1*value);
        }
     });

    var name = crate.portgroup_name + "_analog_blend";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.ux.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [100, -100],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: 10 * crate.analog_blend,
        onChange: function (value) {
          kr.ctrl("e/" + crate.portgroup_name + "/a/blend", 0.1*value);
        }
    });
    $('.effects').append(
     "<div class='eq_area' id='eq_" + crate.portgroup_name
     + "'></div>");

    /*$('#' + 'eq_' + crate.portgroup_name).append(
     " <div id='" + crate.portgroup_name + "_effects_eq_hz'"
     + "style='float:left'></div>");

    for (var i = 0; i < 32; i=i+4) {
      $('#' + crate.portgroup_name + '_effects_eq_hz').append
       ("<div class='kradmixer_control kradmixer_eq' id='"
       + crate.portgroup_name + "_eq_" + i + "_hz'><div class='volume'>"
       + "<div class='handle'></div></div></div>");

      var name = crate.portgroup_name + "_eq_" + i + "_hz";
      kr.debug(name);
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');
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

      this.ux.portgroup_eq_sliders[name] = new Slider(eq_slider,
       eq_slider.getElement('.handle'), {
        range: [maxi, mini],
        steps: 200,
        wheel: true,
        mode: "vertical",
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/hz",
        initialStep: crate.eq.bands[i].hz,
        onChange: function (value) {
          kr.ctrl(this.options.address, value);
          $('#eq_description_' + crate.portgroup_name).replaceWith("<h2 id='eq_description_"
           + crate.portgroup_name + "'>Hz: " + value + "</h2>");
        }
      });

      */


    $('#' + 'eq_' + crate.portgroup_name).append(" <div id='" + crate.portgroup_name + "_effects_eq_db'"
     + " style='float:left'></div>");

    for (var i = 0; i < 32; i=i+4) {
      $('#' + crate.portgroup_name + '_effects_eq_db').append(
       "<div class='kradmixer_control kradmixer_eq' id='"
       + crate.portgroup_name + "_eq_" + i + "_db'><div class='volume'>"
       + "<div class='handle'></div></div></div>");
      var name = crate.portgroup_name + "_eq_" + i.toString() + "_db";
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');

      this.ux.portgroup_eq_sliders[name] = new Slider(eq_slider,
       eq_slider.getElement('.handle'), {
        range: [100, -100],
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/db",
        initialStep: 100/12 * crate.eq.bands[i].db,
        onChange: function (value) {
          kr.ctrl(this.options.address, 12/100*value);
        }
	    });
	  }
    /*
    $('#' + 'eq_' + crate.portgroup_name).append("<br clear='both'/>"
     + " <div id='" + crate.portgroup_name + "_effects_eq_bw'"
     + "style='float:left'></div>");

    for (var i = 0; i < 32;  i=i+4) {
      $('#' + crate.portgroup_name + '_effects_eq_bw').append
       ("<div class='kradmixer_control kradmixer_eq' id='"
       + crate.portgroup_name + "_eq_" + i + "_bw'><div class='volume'>"
       + "<div class='handle'></div></div></div>");

  		var name = crate.portgroup_name + "_eq_" + i.toString() + "_bw";
      kr.debug(name);
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');

      this.ux.portgroup_eq_sliders[name] = new Slider(eq_slider, eq_slider.getElement('.handle'), {
        range: [5.0, 0.1],
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/bw",
        initialStep: crate.eq.bands[i].bw,
        onChange: function (value) {
          kr.ctrl(this.options.address, value);
          $('#eq_description_' + crate.portgroup_name).replaceWith("<h2 id='eq_description_"
           + crate.portgroup_name + "'>Bandwidth: " + value + "</h2>");
        }
	    });
    }*/
  }

  this.mixer_draw_peak_meter(crate.portgroup_name, 0);
}

Kr.prototype.portgroup_handle_crossfade_group = function(portgroup_name,
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

    this.ux.crossfades[portgroup_name] = [portgroup_name, crossfade_name];
    this.ux.crossfades[crossfade_name] = [portgroup_name, crossfade_name];

    var crossfade_element = document.id(portgroup_name + "_crossfade");
    var crossfade_slider = crossfade_element.getElement('.fade');

    this.ux.portgroup_crossfade_sliders[portgroup_name] =
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


Kr.prototype.mixer_got_peak_portgroup = function(portgroup_name, value) {
  value = Math.round(value);
  if (this.peaks_max[portgroup_name] < this.peaks[portgroup_name]) {
    this.peaks_max[portgroup_name] = this.peaks[portgroup_name];
  }
  if (this.peaks[portgroup_name] == value) {
    return;
  } else {
    this.peaks[portgroup_name] = value;
  }
  this.mixer_draw_peak_meter (portgroup_name, value);
}

Kr.prototype.mixer_alt_draw_peak_meter = function (portgroup_name, value) {

  var size = 90;
  var pos = value * 1.8 - 90.0;
  var canvas = document.getElementById('analog_meter_canvas');
  if (!canvas) {
    return;
  }

  var ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, 200, 200);
  ctx.save();
  ctx.translate(100, 100);
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

Kr.prototype.mixer_draw_peak_meter
 = function(portgroup_name, value) {

    if (portgroup_name == "MasterBUS") {
      this.mixer_alt_draw_peak_meter(portgroup_name, value);
    }
  canvas = document.getElementById(portgroup_name  + '_meter_canvas');
  if (!canvas) {
	  return;
  }

  ctx = canvas.getContext('2d');
  value = value * 2;
  inc = 5;
  dink = -4;
  width = 100;
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

Kr.prototype.maximized_crossfader_group_selectors = function(input) {
  var selectors = new Array();
  selectors.push('#portgroup_' + input + '_wrap');
  if (this.ux.crossfades[input]) {
    selectors.push('#' + this.ux.crossfades[input][0] + '_crossfade_wrap');
    if (this.ux.crossfades[input][0] == input) {
      selectors.push('#portgroup_' + this.ux.crossfades[input][1] + '_wrap');
    } else {
      selectors.push('#portgroup_' + this.ux.crossfades[input][0] + '_wrap');
    }
  }
  return selectors;
}

Kr.prototype.minimized_crossfader_group_selectors = function(input) {
  var selectors = new Array();
  selectors.push('#portgroup_' + input + '_minimized_wrap');
  if (this.ux.crossfades[input]) {
    if (this.ux.crossfades[input][0] == input) {
      selectors.push('#portgroup_' + this.ux.crossfades[input][1] + '_minimized_wrap');
    } else {
      selectors.push('#portgroup_' + this.ux.crossfades[input][0] + '_minimized_wrap');
    }
  }
  return selectors;
}
