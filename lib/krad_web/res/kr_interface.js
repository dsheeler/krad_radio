
Kr.prototype.calc_rack = function() {

  this.ux.rack_density = 16;

  var view_width = window.innerWidth;
  var view_height = window.innerHeight;
  var rack_space_sz = view_width / this.ux.rack_density;

  this.ux.rack_width = Math.floor(view_width / rack_space_sz);
  this.ux.rack_width_per = (rack_space_sz / view_width) * 100;
  this.ux.rack_height = Math.floor(view_height / rack_space_sz);
  this.ux.rack_height_per = (rack_space_sz / view_height) * 100;

  kr.debug("rw: " + this.ux.rack_width + "rh: " + this.ux.rack_height);
  kr.debug("rwp: " + this.ux.rack_width_per + "rhp: " + this.ux.rack_height_per);

}


Kr.prototype.first_open_rack_position = function(RCU, positioned_RCUs) {
  var ret = null;
  var fits = 1;
  for (var j = 0; j < kr.ux.rack_height; j++) {
    for (var i = 0; i < kr.ux.rack_width; i++) {
      for (var r = 0; r < positioned_RCUs.length; r++) {
       if (
           ((i >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i)
            && (j >= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j))
           ||
             ((i+RCU.width >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i + RCU.width)
            && (j + RCU.height>= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j+RCU.height))
           ||
             ((i+RCU.width >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i + RCU.width)
            && (j >= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j))
           ||
             ((i >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i)
            && (j + RCU.height>= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j+RCU.height))
            ||
             (i+RCU.width > kr.ux.rack_width || j + RCU.height > kr.ux.rack_height)
           ) {

          fits = 0;
          kr.debug(i + " " +j + " " +  positioned_RCUs[r].x + " " + positioned_RCUs[r].y + " " + positioned_RCUs[r].width + " " + positioned_RCUs[r].height);
          break;
        }
      }
      if (fits == 1) {
        kr.debug('IT FIT ' + i + " " + j);
        return [i, j];
      } else {
        fits = 1;
      }
    }
  }
  kr.debug('NO FIT');
  return null;
}

Kr.prototype.position_RCUs = function() {

  var positioned_RCUs = new Array();
  this.calc_rack();

  for (var i = 0; i < kr.ux.RCUs.length; i++) {

        var result =kr.first_open_rack_position(kr.ux.RCUs[i], positioned_RCUs);
        if (result != null) {
          positioned_RCUs.push(kr.ux.RCUs[i]);
            $(kr.ux.RCUs[i].sel).show();
          kr.ux.RCUs[i].x = result[0];
          kr.ux.RCUs[i].y = result[1];
        } else {
          $(kr.ux.RCUs[i].sel).hide();
        }

        var top = kr.ux.rack_height_per * kr.ux.RCUs[i].y;
        var left = kr.ux.rack_width_per * kr.ux.RCUs[i].x;
        var height = kr.ux.rack_height_per * kr.ux.RCUs[i].height;
        var width = kr.ux.rack_width_per * kr.ux.RCUs[i].width;

        $(kr.ux.RCUs[i].sel).css({
            'top': top + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'left': left + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'width': width + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'height': height + '%'
        });
    }
}
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
      height: 1,
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


function PerspectiveCorrectionView(x0, y0, x1, y1, x2, y2, x3, y3) {
    this.handles = [];
    this.handles[0] = new PerspectiveCorrectionHandle(0, x0, y0, this);
    this.handles[1] = new PerspectiveCorrectionHandle(1, x1, y1, this);
    this.handles[2] = new PerspectiveCorrectionHandle(2, x2, y2, this);
    this.handles[3] = new PerspectiveCorrectionHandle(3, x3, y3, this);
    this.on == false;
}

PerspectiveCorrectionView.prototype.draw = function(ctx, onGhost) {
  if (this.on == true) {
    ctx.save();
    ctx.beginPath();
    ctx.moveTo(this.handles[0].x, this.handles[0].y);
    ctx.lineTo(this.handles[1].x, this.handles[1].y);
    ctx.lineTo(this.handles[3].x, this.handles[3].y);
    ctx.lineTo(this.handles[2].x, this.handles[2].y);
    ctx.closePath();
    ctx.fillStyle = 'rgba(17,119,102,0.5)';
	  ctx.fill();
    ctx.restore();
  }

	/* draw the perspective correction handles */
  l = this.handles.length;
  for (var i = 0; i < l; i++) {
    this.handles[i].draw(ctx, onGhost);
  }
}

