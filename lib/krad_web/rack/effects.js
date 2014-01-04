$(document).ready(function() {
  rack_units.push({'constructor': Effects, 'aspect': [8,4]});
});

function Effects(info_object) {
  this.title = "Effects";
  this.description = "Effects sliders, geek.";
  this.aspect_ratio = [8,2];
  this.div = "<div id='effects' class='coolblue effects RCU'></div>";
  this.width = 0;
  this.height = 0;
  this.sel = "#effects";
  this.x = 24;
  this.y = 5;
  this.address_masks = ["kradmixer"];
  this.portgroup_effect_sliders = new Object();
  this.shown_effects = null;
  info_object['parent_div'].append(this.div);
}

Effects.prototype.destroy = function() {
}

Effects.prototype.update = function(crate) {
  if (crate.ctrl == "effect_control") {
    this.got_effect_control(crate);
  }
  if (crate.ctrl == "add_portgroup") {
    this.got_add_portgroup(crate);
  }
  if (crate.ctrl == "effect_control") {
    this.got_effect_control(crate);
  }
}

Effects.prototype.shared = function(key, shared_values) {
  if (key == 'effects_display') {
    this.toggle_show(shared_values[key]);
  }
}

Effects.prototype.got_add_portgroup = function(crate) {
  if (true || crate.direction == 1) {
    var pname = crate.content.name;

    $('#effects').append("<div class='effect_area' "
     + "id='" + pname
     + "_effects' style='float:left'></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_lowpass_hz'><h4 id='lp_hz_label'>LP FREQ</h4>"
   + "<div class='volume'>"
   + "<div class='handle'></div></div></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_lowpass_bw'><h4 id='lp_bw_label'>BANDWIDTH</h4><div class='volume'>"
   + "<div class='handle'></div></div></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_highpass_hz'><h4 id='hp_hz_label'>HP FREQ</h4><div class='volume'>"
   + "<div class='handle'></div></div></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_highpass_bw'><h4 id='hp_hz_label'>BANDWIDTH</h4><div class='volume'>"
   + "<div class='handle'></div></div></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_analog_drive'><h4 id='hp_hz_label'>DRIVE</h4><div class='volume'>"
   + "<div class='handle'></div></div></div>");

  $('#' + pname + '_effects').append
   ("<div class='kradmixer_control' id='"
   + pname + "_analog_blend'><h4 id='hp_hz_label'>BLEND</h4><div class='volume'>"
   + "<div class='handle'></div></div></div>");

  var name = pname + "_lowpass_hz";
  var effect_element = document.id(name);
  var effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider,
   effect_slider.getElement('.handle'), {
   range: [100, 0],
   steps: 200,
   wheel: true,
   mode: "vertical",
   initialStep: 100*Math.log(crate.content.lowpass.hz/20)/Math.log(600),
   onChange: function (value) {
     kr.ctrl("e/" + pname + "/lp/hz", 20*Math.pow(2,value*Math.log(600)/Math.log(2)/100));
   }
  });

  name = pname + "_lowpass_bw";
  effect_element = document.id(name);
  effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider,
   effect_slider.getElement('.handle'), {
     range: [100, 0],
     steps: 200,
     wheel: true,
     mode: "vertical",
     initialStep: 20*crate.content.lowpass.bw,
     onChange: function (value) {
       kr.ctrl("e/" + pname + "/lp/bw", value/20);
     }
   });

  name = pname + "_highpass_hz";
  effect_element = document.id(name);
  effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
    range: [100, 0],
    steps: 200,
    wheel: true,
    mode: "vertical",
    initialStep: 100*Math.log(crate.content.highpass.hz/20)/Math.log(1000),
    onChange: function (value) {
      kr.ctrl("e/" + pname + "/hp/hz", 20*Math.pow(2,value*Math.log(1000)/Math.log(2)/100));
    }
  });

  name = pname + "_highpass_bw";
  effect_element = document.id(name);
  effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
    range: [100, 0],
    steps: 101,
    wheel: true,
    mode: "vertical",
    initialStep: 20*crate.content.highpass.bw,
    onChange: function (value) {
      kr.ctrl("e/" + pname + "/hp/bw", value/20);
    }
  });

  name = pname + "_analog_drive";
  effect_element = document.id(name);
  effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
    range: [100, 0],
    steps: 200,
    wheel: true,
    mode: "vertical",
    initialStep: 10 * crate.content.analog.drive,
    onChange: function (value) {
      kr.ctrl("e/" + pname + "/a/drive", 0.1*value);
    }
  });

  name = pname + "_analog_blend";
  effect_element = document.id(name);
  effect_slider = effect_element.getElement('.volume');

  this.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
    range: [100, -100],
    steps: 200,
    wheel: true,
    mode: "vertical",
    initialStep: 10 * crate.content.analog.blend,
    onChange: function (value) {
      kr.ctrl("e/" + pname + "/a/blend", 0.1*value);
    }
  });
  }
}

Effects.prototype.toggle_show = function(portgroup_name) {
  if (this.shown_effects != portgroup_name) {
    if (this.shown_effects != '') {
      $('#' + this.shown_effects + "_effects") .hide(200);
    } else {
        $('#effects').show(200);
    }

    $('#' +portgroup_name + "_effects") .show(200);
    this.shown_effects = portgroup_name;
  } else {
    $('#' +portgroup_name + "_effects") .hide(200);
    this.shown_effects = '';
  }
}

Effects.prototype.got_effect_control = function(crate) {
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
