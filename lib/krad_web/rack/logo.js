$(document).ready(function() {
  rack_units.push({'constructor': Logo, 'aspect': [4,1]});
});

function Logo(info_object) {
  this.title = "Logo";
  this.description = "Display your station's logo.";
  this.aspect_ratio = [0,0];
  this.image = "http://space.kradradio.com/krad_color_logo_trans2.png";
  this.div = "<div class='logo RCU'><img style='width: 90%; height: auto; margin: 4%;' src='" + this.image + "'></img></div>";
  this.width = 0;
  this.height = 0;
  this.sel = ".logo";
  this.x = 24;
  this.y = 1;
  this.address_masks = [];

  info_object['parent_div'].append(this.div);
}

Logo.prototype.shared = function(key, shared_object) {
}

Logo.prototype.destroy = function() {
}

Logo.prototype.update = function(crate) {
}