function PerspectiveCorrectionHandle(id, x, y, view) {
  this.x = x || 0;
  this.y = y || 0;
  this.id = id || 0;
  this.view = view || null;
  this.selectionHandleColor = '#117766';
  this.selectionHandleSize = 20;
}

PerspectiveCorrectionHandle.prototype.draw = function(ctx, onGhost) {
  ctx.save();
  var half = this.selectionHandleSize / 2;

	if (onGhost == true) {
    ctx.fillStyle = '#fe0' + this.id.toString(16) + 'ff';
    ctx.strokeStyle = '#fe0' + this.id.toString(16) + 'ff';
	} else {
	  ctx.fillStyle = this.selectionHandleColor;
    ctx.strokeStyle = this.selectionHandleColor;
	}

  ctx.lineWidth = 12;
  ctx.lineJoin = 'round';
  ctx.lineCap = 'round';
  ctx.beginPath();

  var id1, id2;
  if (this.id == 0) {
    id1 = 1;
    id2 = 2;
  } else if (this.id == 1) {
    id1 = 3;
    id2 = 0;
  } else if (this.id == 2) {
    id1 = 0;
    id2 = 3;
  } else if (this.id == 3) {
    id1 = 2;
    id2 = 1;
  }

  var aX, aY, A;
  aX = this.view.handles[id1].x - this.x;
  aY = this.view.handles[id1].y - this.y;
  A = Math.sqrt(aX*aX + aY*aY);
  aX = aX/A;
  aY = aY/A;

  ctx.moveTo(this.x + 0.333 * A * aX, this.y + 0.333 * A * aY);
  ctx.lineTo(this.x, this.y);
  aX = this.view.handles[id2].x - this.x;
  aY = this.view.handles[id2].y - this.y;
  A = Math.sqrt(aX*aX + aY*aY);
  aX = aX/A;
  aY = aY/A;
  ctx.lineTo(this.x + 0.333 * A * aX, this.y + 0.333 * A * aY);
  ctx.stroke();
  ctx.restore();
}

function Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {
  /*This is a very simple and unsafe constructor.
    All we're doing is checking if the values exist.
    "x || 0" just means "if there is a value for x, use that.
    Otherwise use 0."
    But we aren't checking anything else!
    We could put "Lalala" for the value of x. */
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
  this.selectionBoxColor = 'darkred';
  this.selectionBoxSize = 20;
  this.selectionColor = '#CC0000';
  this.selectionWidth = 2;
  this.aspectRatio = this.w/this.h;

  this.fill_red = 60;
  this.fill_red_target = 120;
  this.fill_red_target_last = 60;
  this.fill_green = 60;
  this.fill_green_target = 120;
  this.fill_green_target_last = 60;
  this.fill_blue = 60;
  this.fill_blue_target = 120;
  this.fill_blue_target_last = 60;
  this.fill_frame = 0;

  this.PCView = type == 'videoport' ? new PerspectiveCorrectionView(0,0,
   this.w,0,0,this.h,this.w, this.h) : null;

  if (type == 'text') {
    this.h = 40;
    this.xs = 1;
    this.ys = 1;
  }
}

Shape.prototype.setValue = function(name, value) {
  if (name == "x") {
	  this.x = value;
  } else if (name == "y") {
	  this.y = value;
  } else if (name == "height") {
	  this.h = value;
  } else if (name == "width") {
	  this.w = value;
  } else if (name == "xscale") {
	  this.xs = value;
  } else if (name == "yscale") {
	  this.ys = value;
  } else if (name == "rotation") {
	  this.r = Math.PI / 180.0 * value;
  } else if (name == "opacity") {
	  this.o = value;
  }
}

