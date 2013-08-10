

Kr.prototype.got_tag = function (tag_item, tag_name, tag_value) {
}

Kr.prototype.got_peak_portgroup = function(portgroup_name, value) {
  this.mixer_got_peak_portgroup(portgroup_name, value);
}

Kr.prototype.got_update_subunit = function(num, type,  control_name, value) {
  kr.subunit_list_update_subunit(num, type, control_name, value);
  kr.comper_update_subunit(num, type, control_name, value); 
}



Kr.prototype.got_add_subunit = function (crate) {
  this.comper_got_add_subunit(crate);
  this.subunit_list_got_add_subunit(crate);
}

Kr.prototype.got_remove_subunit = function(crate) {
  this.comper_got_remove_subunit(crate);
  this.subunit_list_got_remove_subunit(crate);
}


Kr.prototype.got_add_portgroup = function(crate) {
  this.mixer_got_add_portgroup(crate);
}

Kr.prototype.set_mixer_params = function(mixer_params) {
  this.sample_rate = mixer_params.sample_rate;
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
