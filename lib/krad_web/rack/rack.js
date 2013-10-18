var rack_units = [];

Kr.prototype.load_interface = function() {
	this.ux = new Rack();
}

function Rack() {
  this.units = [];
  this.shared = {};
  this.positioned_units = [];
  this.current_page = 0;
  this.calc_rack();
}

Rack.prototype.share = function(key, value) {
  this.shared[key] = value;
  for (var i = 0; i < this.units.length; i++) {
    this.units[i].shared(key, this.shared);
  }
}

Rack.prototype.display_page = function(num, duration) {
  // use this if you specifically want to know if b was passed
  if ('undefined' === typeof duration) {
    duration = 0;
  }
  this.current_page = num;
  this.position_units(0, num, duration);
}

Rack.prototype.rack_comp_ctrl = function(unit_title, id, type, control_name,
 value, dur) {
  comp_ctrl(id, type, control_name, value, dur);
  if (typeof(dur) === 'undefined' || dur == 0) {
    var crate = new Object();
    crate.com = "kradcompositor";
    crate.ctrl = "update_subunit"
    crate.subunit_id = id;
    crate.subunit_type = type;
    crate.control_name = control_name;
    crate.value = value;

    for (var i = 0; i < this.units.length; i++) {
      for (var j = 0; j < this.units[i].address_masks.length; j++) {
        if (this.units[i].title != unit_title
         && this.units[i].address_masks[j] == "kradcompositor") {
          this.units[i].update(crate);
        }
      }
    }
  }
}

Rack.prototype.setup_rack = function(info_object) {
  /* loop through global rack_units array and call constructors */
  for (var i = 0; i < rack_units.length; i++) {
    unit = new rack_units[i]['constructor'](info_object);
    unit.width = rack_units[i]['aspect'][0];
    unit.height = rack_units[i]['aspect'][1];
    unit.page = rack_units[i]['page'] || 0;
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

  $('#' + this.sysname).append("<div style='display: none' "
   + "class='grid'></div>");

 window.onkeypress = function(event) {
    var unicode = event.charCode ? event.charCode : event.keyCode;
    var actualkey = String.fromCharCode(unicode);
    if (actualkey == "g") {
      $(".grid").toggle();
    }
  }

  this.setup_rack({'parent_div': $('#' + this.sysname)});
  this.position_units(0);

  window.onresize = function(event) {
    kr.ux.position_units(0, kr.ux.current_page);
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

Rack.prototype.position_units = function(auto, page=0, duration=0) {


  for (var i = 0; i < this.positioned_units.length; i++) {
    $(this.positioned_units[i].sel).hide(duration);
  }

  this.positioned_units = new Array();

  for (var j = 0; j < this.rack_height; j++) {
    for (var i = 0; i < this.rack_width; i++) {
      var id = i + "_" + j;
      $('#' + id).remove();
    }
  }

  this.calc_rack();
  for (var j = 0; j < this.rack_height; j++) {
    for (var i = 0; i < this.rack_width; i++) {
      var top = this.rack_height_per * j;
      var left = this.rack_width_per * i;
      var height = this.rack_height_per;
      var width = this.rack_width_per;
      var id = i + "_" + j;

      $('.grid').append("<div id='" + id + "' style='z-index: 100;" +
       "position: absolute; border: 1px solid black'></div>");

      $('#' + id).css({'top': top + '%'});
      $('#' + id).css({'left': left + '%'});
      $('#' + id).css({'width': width + '%'});
      $('#' + id).css({'height': height + '%'});
    }
  }

  for (var i = 0; i < this.units.length; i++) {
    if (this.units[i].page == 'all' || this.units[i].page == page) {
      if (auto == 1) {
        var result = this.first_open_rack_position(this.units[i], this.positioned_units);
        if (result != null) {
          this.positioned_units.push(this.units[i]);
          $(this.units[i].sel).show();
          this.units[i].x = result[0];
          this.units[i].y = result[1];
        } else {
          $(this.units[i].sel).hide();
        }
      }
      this.positioned_units.push(this.units[i]);
      $(this.units[i].sel).show(duration);
      var top = this.rack_height_per * this.units[i].y;
      var left = this.rack_width_per * this.units[i].x;
      var height = this.rack_height_per * this.units[i].height;
      var width = this.rack_width_per * this.units[i].width;

      $(this.units[i].sel).css({'top': top + '%' });
      $(this.units[i].sel).css({ 'left': left + '%' });
      $(this.units[i].sel).css({ 'width': width + '%' });
      $(this.units[i].sel).css({ 'height': height + '%' });
    }
  }
}

Rack.prototype.on_crate = function(crate) {
  if (crate.com == "kradmixer") {
    for (var i=0; i < this.units.length; i++) {
      for (var j=0; j <this.units[i].address_masks.length; j++) {
        if (this.units[i].address_masks[j] == "kradmixer") {
          if (crate.ctrl == "peak_portgroup") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "control_portgroup") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "effect_control") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "update_portgroup") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "add_portgroup") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "remove_portgroup") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "set_mixer_params") {
            this.units[i].update(crate);
          }
        }
      }
    }
  }
  if (crate.com == "kradcompositor") {
    for (var i=0; i < this.units.length; i++) {
      for (var j=0; j < this.units[i].address_masks.length; j++) {
        if (this.units[i].address_masks[j] == "kradcompositor") {
          if (crate.ctrl == "add_subunit") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "remove_subunit") {
            this.units[i].update(crate);
          }
          if (crate.ctrl == "update_subunit") {
            this.units[i].update(crate);
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