/* Draws this shape to a given context. */
Shape.prototype.draw = function(ctx, onGhost) {

  ctx.save();

  ctx.translate(this.x, this.y);
  ctx.scale(this.xs, this.ys);
  ctx.translate(-this.x, -this.y);

  ctx.translate(this.x, this.y);
  ctx.translate(this.w*0.5, this.h*0.5);
  ctx.rotate(this.r);
  ctx.translate(-this.w*0.5, -this.h*0.5);
  ctx.translate(-this.x, -this.y);

  if (onGhost) {
	  ctx.fillStyle = '#000000';
  } else {
    if (this.selected == true) {
      this.fill_frame = this.fill_frame +1;
      if (this.fill_frame > 200) {
        this.fill_frame = 0;
        this.fill_red = this.fill_red_target;
        this.fill_green = this.fill_green_target;
        this.fill_blue = this.fill_blue_target;

        this.fill_red_target_last = this.fill_red_target;
        this.fill_red_target = 255*Math.random();
        this.fill_green_target_last = this.fill_green_target;
        this.fill_green_target = 255*Math.random();
        this.fill_blue_target_last = this.fill_blue_target;
        this.fill_blue_target = 255*Math.random();
      }
      this.fill_red = Math.round(this.fill_red_target_last+ this.fill_frame/200 *(this.fill_red_target - this.fill_red_target_last));
      this.fill_green = Math.round(this.fill_green_target_last+ this.fill_frame/200 *(this.fill_green_target - this.fill_green_target_last));
      this.fill_blue = Math.round(this.fill_blue_target_last+ this.fill_frame/200 *(this.fill_blue_target - this.fill_blue_target_last));
      ctx.fillStyle = 'rgb(' + this.fill_red + ',' + this.fill_green + ',' + this.fill_blue + ')';
      kr.debug(ctx.fillStyle);
    } else {
       ctx.fillStyle = this.fill;
    }
  }
  ctx.fillRect(this.x, this.y, this.w, this.h);

  ctx.strokeStyle = '#5588cc';
  ctx.lineWidth = this.selectionWidth;
  ctx.strokeRect(this.x,this.y,this.w,this.h);

  /*Draw selection which is a stroke along the box
     and also selection handles. */
  this.selectionBoxSize = 0.05 * (this.h > this.w ? this.w : this.h);
  if (this.selected == true) {
    var half = this.selectionBoxSize / 2;
	  ctx.strokeStyle = this.selectionColor;
	  ctx.lineWidth = this.selectionWidth;
	  ctx.strokeRect(this.x,this.y,this.w,this.h);
	  ctx.beginPath();
	  ctx.moveTo(this.x+this.w/2, this.y);
	  ctx.lineTo(this.x+0.5*this.w, this.y-3*this.selectionBoxSize);
	  ctx.stroke();
	  /*Draw the handles.
	    0  1  2
	    3     4
	    5  6  7*/

	  /*Top left, middle, and right handles.*/
	  kr.selectionHandles[0].x = this.x;
	  kr.selectionHandles[0].y = this.y;

	  kr.selectionHandles[1].x = this.x+this.w/2;
	  kr.selectionHandles[1].y = this.y;

	  kr.selectionHandles[2].x = this.x+this.w;
	  kr.selectionHandles[2].y = this.y;

	  /*Middle left handle.*/
	  kr.selectionHandles[3].x = this.x;
	  kr.selectionHandles[3].y = this.y+this.h/2;

	  /*Middle right handle.*/
	  kr.selectionHandles[4].x = this.x+this.w;
	  kr.selectionHandles[4].y = this.y+this.h/2;

    /*Bottom left, middle, and right handles.*/
    kr.selectionHandles[6].x = this.x+this.w/2;
    kr.selectionHandles[6].y = this.y+this.h;

    kr.selectionHandles[5].x = this.x;
    kr.selectionHandles[5].y = this.y+this.h;

    kr.selectionHandles[7].x = this.x+this.w;
    kr.selectionHandles[7].y = this.y+this.h;

    /*Rotation handle.*/
    kr.selectionHandles[8].x = this.x+0.5*this.w;
    kr.selectionHandles[8].y = this.y- 3*this.selectionBoxSize;

    /*Opacity handle.*/
    kr.selectionHandles[9].x = this.x+0.5*this.w;
    kr.selectionHandles[9].y = this.y+0.5*this.h;

    for (var i = 0; i < 10; i ++) {
      var cur = kr.selectionHandles[i];
      ctx.beginPath();
      ctx.arc(cur.x,cur.y,this.selectionBoxSize,0,2*Math.PI);
      if (onGhost == true) {
        ctx.fillStyle = '#ff0' + i.toString(16) + 'ff';
        ctx.fill();
      } else {
		    ctx.fillStyle = this.selectionBoxColor;
		    ctx.fill();
	    }
	  }
  }

  var xpad = 5;
  ctx.fillStyle = '#000000';

  var lineHeight = 2*this.selectionBoxSize;

  ctx.font = lineHeight.toString() + "px Ubuntu";
  lineHeight = parseInt(ctx.font.match(/^(\d+)/)[0]);

  var tmpy = this.y + lineHeight + this.selectionBoxSize;

  ctx.fillText(this.type + " " + this.num, this.x, tmpy);
  tmpy += lineHeight;

  /*lineHeight = lineHeight/3;

  ctx.font = lineHeight.toString() + "px Ubuntu";

  lineHeight = parseInt(ctx.font.match(/^(\d+)/)[0]);

  ctx.fillText("x: " + this.x.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("y: " + this.y.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("height: " + this.h.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("width: " + this.w.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("rotation: " + this.r.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;
  ctx.fillText("opacity: " + this.o.toFixed(2), this.x + xpad, tmpy);

  tmpy += lineHeight;
  ctx.fillText("xscale: " + this.xs.toFixed(2), this.x + xpad, tmpy);

  tmpy += lineHeight;
  ctx.fillText("yscale: " + this.ys.toFixed(2), this.x + xpad, tmpy);*/

  if (this.PCView) {
    ctx.translate(this.x, this.y);
    this.PCView.draw(ctx, onGhost);
  }

  ctx.restore();
}

