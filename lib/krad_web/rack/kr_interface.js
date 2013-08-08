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
      sel: '.effects',
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
<div class='effects RCU'></div>\
<div id='kradmixer' class='RCU kradmixer'>\
<div id='mixers'>\
<div class='analog_meter_wrapper' style='float: left'>\
<canvas class='meter' id='analog_meter_canvas'>\
</canvas></div><div class='maximized_portgroup_wraps'></div>\
<div style='float: right' class='minimized_portgroup_wraps'></div>\
</div>\
</div>\
</div>");

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

  kr.compositor_canvas = new CanvasState(document.getElementById
  ("compositor_canvas"), document.createElement('canvas'));

  $('.effects .close').bind('click', function() {
    kr.toggle_show_effects();
  });

  $('.effects .close').mouseenter(function() {
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



Kr.prototype.kradcompositor_controls = function () {
  $('.kradcompositor').append("<br clear='both'/>");
}


Kr.prototype.got_tag = function (tag_item, tag_name, tag_value) {
 
}


Kr.prototype.got_peak_portgroup = function(portgroup_name, value) {
  this.mixer_got_peak_portgroup(portgroup_name, value);
}

Kr.prototype.got_update_subunit = function(num, type,  control_name, value) {
  kr.subunit_list_update_subunit(num, type, control_name, value);
  kr.comper_update_subunit(num, type, control_name, value); 
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
        $('.effects').show(200);
    }

    $('#' +portgroup_name + "_effects") .show(200);
    $('#eq_' + portgroup_name).show(200);
    this.ux.shown_effects = portgroup_name;
  } else {
    $('#' +portgroup_name + "_effects") .hide(200);
    $('#eq_' + portgroup_name).hide(200);
    $('.effects').hide(200);
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
