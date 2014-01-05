$(document).ready(function() {
  rack_units.push({'constructor': Eq, 'aspect': [8,4]});
});

function Eq(info_object) {
  this.title = "Eq";
  this.description = "Eq sliders, geek.";
  this.aspect_ratio = [8,2];
  this.div = "<div id='eq' class='eq RCU'></div>";
  this.width = 0;
  this.height = 0;
  this.sel = "#eq";
  this.x = 24;
  this.y = 5;
  this.address_masks = ["kradmixer"];
  this.portgroup_eq_sliders = new Object();
  this.shown_effects = null;
  info_object['parent_div'].append(this.div);
}

Eq.prototype.destroy = function() {
}

Eq.prototype.update = function(crate) {
  if (crate.ctrl == "effect_control") {
    this.got_effect_control(crate);
  }
  if (crate.ctrl == "add_portgroup") {
    this.got_add_portgroup(crate);
  }
}

Eq.prototype.shared = function(key, shared_values) {
  if (key == 'eq_display') {
    this.toggle_show(shared_values[key]);
  }
}

Eq.prototype.got_add_portgroup = function(crate) {
  if (true || crate.direction == 1) {
    var pname = crate.content.name;

  $('#eq').append(
   "<div class='eq_area' id='eq_" + pname
   + "'><h1>" + pname + "</h1></div>");

  $('#' + 'eq_' + pname).append(" <div id='" + pname + "_effects_eq_db'"
   + " style='float:left'></div>");

    var eq_element, eq_slider;

  for (var i = 0; i < 32; i=i+4) {
    $('#' + pname + '_effects_eq_db').append(
     "<div class='kradmixer_control kradmixer_eq' id='"
     + pname + "_eq_" + i + "_db'><h4>" + crate.content.eq.band[i].hz + "</h4>"
     + "<div class='volume'>"
     + "<div class='handle'></div></div></div>");
    name = pname + "_eq_" + i.toString() + "_db";
    eq_element = document.id(name);
    eq_slider = eq_element.getElement('.volume');

  this.portgroup_eq_sliders[name] = new Slider(eq_slider,
   eq_slider.getElement('.handle'), {
     range: [100, -100],
     steps: 200,
     wheel: true,
     mode: "vertical",
     address: "e/" + pname + "/eq/" + i.toString() + "/db",
     initialStep: 100/12 * crate.content.eq.band[i].db,
     onChange: function (value) {
       kr.ctrl(this.options.address, 12/100*value);
     }
   });
  }
  }
}

Eq.prototype.toggle_show = function(portgroup_name) {
  /*$('#effects_region_description').html(portgroup_name +" Eq");*/
  if (this.shown_effects != portgroup_name) {
    if (this.shown_effects != '') {
      $('#eq_' + this.shown_effects).hide(0);
    } else {
        $('#eq').show(0);
    }

    $('#eq_' + portgroup_name).show(0);
    this.shown_effects = portgroup_name;
  } else {
    $('#eq_' + portgroup_name).hide(200);
    this.shown_effects = '';
  }
}

Eq.prototype.got_effect_control = function(crate) {
  if (crate.effect_name == "eq") {
    if ($('#' + crate.portgroup_name + '_' + crate.effect_name
     + '_' + crate.effect_num + '_' + crate.control_name)) {
      this.portgroup_eq_sliders[crate.portgroup_name + '_' + crate.effect_name
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
      this.portgroup_effect_sliders[crate.portgroup_name + '_'
       + crate.effect_name + '_'
       + crate.control_name].set(result);
    }
  }
}