function CanvasState(canvas, ghostcanvas) {
  /*First some setup!*/
  this.canvas = canvas;
  this.width = canvas.width;
  this.height = canvas.height;
  this.ctx = canvas.getContext('2d');

  this.ghostcanvas = ghostcanvas;
  this.ghostcanvas.height = canvas.height;
  this.ghostcanvas.width = canvas.width;
  this.gctx = ghostcanvas.getContext('2d');

  /*This complicates things a little but
     fixes mouse co-ordinate problems
     when there's a border or padding.
    See getMouse for more detail.*/

  var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
  if (document.defaultView && document.defaultView.getComputedStyle) {
	  this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['paddingLeft'], 10) || 0;
	  this.stylePaddingTop = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['paddingTop'], 10) || 0;
	  this.styleBorderLeft = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['borderLeftWidth'], 10) || 0;
	  this.styleBorderTop = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['borderTopWidth'], 10)   || 0;
  }

  /*Some pages have fixed-position bars (like the stumbleupon bar)
     at the top or left of the page.
    They will mess up mouse coordinates and this fixes that.*/

  var html = document.body.parentNode;
  this.htmlTop = html.offsetTop;
  this.htmlLeft = html.offsetLeft;

  /*Keep track of state!*/
  /*When set to false, the canvas will redraw everything.*/
  this.valid = false;
  /*The collection of things to be drawn.*/
  this.shapes = [];
  /*Keep track of when we are dragging.*/
  this.dragging = false;
  this.resizeDragging = false;
  this.perspectiveCorrectionDragging = false;
  this.expectResize = -1;
	this.expectMovePerspectiveCorrectionHandle = -1;
  /*The current selected object.
    In the future we could turn this into an array for multiple selection.*/
  this.selection = null;

  /*See mousedown and mousemove events for explanation.*/
  this.dragoffx = 0;
  this.dragoffy = 0;
  this.alt = false;

  /*Then events!*/

  /*This is an example of a closure!
    Right here "this" means the CanvasState.
    But we are making events on the Canvas itself,
     and when the events are fired on the canvas the variable
     "this" is going to mean the canvas!
    Since we still want to use this particular CanvasState
     in the events we have to save a reference to it.
    This is our reference!*/
  var myState = this;

  /*Fixes a problem where double clicking causes text
     to get selected on the canvas.*/
  canvas.addEventListener('selectstart', function(e) {
    e.preventDefault(); return false;
  }, false);

  canvas.addEventListener('mousedown', function(e) {
    var mouse = myState.getMouse(e);
    var mx = mouse.x;
    var my = mouse.y;
    var shapes = myState.shapes;
	  var l = shapes.length;
    var mySel = null;

    if (myState.expectMovePerspectiveCorrectionHandle !== -1) {
      myState.perspectiveCorrectionDragging = true;
      return;
    }

	  if (myState.expectResize !== -1) {
	    myState.resizeDragging = true;
	    return;
	  }

    for (var i = l-1; i >= 0; i--) {
      shapes[i].selected = false;
        $('#' + shapes[i].type + '_' + shapes[i].num).removeClass('selected');
    }

    myState.clearGhost();
    for (var i = l-1; i >= 0; i--) {
      /*Draw shape onto ghost context.*/
	    shapes[i].draw(myState.gctx, true);

	    /*Get image data at the mouse x,y pixel.*/
	    var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	    /*If the mouse pixel exists, select and break.*/
	    if (imageData.data[3] > 0) {
		    mySel = shapes[i];
		    myState.dragoffx = mx - mySel.x;
		    myState.dragoffy = my - mySel.y;
		    myState.dragging = true;
		    mySel.selected = true;
  		  myState.selection = mySel;
		    myState.valid = false;
		    myState.clearGhost();
		    $('#' + mySel.type + '_' + mySel.num).addClass('selected');
		    return;
	    }
	  }

    myState.clearGhost();

    if (myState.selection) {
      myState.selection.selected = false;
      $('#' + myState.selection.type + '_'
       + myState.selection.num).removeClass('selected');
      myState.selection = null;
      /*Need to clear the old selection border.*/
      myState.valid = false;
	  }
  }, true);

  canvas.addEventListener('mousemove', function(e) {
	  if (myState.dragging) {
      /*Something's dragging so we must redraw.*/
	    myState.valid = false;
	    var mouse = myState.getMouse(e);
	    /*We don't want to drag the object by its top-left corner,
         we want to drag it from where we clicked.
        Thats why we saved the offset and use it here.*/
      myState.selection.x = mouse.x - myState.dragoffx;
	    myState.selection.y = mouse.y - myState.dragoffy;
	    kr.comp_ctrl(myState.selection.num,
       myState.selection.type, "x", myState.selection.x);
	    kr.comp_ctrl(myState.selection.num,
       myState.selection.type, "y", myState.selection.y);
	    return;
    } else if (myState.perspectiveCorrectionDragging) {
      var mouse = myState.getMouse(e);
      var mx = mouse.x;
      var my = mouse.y;

      mx = mx - myState.selection.x - myState.selection.w*0.5;
      my = my - myState.selection.y - myState.selection.h*0.5;
      mx = Math.cos(myState.selection.r) * mx
       + Math.sin(myState.selection.r) * my;
      my = -Math.sin(myState.selection.r) * mx
       + Math.cos(myState.selection.r) * my;

      myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].x
       = mx/myState.selection.xs + myState.selection.w*0.5;
      myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].y
       = my/myState.selection.ys + myState.selection.h*0.5;
      kr.debug(myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].x
       + " " + myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].y);

      myState.valid = false;
      return;

	  } else if (myState.resizeDragging) {
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;
	    var oldx = myState.selection.x;
	    var oldy = myState.selection.y;
	    var shapes = myState.shapes;
	    var l = shapes.length;

      /*0  1  2
        3     4
        5  6  7*/
      switch (myState.expectResize) {
	      case 0:
          var xscale = (myState.selection.xs * myState.selection.w + oldx - mx) / myState.selection.w ;
          var yscale = (myState.selection.ys * myState.selection.h + oldy - my) / myState.selection.h ;
          var y;
          var x;

          if (xscale > yscale) {
            yscale = xscale;
            y = oldy + myState.selection.h * myState.selection.ys - yscale * myState.selection.h;
            x = mx;
          } else {
            xscale = yscale;
            x = oldx + myState.selection.w * myState.selection.xs - xscale * myState.selection.w;
            y = my;
          }

          kr.comp_ctrl(myState.selection.num, myState.selection.type, "xscale", xscale);
          kr.comp_ctrl(myState.selection.num, myState.selection.type, "x", x);

          myState.selection.xs = xscale;
          myState.selection.x = x;

          kr.comp_ctrl(myState.selection.num, myState.selection.type, "yscale", xscale);
          kr.comp_ctrl(myState.selection.num, myState.selection.type, "y", y);

          myState.selection.ys = xscale;
          myState.selection.y = y;
          break;

        case 1:
	  	    var ys = (myState.selection.ys * myState.selection.h + oldy - my)
           / myState.selection.h ;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", ys);
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "y", my);

          myState.selection.ys = ys;
          myState.selection.y = my;

          break;
        case 2:
          var yscale = (myState.selection.ys * myState.selection.h + oldy - my)
           / myState.selection.h ;
          var xscale = (mx - oldx) / myState.selection.w;
          var y;
          if (xscale > yscale) {
            yscale = xscale;
            y = oldy + myState.selection.h * myState.selection.ys
             - yscale * myState.selection.h;
          } else {
            xscale = yscale;
            y = my;
          }
          myState.selection.y = y;
          myState.selection.ys = xscale;

          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "y", y);
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale);
          myState.selection.xs = xscale;
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    break;
	      case 3:
	  	    var xs = (myState.selection.xs * myState.selection.w + oldx - mx)
           / myState.selection.w ;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xs);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "x", mx);
          myState.selection.xs = xs;
          myState.selection.x = mx;
	  	    break;
	  	  case 4:
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", (mx - oldx) / myState.selection.w);
          myState.selection.xs = (mx - oldx) / myState.selection.w;
          break;

        case 5:
          var xscale = (myState.selection.xs * myState.selection.w + oldx - mx)
           / myState.selection.w ;
          var yscale = (my - oldy) / myState.selection.h;
          var x;

          if (xscale > yscale) {
            x = mx;
          } else {
            xscale = yscale;
            x = oldx + myState.selection.w * myState.selection.xs
             - xscale * myState.selection.w;
          }
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "x", x);

	  	    myState.selection.xs = xscale;
	  	    myState.selection.x = x;

	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale);
	  	    myState.selection.ys = xscale;
	  	    break;

	      case 6:
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", (my - oldy) / myState.selection.h);
	  	    myState.selection.ys = (my - oldy) / myState.selection.h;
	  	    break;
	      case 7:
          var xscale = (mx - oldx) / myState.selection.w;
          var yscale = (my - oldy) / myState.selection.h;
          xscale = xscale > yscale ? xscale : yscale;

          myState.selection.xs = myState.selection.ys = xscale;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale);
	  	    break;
	      case 8:
	  	    var angle = Math.atan2((mx - (myState.selection.x + 0.5
           * myState.selection.xs*myState.selection.w)),
           -(my - (myState.selection.y+0.5*myState.selection.ys
           * myState.selection.h)));
	  	    myState.selection.r = angle;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "rotation", angle *180/Math.PI);
	  	    break;
	      case 9:
	  	    myState.selection.o = -0.5+2*(myState.height-mouse.y)/myState.height;
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "opacity", myState.selection.o);
	  	    break;
	    }

	    myState.valid = false;
	  }

	  if (myState.selection !== null && !myState.perspectiveCorrectionDragging) {
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;

      /*Look for mouse on a perspective correction handle.*/
      myState.clearGhost();

	    myState.selection.draw(myState.gctx, true);

      /*Get image data at the mouse x,y pixel.*/
	    var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	    if (imageData.data[0] == 254) {
	  	  myState.expectMovePerspectiveCorrectionHandle = imageData.data[1];
	  	  myState.valid = false;
        this.style.cursor='move';
	  	  myState.clearGhost();
	  	  return;
	    }
      this.style.cursor='auto';
      myState.expectMovePerspectiveCorrectionHandle = -1;
	    myState.clearGhost();
    }
    if (myState.selection !== null && !myState.resizeDragging) {
	    myState.clearGhost();
	    var found = false;
	    var shapes = myState.shapes;
	    var l = shapes.length;
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;

	    for (var i = l-1; i >= 0; i--) {
	  	  /*Draw shape onto ghost context.*/
	  	  shapes[i].draw(myState.gctx, true);

	  	  /*Get image data at the mouse x,y pixel.*/
	  	  var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	  	  /*If the mouse pixel exists, select and break.*/
	  	  if (imageData.data[0] == 255) {
	  	    myState.expectResize = imageData.data[1];
	  	    myState.clearGhost();
	  	    myState.valid = false;

	  	    /*We found one!*/
	  	    switch (myState.expectResize) {
	  	      case 0:
	  		      this.style.cursor='nw-resize';
	  		      break;
	  	      case 1:
	  		      this.style.cursor='n-resize';
	  		      break;
	  	      case 2:
	  		      this.style.cursor='ne-resize';
	  		      break;
	  	      case 3:
	  		      this.style.cursor='w-resize';
	            break;
	  	      case 4:
	  		      this.style.cursor='e-resize';
	  		      break;
	  	      case 5:
	  		      this.style.cursor='sw-resize';
	  		      break;
	  	      case 6:
	  		      this.style.cursor='s-resize';
	  		      break;
	  	      case 7:
	  		      this.style.cursor='se-resize';
	  		      break;
	  	      case 8:
	  		      this.style.cursor='move';
	  		      break;
            case 9:
              this.style.cursor='move';
	            break;
	        }
	  	    return;
	  	  }
	    }
      /*Not over a selection box, return to normal.*/
	    myState.resizeDragging = false;
	    myState.expectResize = -1;
	    this.style.cursor='auto';
	  }
  }, true);

  canvas.addEventListener('mouseup', function(e) {
    myState.perspectiveCorrectionDragging = false;
    myState.expectMovePerspectiveCorrectionHandle = -1;
    myState.dragging = false;
    myState.resizeDragging = false;
  }, true);

  this.interval = 100;
  (function animloop(){
	  requestAnimFrame(animloop);
	  myState.draw();
  })();
}

