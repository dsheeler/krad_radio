var rack_units = [];/*{
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
    }];*/


Kr.prototype.load_interface = function() {
	this.ux = new Rack();
}

function Rack() {
  this.units = [];
  /*this.portgroup_volume_sliders = new Object();
  this.portgroup_crossfade_sliders = new Object();
  this.portgroup_effect_sliders = new Object();
  this.portgroup_eq_sliders = new Object();
  this.portgroup_values = new Object();
  this.crossfades = new Object();
  this.portgroup_volume_toggle = new Object();
  this.opacity_toggle = new Object();
  this.screen = new Object();
  this.rack_control_units = new Array();
  this.shown_effects = '';
  this.background_color_backups = new Object();
  this.sysname = "";
  this.sample_rate = 0;
  this.peaks = new Array();
  this.peaks_max = new Array();
  this.system_cpu_usage = 0;
  this.xmms2_portgroup_count = 0;
  this.compositor_canvas = null;*/
	
  this.calc_rack();
  /* set up the selection handle boxes */
  /*this.selectionHandles = [];

  for (var i = 0; i < 10; i ++) {
    var rect = new Shape;
    this.selectionHandles.push(rect);
  }*/
}

Rack.prototype.setup_rack = function(info_object) {
  /* loop through global rack_units array and call constructors */
  for (var i = 0; i < rack_units.length; i++) {
    unit = new rack_units[i]['constructor'](info_object);  
    unit.width = rack_units[i]['aspect'][0];
    unit.height = rack_units[i]['aspect'][1];
    this.units.push(unit);
  }
}

Rack.prototype.calc_rack = function() {

  this.rack_density = 32;

  var view_width = window.innerWidth;
  var view_height = window.innerHeight;
  var rack_space_sz = view_width / this.rack_density;

  this.rack_width = Math.floor(view_width / rack_space_sz);
  this.rack_width_per = (rack_space_sz / view_width) * 100;
  this.rack_height = Math.floor(view_height / rack_space_sz);
  this.rack_height_per = (rack_space_sz / view_height) * 100;
}

Rack.prototype.destroy = function() {
  $('#' + this.sysname).remove();
}

Rack.prototype.got_sysname = function(sysname) {
  this.sysname = sysname;
  $('#kradradio').append("<div class='kradradio_station' id='"
   + this.sysname + "'></div>");
  
  this.setup_rack({'parent_div': $('#' + this.sysname)});
  this.position_units();

  window.onresize = function(event) {
    kr.ux.position_units();
  }
}

/*Rack.prototype.old_got_sysname = function(sysname) {
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
<div id='kradmixer' class='RCU kradmixer'>\
<div id='mixers'>\
<div class='analog_meter_wrapper' style='float: left'>\
<canvas class='meter' id='analog_meter_canvas'>\
</canvas></div><div class='maximized_portgroup_wraps'></div>\
<div style='float: right' class='minimized_portgroup_wraps'></div>\
</div>\
</div>\
</div>");

  this.position_units();

  window.onresize = function(event) {
    kr.ux.position_units();
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
}*/

Rack.prototype.first_open_rack_position = function(RCU, positioned_units) {
  var ret = null;
  var fits = 1;
  for (var j = 0; j < this.rack_height; j++) {
    for (var i = 0; i < this.rack_width; i++) {
      for (var r = 0; r < positioned_units.length; r++) {
       if (
           ((i >= positioned_units[r].x && positioned_units[r].x + positioned_units[r].width > i)
            && (j >= positioned_units[r].y && positioned_units[r].y + positioned_units[r].height > j))
           ||
             ((i+RCU.width >= positioned_units[r].x && positioned_units[r].x + positioned_units[r].width > i + RCU.width)
            && (j + RCU.height>= positioned_units[r].y && positioned_units[r].y + positioned_units[r].height > j+RCU.height))
           ||
             ((i+RCU.width >= positioned_units[r].x && positioned_units[r].x + positioned_units[r].width > i + RCU.width)
            && (j >= positioned_units[r].y && positioned_units[r].y + positioned_units[r].height > j))
           ||
             ((i >= positioned_units[r].x && positioned_units[r].x + positioned_units[r].width > i)
            && (j + RCU.height>= positioned_units[r].y && positioned_units[r].y + positioned_units[r].height > j+RCU.height))
            ||
             (i+RCU.width > this.rack_width || j + RCU.height > this.rack_height)
           ) {

          fits = 0;
          break;
        }
      }
      if (fits == 1) {
        return [i, j];
      } else {
        fits = 1;
      }
    }
  }
  return null;
}

