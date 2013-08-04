Kr.prototype.comp_ctrl = function(num, type, control_name, value, duration) {
  var t;
  var d = duration || 0;
  if (type == "videoport") {
    t = "v";
  } else {
    t = type;
  }
  kr.ctrl(t + "/" + num + "/" + control_name, value, d);
  this.update_subunit(num, type, control_name, value);
}


Kr.prototype.load_interface = function() {
  this.ux.portgroup_volume_sliders = new Object();
  this.ux.portgroup_crossfade_sliders = new Object();
  this.ux.portgroup_effect_sliders = new Object();
  this.ux.portgroup_eq_sliders = new Object();
  this.ux.portgroup_values = new Object();
  this.ux.crossfades = new Object();
  this.ux.portgroup_volume_toggle = new Object();
  this.ux.opacity_toggle = new Object();
  this.ux.screen = new Object();
  this.ux.rack_control_units = new Array();
  this.ux.shown_effects = '';
  this.ux.background_color_backups = new Object();

  this.calc_rack();

  var OFF_GRID = 0;
  kr.ux.RCUs = [{
        sel: '#kradcompositor',
        width: 8,
        height: 5,
        x: OFF_GRID,
        y: OFF_GRID
    }, {
        sel: '#subunit_list',
        width: 4,
        height: 4,
        x: OFF_GRID,
        y: OFF_GRID
    }, {
        sel: '#chooser',
        width: 4,
        height: 1,
        x: OFF_GRID,
        y: OFF_GRID
    }, {
        sel: '#xmms2s',
        width: 4,
        height: 1,
        x: OFF_GRID,
        y: OFF_GRID
    }, {
        sel: '#dtmf_pad',
        width: 2,
        height: 2,
        x: OFF_GRID,
        y: OFF_GRID
    }, {
      sel: '.kradmixer_cappa',
      width: 6,
      height: 2,
      x: OFF_GRID,
      y: OFF_GRID
    }, {
        sel: '#kradmixer',
        width: 16,
        height: 3,
        x: OFF_GRID,
        y: OFF_GRID
    }];

  /* set up the selection handle boxes */
  this.selectionHandles = [];

  for (var i = 0; i < 10; i ++) {
    var rect = new Shape;
    this.selectionHandles.push(rect);
  }
}