CanvasState.prototype.addShape = function(shape) {
  this.shapes.push(shape);
  this.valid = false;
}

CanvasState.prototype.removeShape = function(subunit_type, subunit_id) {
  var l = this.shapes.length;
  for (var i = 0; i < l; i++) {
    if (this.shapes[i].num == subunit_id
     && this.shapes[i].type == subunit_type) {
      this.shapes.splice(i, 1);
      this.valid = false;
      return;
    }
  }
}

CanvasState.prototype.clear = function() {
  this.ctx.clearRect(0, 0, this.width, this.height);
}

CanvasState.prototype.clearGhost = function() {
  this.gctx.clearRect(0, 0, this.width, this.height);
}

/*While draw is called as often as the INTERVAL variable demands,
   it only ever does something if the canvas gets invalidated by our code.*/
CanvasState.prototype.draw = function() {
  /*If our state is invalid, redraw and validate!*/
  if (!this.valid) {
    var ctx = this.ctx;
    var shapes = this.shapes;
    this.clear();

    /*Add stuff you want drawn in the background all the time here.*/

	  /*Draw all shapes.*/
	  var l = shapes.length;
	  for (var i = 0; i < l; i++) {
	    var shape = shapes[i];
      shape.draw(ctx);
    }

    /*Add stuff you want drawn on top all the time here.*/
	  this.valid = false;
  }
}