Rack.prototype.position_units = function() {

  var positioned_units = new Array();
  this.calc_rack();

  for (var i = 0; i < this.units.length; i++) {

        var result = this.first_open_rack_position(this.units[i], positioned_units);
        if (result != null) {
          positioned_units.push(this.units[i]);
          $(this.units[i].sel).show();
          this.units[i].x = result[0];
          this.units[i].y = result[1];
        } else {
          $(this.units[i].sel).hide();
        }

        var top = this.rack_height_per * this.units[i].y;
        var left = this.rack_width_per * this.units[i].x;
        var height = this.rack_height_per * this.units[i].height;
        var width = this.rack_width_per * this.units[i].width;

        $(this.units[i].sel).css({
            'top': top + '%'
        });
        $(this.units[i].sel).css({
            'left': left + '%'
        });
        $(this.units[i].sel).css({
            'width': width + '%'
        });
        $(this.units[i].sel).css({
            'height': height + '%'
        });
    }
}

Rack.prototype.on_crate = function(crate) {
  if (crate.com == "kradmixer") {
    for (var i=0; i < this.units.length; i++) {
      if (this.units[i].address_masks[0] == "kradmixer") {
   
    if (crate.ctrl == "peak_portgroup") {
      this.units[i].got_peak_portgroup(crate.portgroup_name, crate.value);
      return;
    }
    if (crate.ctrl == "control_portgroup") {
      this.units[i].got_control_portgroup(crate.portgroup_name,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "effect_control") {
      this.units[i].got_effect_control(crate);
      return;
    }
    if (crate.ctrl == "update_portgroup") {
      this.units[i].got_update_portgroup(crate.portgroup_name,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "add_portgroup") {
      this.units[i].got_add_portgroup(crate);
      return;
    }
    if (crate.ctrl == "remove_portgroup") {
      this.units[i].got_remove_portgroup(crate.portgroup_name);
      return;
    }
    if (crate.ctrl == "set_mixer_params") {
      this.units[i].set_mixer_params(crate);
      return;
    }
  }
    }
  }
  if (crate.com == "kradcompositor") {
    for (var i=0; i < this.units.length; i++) {
    if (this.units[i].address_masks[0] == "kradcompositor") {
    if (crate.ctrl == "add_subunit") {
      kr.debug("adding to " + this.units[i].description);
      this.units[i].got_add_subunit(crate);
    }
    if (crate.ctrl == "remove_subunit") {
      this.units[i].got_remove_subunit(crate);
      return;
    }
    if (crate.ctrl == "update_subunit") {
      this.units[i].got_update_subunit(crate.subunit_id, crate.subunit_type,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "set_frame_rate") {
      this.units[i].got_frame_rate(crate.numerator, crate.denominator);
      return;
    }
    if (crate.ctrl == "set_frame_size") {
      this.units[i].got_frame_size(crate.width, crate.height);
      return;
    }
  }
    }
  }

  if (crate.com == "kradradio") {
    if (crate.info == "cpu") {
      kr.got_system_cpu_usage(crate.system_cpu_usage);
      return;
    }
    if (crate.info == "sysname") {
      this.got_sysname(crate.infoval);
      return;
    }
  }
  if (crate.com == "kradlink") {
    if (crate.ctrl == "update_link") {
      kr.got_update_link(crate.link_num,
       crate.update_item, crate.update_value);
      return;
    }
    if (crate.ctrl == "add_link") {
      kr.got_add_link(crate);
      return;
    }
    if (crate.ctrl == "remove_link") {
      kr.got_remove_link(crate);
      return;
    }
    if (crate.ctrl == "add_decklink_device") {
      kr.got_add_decklink_device(crate);
      return;
    }
  }
}

Kr.prototype.display = function() {
  var cmd = {};
  cmd.com = "kradcompositor";
  cmd.cmd = "display";

  var JSONcmd = JSON.stringify(cmd);
  kr.ws.send(JSONcmd);
}

Kr.prototype.addsprite = function(filename) {
  var cmd = {};
  cmd.com = "kradcompositor";
  cmd.cmd = "add_subunit";
  cmd.subunit_type = "sprite";
  cmd.filename = filename;

  var JSONcmd = JSON.stringify(cmd);
  kr.ws.send(JSONcmd);
}

Rack.prototype.push_dtmf = function(value) {
  kr.ctrl("m/DTMF/t", value);
  kr.kode += value;
}

