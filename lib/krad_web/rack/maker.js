$(document).ready(function() {
  rack_units.push({'constructor': Maker, 'aspect': [16,10], 'page': 2});
});

function Maker(info_object) {
  this.title = "Maker";
  this.description = "Make stuff.";
  this.aspect_ratio = [0,0];
  this.id = 'maker';
  this.div = "<div class='maker ultraviolet RCU' id='" + this.id + "'></div>";
  this.width = 0;
  this.height = 0;
  this.sel = "#" + this.id;
  this.x = 16;
  this.y = 1;
  this.address_masks = [];
  info_object['parent_div'].append(this.div);

  kr.maker = this;
  $(this.sel).append('\
<select id="make_what" onchange=kr.maker.handleSelect(this.value)>\
<option value="form_create_path_v4l2">V4l2</option>\
<option value="form_create_path_wayland">Wayland Window</option>\
<option value="form_create_path_jack">Jack</option>\
</select>');

  $(this.sel).append('\
<form class="creation_form" id="form_create_path_v4l2">\
<h1>Create v4l2 Path</h1>\
Device:\
<select name="device">\
<option value="0">HD Pro Webcam C920 (/dev/video0)</option>\
<option value="1">UVC Camera (046d:0821) (/dev/video1)</option>\
</select>\
<br>\
Resolution and Framerate:\
<select name="mode">\
  <optgroup label="640x360">\
    <option value="640x360_30">640x360 30/1 fps</option>\
    <option value="640x360_24">640x360 24/1 fps</option>\
    <option value="640x360_20">640x360 20/1 fps</option>\
  </optgroup>\
  <optgroup label="1280x720">\
    <option value="1280x720_30">1280x720 30/1 fps</option>\
    <option value="1280x720_24">1280x720 24/1 fps</option>\
    <option value="1280x720_20">1280x720 20/1 fps</option>\
  </optgroup>\
</select>\
<br>\
<br>\
<input type="submit" value="v4l2" id="button_create_path_v4l2">Create</input>\
</form>');

  $(this.sel).append('\
<form style="display:none" class="creation_form" id="form_create_path_wayland">\
<h1>Create Wayland Window</h1>\
Server: <select name="server">\
<option value="default" selected>default</option>\
<option value="another">another</option>\
</select><br>\
\
Fullscreen? <input type="checkbox" name="fullscreen" value="false" /><br>\
\
Window Dimensions:<br>\
<input type="radio" name="wxh_input_method" value="predefined">\
<select name="wxh">\
<option value="640x320" selected>640x320</option>\
<option value="1280x720">1280x720</option>\
</select>\
OR<br>\
<input type="radio" name="wxh_input_method" value="custom">\
Width: <input type="text" name="width">\
Height: <input type="text" name="height">\
<br>\
<input type="submit" value="Create">Create</input>\
</form>');

  $(this.sel).append('\
<form style="display:none" class="creation_form" id="form_create_path_jack">\
<h1>Create Jack Path</h1>\
Name:<input type="text" name="name" required><br>\
Number Channels: <select name="num_channels">\
<option value="1">1</option>\
<option value="2"selected>2</option>\
<option value="3">3</option>\
<option value="4">4</option>\
<option value="5">5</option>\
<option value="6">6</option>\
<option value="7">7</option>\
<option value="8">8</option>\
</select><br>\
Direction: <select name="direction">\
<option value="1" selected>INPUT</option>\
<option value="2">OUTPUT</option>\
</select>\
<br>\
<input type="submit" value="Create"/>\
</form>');

  
  $('#form_create_path_jack').submit(function( event ) {
    var json = '{\
      "ctrl": "add_adapter",\
      "payload": {\
        "name": ' + event.currentTarget.name.value + ',\
        "dir": ' + event.currentTarget.direction.value + ',\
        "api": 0,\
        "info": {\
          "name": ' + event.currentTarget.name.value + ',\
          "channels": ' + event.currentTarget.num_channels.value + ',\
          "direction": ' + event.currentTarget.direction.value + '\
        }\
      }\
    }';
    kr.send_json(json);
    event.preventDefault();
  });
}

Maker.prototype.handleSelect = function(value) {
  $('.creation_form').hide();
  $('#' + value).show();
}

Maker.prototype.shared = function(key, shared_object) {
}

Maker.prototype.destroy = function() {
}

Maker.prototype.update = function(crate) {
}