/*Creates an object with x and y defined, set to the mouse position
   relative to the state's canvas if you wanna be super-correct
   this can be tricky, we have to worry about padding and borders.*/
CanvasState.prototype.getMouse = function(e) {
  var element = this.canvas, offsetX = 0, offsetY = 0, mx, my;
  /*Compute the total offset.*/
  if (element.offsetParent !== undefined) {
    do {
      offsetX += element.offsetLeft;
      offsetY += element.offsetTop;
    } while (element = element.offsetParent);
  }

  /*Add padding and border style widths to offset.
    Also add the <html> offsets in case there's a position:fixed bar*/
  offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
  offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;
  mx = e.pageX - offsetX;
  my = e.pageY - offsetY;

  /*We return a simple javascript object (a hash) with x and y defined.*/
  return {x: mx, y: my};
}

Kr.prototype.got_system_cpu_usage = function(system_cpu_usage) {
  this.system_cpu_usage = system_cpu_usage;
  $('.system_cpu_usage').html(this.system_cpu_usage);
}

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


Kr.prototype.calc_rack = function() {

  this.ux.rack_density = 16;

  var view_width = window.innerWidth;
  var view_height = window.innerHeight;
  var rack_space_sz = view_width / this.ux.rack_density;

  this.ux.rack_width = Math.floor(view_width / rack_space_sz);
  this.ux.rack_width_per = (rack_space_sz / view_width) * 100;
  this.ux.rack_height = Math.floor(view_height / rack_space_sz);
  this.ux.rack_height_per = (rack_space_sz / view_height) * 100;

  kr.debug("rw: " + this.ux.rack_width + "rh: " + this.ux.rack_height);
  kr.debug("rwp: " + this.ux.rack_width_per + "rhp: " + this.ux.rack_height_per);

}