Kr.prototype.got_sysname = function(sysname) {
  this.sysname = sysname;
  $('#kradradio').append("<div class='kradradio_station' id='"
   + this.sysname + "'>\
<div id='kradcompositor' class='RCU kradcompositor'></div>\
<ul id='subunit_list' class='RCU kradcompositor_subunit_list'><div class='button_wrap'>\
<div class='krad_button3' id='uber_happy'>\
UBER HAPPY</div></div></ul>\
<div id='chooser' class='chooser RCU'>\
<div class='button_wrap'><div class='krad_button3' id='toggle_minimizable'>\
EDIT MODE</div></div></div>\
<div id='xmms2s' class='RCU'></div>\
<div class='kradmixer_cappa RCU'><div class='button_wrap'><div \
class='krad_button_very_small close'>-</div></div>\
<h2 id='effects_region_description'></h2></div>\
<div id='kradmixer' class='RCU kradmixer'>\
<div id='mixers'>\
<div class='analog_meter_wrapper' style='float: left'>\
<canvas class='meter' id='analog_meter_canvas'>\
</canvas></div><div class='maximized_portgroup_wraps'></div>\
<div style='float: right' class='minimized_portgroup_wraps'></div>\
</div>\
</div>\
</div>");

  /*<div id='crossfades'><button class='close'>&times;</button></div>*/

  /*<div class='chooser'><div id='MIXER' class='chooser_choice chooser_chosen'>\
<h2>MIXER</h2></div>\
<div id='COMPER' class='chooser_choice'><h2>COMPER</h2>\
</div><div><br clear='both'>\*/
  this.position_RCUs();

  window.onresize = function(event) {
    kr.position_RCUs();
  }

  $('#toggle_minimizable').bind('click', function () {
    $(".close").toggle(200);
  });

  $('#uber_happy').bind('click', function() {
    $(".happy_button").click();
  });

  $('#kradcompositor').append("<canvas width='960px'"
  + "height='540px' class='kradcompositor_canvas'"
   + "id='compositor_canvas'>"
   + "</canvas>");

  $("#kradcompositor .close").bind('click', function () {
    $('.chooser').append("<div class='chooser_choice' id='kradcompositor_min_choice'"
     + "><h3>Comper</h3></div>");
     $('#kradcompositor').hide(200);

    $('#kradcompositor_min_choice').bind('click', function() {
      $('#kradcompositor').show(200);
      $(this).remove();
    });
  });

  kr.compositor_canvas = new CanvasState(document.getElementById
  ("compositor_canvas"), document.createElement('canvas'));

  $('.kradmixer_cappa .close').bind('click', function(e, ui) {
    kr.toggle_show_effects();
  });

  $('.kradmixer_cappa .close').mouseenter(function() {
    kr.close_color_backup = $(this).css('color');
    $(this).css('color', '#f00c0c');
  });

  $('.close').mouseleave(function(){
    $(this).css('color', kr.close_color_backup);
  });
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

Kr.prototype.mouseenter_handle = function() {
  kr.handle_border_backup = $(this).css('border');
  $(this).css('border', '2px solid #399');
}

Kr.prototype.mouseleave_handle = function() {
  $(this).css('border', kr.handle_border_backup);
}

Kr.prototype.mouseenter_glow = function () {
  this.border_backup = $(this).css('border-color');
  this.box_shadow_backup = $(this).css('box-shadow');
  $(this).css('border-color', '#ccc #aaa #aaa #ccc');
  $(this).css('box-shadow', '0px 0px 10px 5px rgba(240, 240, 240, 0.5)');
}

Kr.prototype.mouseleave_restore = function () {
  $(this).css('border-color', this.border_backup);
  $(this).css('box-shadow', this.box_shadow_backup);
}

Kr.prototype.kradcompositor_controls = function () {
  $('.kradcompositor').append("<br clear='both'/>");
}


Kr.prototype.got_tag = function (tag_item, tag_name, tag_value) {
  for (t in this.tags) {
    if ((this.tags[t].tag_item == tag_item)
     && (this.tags[t].tag_name == tag_name)) {
      this.tags[t].update_value (tag_value);
      return;
	  }
  }
  tag = new Tag (tag_item, tag_name, tag_value);
  this.tags.push (tag);
}

Kr.prototype.portgroup_alt_draw_peak_meter = function (portgroup_name, value) {

  var size = 90;
  var pos = value * 1.8 - 90.0;
  var canvas = document.getElementById('analog_meter_canvas');
  if (!canvas) {
    return;
  }

  var ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, 200, 200);
  ctx.save();
  ctx.translate(100, 150);
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

Kr.prototype.portgroup_draw_peak_meter
 = function(portgroup_name, value) {

    if (portgroup_name == "MasterBUS") {
      this.portgroup_alt_draw_peak_meter(portgroup_name, value);
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

Kr.prototype.got_peak_portgroup = function(portgroup_name, value) {
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

//function Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {
Kr.prototype.update_subunit = function(num, type, control_name, value) {
  var type_num = type + "_" + num;
  this.debug(type_num);
  switch (control_name) {
    case "x":
      $('#' + type_num + '_x').html(value.toFixed(0));
      break;
    case "y":
      $('#' + type_num + '_y').html(value.toFixed(0));
      break;
    case "width":
      $('#' + type_num + '_w').html(value);
      break;
    case "height":
      $('#' + type_num + '_h').html(value);
      break;
    case "rotation":
      $('#' + type_num + '_r').html(value.toFixed(3));
      break;
    case "opacity":
      $('#' + type_num + '_o').html(value.toFixed(3));
      break;
  }


  for (var i = 0; i < kr.compositor_canvas.shapes.length; i++) {
    if (kr.compositor_canvas.shapes[i].type == type
     && (kr.compositor_canvas.shapes[i].num == num)) {
      kr.compositor_canvas.shapes[i].setValue(control_name, value);
      kr.compositor_canvas.valid = false;
      break;
    }
  }
}

Kr.prototype.got_update_subunit = function(num, type,  control_name, value) {
  this.update_subunit(num, type, control_name, value);
}

Kr.prototype.got_control_portgroup = function(portgroup_name,
 control_name, value) {
  if ($('#' + portgroup_name)) {
    if (control_name == "volume") {
      this.ux.portgroup_values[portgroup_name] = value;
      this.ux.portgroup_volume_sliders[portgroup_name].set(value);
    } else {
      this.ux.portgroup_crossfade_sliders[portgroup_name].set(value);
    }
  }
}

Kr.prototype.got_effect_control = function(crate) {
  if (crate.effect_name == "eq") {
    if ($('#' + crate.portgroup_name + '_' + crate.effect_name
     + '_' + crate.effect_num + '_' + crate.control_name)) {
      this.ux.portgroup_eq_sliders[crate.portgroup_name + '_' + crate.effect_name
     + '_' + crate.effect_num + '_' + crate.control_name].set(100/12*crate.value);
    }
  } else {
    if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_'
     + crate.control_name)) {
      var result;
      if (crate.control_name == "hz") {
        if (crate.effect_name == "lowpass") {
          result = 100*Math.log(crate.value/20)/Math.log(600);
        } else if (crate.effect_name == "highpass") {
          result = 100*Math.log(crate.value/20)/Math.log(1000);
        }
      } else if (crate.control_name == "bw") {
        if (crate.effect_name == "lowpass") {
          result = 20 * crate.value;
        } else if (crate.effect_name == "highpass") {
          result = 20 * crate.value;
        }
      } else if (crate.effect_name == "analog") {
        if (crate.control_name == "drive") {
          result = 10 * crate.value;
        } else if (crate.control_name == "blend") {
          result = 10 * crate.value;
        }
      } else {
        result = crate.value;
      }
      this.debug(result);
      this.ux.portgroup_effect_sliders[crate.portgroup_name + '_'
       + crate.effect_name + '_'
       + crate.control_name].set(result);
    }
  }
}

Kr.prototype.got_update_portgroup = function (portgroup_name,
 control_name, value) {
  if (control_name == "crossfade_group") {
    kr.portgroup_handle_crossfade_group(portgroup_name, value, -100.0);
  }

  if (control_name == "xmms2_ipc_path") {
    if (value.length > 0) {
      kr.portgroup_handle_xmms2(portgroup_name, 1);
    } else {
      kr.portgroup_handle_xmms2(portgroup_name, 0);
    }
  }
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

Kr.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {
  if (xmms2 == 0) {
    if ($('#' + portgroup_name + '_xmms2_wrap')) {
      $('#' + portgroup_name + '_xmms2_wrap').remove();
    }
  } else {
	  xmms2_controls = "<div id='" + portgroup_name
      + "_xmms2_wrap' class='xmms2_control'><h2>" + portgroup_name + "</h2><div id='"
     + portgroup_name + "_xmms2'></div></div>";

    $('#xmms2s').append(xmms2_controls);

    /*if (this.xmms2_portgroup_count == 1) {
      $('.kradmixer_beta').prepend(xmms2_controls);
    } else {
      $('.kradmixer_beta').append(xmms2_controls);
    }*/

    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='"
     + portgroup_name + "_xmms2_prev'>PREV</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + portgroup_name
     + "_xmms2_play'>PLAY</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + portgroup_name
     + "_xmms2_pause'>PAUSE</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + portgroup_name
     + "_xmms2_stop'>STOP</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button3' id='" + portgroup_name
     + "_xmms2_next'>NEXT</div></div>");

	  $('#' + portgroup_name + '_xmms2_prev' ).bind("click",
     function(event, ui) {
      kr.xmms2(portgroup_name, "prev");
    });

    $('#' + portgroup_name + '_xmms2_play' ).bind("click",
     function(event, ui) {
      kr.xmms2(portgroup_name, "play");
    });

	  $('#' + portgroup_name + '_xmms2_pause' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "pause");
	  });

    $('#' + portgroup_name + '_xmms2_stop' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "stop");
	  });

	  $('#' + portgroup_name + '_xmms2_next' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "next");
	  });
  }
}

