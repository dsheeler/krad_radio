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
  width = 42;
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

  if ((crate.subunit_type != 'videoport') || (crate.direction != 'output')) {
    var shape = new Shape(crate.x,crate.y, crate.width,
     crate.height, crate.r * Math.PI / 180.0, crate.o,
     crate.xscale, crate.yscale,
     crate.subunit_type, crate.subunit_id);

    this.debug(crate);
    kr.compositor_canvas.addShape(shape);

    var type_num = crate.subunit_type + "_" + crate.subunit_id;

    $('#subunit_list').append("<li class=subunit id='" + type_num + "'>" + crate.subunit_type
     + " " + crate.subunit_id + " <div class='subunit_buttons_wrap' style='float: left, width: 33%'>"
     + "    <div class='button_wrap'>"
     + "<div class='krad_button_very_small' id='fade_"
     + type_num + "'>FADE</div></div><div class='button_wrap'>"
     + "<div class='krad_button_very_small' id='move_to_origin_"
     + type_num + "'>(0,0)</div></div>"
     + "<div class='button_wrap'><div class='krad_button_very_small' id='reset_w_h_"
     + type_num + "'>ORIG W/H</div></div>"
     + "<div class='button_wrap'><div class='krad_button_very_small happy_button'"
     + "id='happy_button_"
     + type_num + "'>HAPPY</div></div>"
     + "</div></li>");


    if (crate.subunit_type == "sprite") {
       var paths = crate.filename.split("/");
       var path_tail = paths[paths.length-1];
       $('#' + type_num).append(" <div style='display: inline' id='" + type_num
        + "_filename'>'" + path_tail + "'</div>");

        $('#' + type_num).append(" rate: <div style='display: inline' id='" + type_num
        + "_rate'>" + crate.rate + "</div>");
     }
     if (crate.subunit_type == "text") {
       $('#' + type_num).append(" text: <div style='display: inline' id='" + type_num
        + "_text'>''" + crate.text + "''</div>");
     }

     $("#" + type_num).append(

      " x: <div style='display: inline' id='" + type_num + "_x'>" + crate.x + "</div>"
      + " y: <div style='display: inline' id='" + type_num + "_y'>" + crate.y + "</div>"
      + " width: <div style='display: inline' id='" + type_num + "_w'>" + crate.width + "</div>"
      + " height: <div style='display: inline' id='" + type_num + "_h'>" + crate.height + "</div>"
      + " rotation: <div style='display: inline' id='" + type_num + "_r'>" + crate.r.toFixed(3) + "</div>"
      + " opacity: <div style='display: inline' id='" + type_num + "_o'>" + crate.o.toFixed(2) + "</div>"
     );

    $('li.subunit:even').removeClass("alt");
    $('li.subunit:odd').addClass("alt");


     $('#reset_w_h_' + type_num).on("click", {w: crate.w, h: crate.h}, function(e) {
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "xscale", crate.xscale, 60);
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "yscale", crate.yscale, 60);
      return false;
    });

    $('#happy_button_' + type_num).on("click", function() {
      var x = 960*Math.random();
      var y = 540*Math.random();
      var o = Math.random();
      var r = 360*Math.random();
      var xs = 3*Math.random();
      var ys = 3*Math.random();
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "x", x, 120*Math.random());
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "y", y, 120*Math.random());
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "o", o, 120*Math.random());
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "r", r, 120*Math.random());
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "xscale", xs, 120*Math.random());
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "yscale", ys, 120*Math.random());
      return false;
    });

    $('#move_to_origin_' + type_num).on("click", function() {
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "x", 0, 60);
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "y", 0, 60);
      return false;
    });

    $('#fade_' + type_num).bind("click", function() {
      var new_value = 0;
      if (kr.ux.opacity_toggle[type_num] == 0) {
        new_value = 1;
        kr.ux.opacity_toggle[type_num] = 1;
      } else {
        new_value = 0;
        kr.ux.opacity_toggle[type_num] = 0;
      }
      kr.comp_ctrl(crate.subunit_id, crate.subunit_type, "o", new_value, 60);
      return false;
	  });

    $('#' + shape.type + '_' + shape.num).bind("click", function(e,ui) {
	    e.preventDefault();
      var alreadySelected = $(this).hasClass('selected');
      var shapes = kr.compositor_canvas.shapes;
      var l = shapes.length;

      /*Run removeClass on every element.*/
	    $('.subunit').removeClass('selected');
	    for (var i = 0; i < l;  i++) {
	      shapes[i].selected = false;
	      if (shapes[i].num == shape.num && shapes[i].type == shape.type) {
		      shapes[i].selected = !alreadySelected;
          if (!alreadySelected) {
            kr.compositor_canvas.selection = shapes[i];
          }
        }
      }
      kr.compositor_canvas.valid = false;
      if (!alreadySelected) {
        $(this).addClass('selected');
      }
    });
  }
}

Kr.prototype.got_remove_subunit = function(crate) {
  kr.compositor_canvas.removeShape(crate.subunit_type,
   crate.subunit_id);

  $('#' + crate.subunit_type + '_' + crate.subunit_id).remove();
  $('li.subunit:even').removeClass("alt");
  $('li.subunit:odd').addClass("alt");

  kr.compositor_canvas.valid = false;
}

Kr.prototype.toggle_show_effects = function(portgroup_name) {
  $('#effects_region_description').replaceWith("<h2 id='effects_region_description'>"
     + portgroup_name +" Effects</h2>");
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

  /*
      $('#' +portgroup_name + "_effects") .show(200);
    $('#eq_' + portgroup_name).show(200);
  $('.effect_area').hide(200);
  $('.eq_area').hide(200);
  $('#effects_region_description').replaceWith("<h2 id='effects_region_description'>"
   + "Audio Effects</h2>");
  if (portgroup_name) {
    $('#effects_region_description').replaceWith("<h2 id='effects_region_description'>"
     + portgroup_name +" Effects</h2>");
    $('#' +portgroup_name + "_effects") .show(200);
    $('#eq_' + portgroup_name).show(200);
  }
}
*/
Kr.prototype.got_add_portgroup = function(crate) {
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
      kr.debug(min_selectors[i]);
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


    $('#dtmf_1').bind("click",
     function(event, ui) {
      kr.push_dtmf ("1");
    });

  	$('#dtmf_2').bind("click",
     function(event, ui) {
  	  kr.push_dtmf ("2");
	  });

	  $('#dtmf_3').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("3");
	  });

	  $('#dtmf_4').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("4");
	  });

	  $('#dtmf_5').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("5");
	  });

	  $('#dtmf_6').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("6");
	  });

	  $('#dtmf_7').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("7");
	  });

	  $('#dtmf_8').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("8");
	  });

	  $('#dtmf_9').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("9");
	  });

	  $('#dtmf_0').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("0");
	  });

	  $('#dtmf_star').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("*");
	  });

    $('#dtmf_hash').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("#");
	  });

	  $('#dtmf_a').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("A");
	  });

	  $('#dtmf_b').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("B");
	  });

	  $('#dtmf_c').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("C");
	  });

	  $('#dtmf_d').bind("click",
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

    $('.kradmixer_cappa').append("<div class='effect_area' "
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
    $('.kradmixer_cappa').append(
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

  this.portgroup_draw_peak_meter(crate.portgroup_name, 0);

}

Kr.prototype.set_mixer_params = function(mixer_params) {

  this.sample_rate = mixer_params.sample_rate;/*
  $('.kradmixer').prepend("<div><h2>Mixer Sample Rate: "
   + this.sample_rate + "</h2> </div>");
*/
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
