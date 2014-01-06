$(document).ready(function() {
  rack_units.push({'constructor': Comper, 'aspect': [16,9], 'page': 0});
});

comp_ctrl = function(num, type, control_name, value, duration) {
  var t;
  var d = duration || 0;
  if (type == "videoport") {
    t = "v";
  } else {
    t = type;
  }
  value = Math.round(value * 100)/100;
  kr.ctrl(t + "/" + num + "/" + control_name, value, d);
}

despace_name = function(name) {
  return name.replace(/ /g, "_");
}

function Comper(info_object) {
  kr.comper = this;
  this.title = "Comper";
  this.description = "Composite, honky";
  this.aspect_ratio = [16,9];
  this.x = 0;
  this.y = 0;
  this.width = 0;
  this.height = 0;
  this.address_masks = ["kradcompositor"];
  this.div_text = "<div id='kradcompositor' class='RCU kradcompositor'></div>";
  this.sel = "#kradcompositor";
  info_object['parent_div'].append(this.div_text);
  this.resizeTimer;
  this.output_path_width = 0;
  this.output_path_height = 0;
  this.width_upscaler;
  this.height_upscaler;
  this.num_videoports = 0;

  $(this.sel).append("<div "
  + "class='yelleryellow scene'"
   + "id='scene'>"
   + "</div>");


  }

Comper.prototype.page_resize = function() {
  var w = $(kr.comper.sel).width();
  var h = $(kr.comper.sel).height();
  $('#scene').css('width', w);
  $('#scene').css('height', h);
}

Comper.prototype.window_resize = function() {
  clearTimeout(this.resizeTimer);
  this.resizeTimer = setTimeout(this.page_resize, 100);
}

Comper.prototype.update = function(crate) {
  if (crate.ctrl == "add_videoport") {
    this.got_add_subunit(crate);
  }
  if (crate.ctrl == "remove_subunit") {
    this.got_remove_subunit(crate);
  }
  if (crate.ctrl == "update_subunit") {
    this.got_update_subunit(crate.subunit_id, crate.subunit_type,
     crate.control_name, crate.value);
  }
}


Comper.prototype.got_add_subunit = function(crate) {
  this.num_videoports++;
  var pname = crate.content.name;
  var w = crate.content.width;
  var h = crate.content.height;
  if (pname == "Wayland Test") {
    this.output_path_width = w;
    this.output_path_height = h;
  } else {
    pname = despace_name(pname);
    $('#scene').append('<div style="position: absolute"\
     class="rrrred comper_item"\
     id="' + pname +  '_comper_item">' + pname + '</div>');
    var percent_width = 100 * (w / this.output_path_width);
    var percent_height = 100 * (h / this.output_path_height);
    var percent_x = 100 * (crate.content.controls.x / this.output_path_width);
    var percent_y = 100 * (crate.content.controls.y / this.output_path_height);

    $('#' + pname + '_comper_item').css({'width': percent_width + '%'});
    $('#' + pname + '_comper_item').css({'height': percent_height + '%'});
    $('#' + pname + '_comper_item').css({'left': percent_x + '%'});
    $('#' + pname + '_comper_item').css({'top': percent_y + '%'});

    $('#' + pname + '_comper_item').draggable();
    $('#' + pname + '_comper_item').on("drag", {num: this.num_videoports-1}, 
     function(event, ui) {
       var x_percent = ui.position.left / $('#scene').width();
       var x = x_percent * kr.comper.output_path_width;
       var y_percent = ui.position.top / $('#scene').height();
       var y = y_percent * kr.comper.output_path_height;
       comp_ctrl(event.data.num, 'videoport', 'x', x, 0);
       comp_ctrl(event.data.num, 'videoport', 'y', y, 0);
    });
  }
}

Comper.prototype.got_remove_subunit = function(crate) {
}

Comper.prototype.got_update_subunit = function(num, type, control_name, value) {
}