Kr.prototype.got_add_subunit = function (crate) {
  this.comper_got_add_subunit(crate);
  this.subunit_list_got_add_subunit(crate);
}

Kr.prototype.got_remove_subunit = function(crate) {
  this.comper_got_remove_subunit(crate);
  this.subunit_list_got_remove_subunit(crate);
}

Kr.prototype.toggle_show_effects = function(portgroup_name) {
  $('#effects_region_description').html(portgroup_name +" Effects");
  if (this.ux.shown_effects != portgroup_name) {
    if (this.ux.shown_effects != '') {
      $('#' + this.ux.shown_effects + "_effects") .hide(200);
      $('#eq_' + this.ux.shown_effects).hide(200);
    } else {
        $('.kradmixer_cappa').show(200);
    }

    $('#' +portgroup_name + "_effects") .show(200);
    $('#eq_' + portgroup_name).show(200);
    this.ux.shown_effects = portgroup_name;
  } else {
    $('#' +portgroup_name + "_effects") .hide(200);
    $('#eq_' + portgroup_name).hide(200);
    $('.kradmixer_cappa').hide(200);
    this.ux.shown_effects = '';
  }
}

Kr.prototype.got_add_portgroup = function(crate) {
  this.mixer_got_add_portgroup(crate);
}

Kr.prototype.set_mixer_params = function(mixer_params) {
  this.sample_rate = mixer_params.sample_rate;
}

Kr.prototype.got_remove_portgroup = function(name) {
  $('#portgroup_' + name + '_wrap').remove();
}

Kr.prototype.remove_link = function(link_num) {
}

Kr.prototype.got_update_link = function(link_num, control_name, value) {
}

Kr.prototype.update_link = function(link_num, control_name, value) {
}


Kr.prototype.got_add_decklink_device = function() {
}

Kr.prototype.got_remove_link = function(crate) {
}

Kr.prototype.got_add_link = function(crate) {
}
