$(document).ready(function() {
  rack_units.push({'constructor': Xmms2, 'aspect': [8,3]});
});

xmms2_ctrl = function(portgroup_name, xmms2_command) {
  var cmd = '{"xmms2":"' + portgroup_name + ' ' + xmms2_command + ' "}';
  kr.ws.send(cmd);
}

function Xmms2(info_object) {
  this.title = "Xmms2";
  this.description = "NERDS!!!";
  this.aspect_ratio = [8,3];
  this.x = 24;
  this.y = 2;
  this.width = 0;
  this.height = 0;
  this.address_masks = ["kradmixer"];
  this.div_text = "<div id='xmms2s' class='rrrred RCU xmms2s'>";
  this.sel = '#xmms2s';
  info_object['parent_div'].append(this.div_text);
}

Xmms2.prototype.shared = function(key, shared_object) {
}

Xmms2.prototype.update = function(crate) {
  if (crate.ctrl == "update_portgroup") {
    this.got_update_portgroup(crate.portgroup_name, crate.control_name, 
     crate.value);   
  }
  if (crate.ctrl == "add_portgroup") {
    this.got_add_portgroup(crate);
  }
  if (crate.ctrl == "update_portgroup") {
    this.got_update_portgroup(crate.portgroup_name, crate.control_name, 
     crate.value);
  }
}

Xmms2.prototype.got_add_portgroup = function(crate) {
  if (crate.xmms2 == 1) {
    this.portgroup_handle_xmms2(crate.portgroup_name, crate.xmms2);
  }
}

Xmms2.prototype.got_update_portgroup = function(portgroup_name, 
 control_name, value) {
  if (control_name == "xmms2_ipc_path") {
    if (value.length > 0) {
      this.portgroup_handle_xmms2(portgroup_name, 1);
    } else {
      this.portgroup_handle_xmms2(portgroup_name, 0);
    }
  }
}

Xmms2.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {
  if (xmms2 == 0) {
    if ($('#' + portgroup_name + '_xmms2_wrap')) {
      $('#' + portgroup_name + '_xmms2_wrap').remove();
    }
  } else {
	  xmms2_controls = "<div id='" + portgroup_name
      + "_xmms2_wrap' class='xmms2_control'><h2>" + portgroup_name + "</h2><div id='"
     + portgroup_name + "_xmms2'></div></div>";

    $(this.sel).append(xmms2_controls);

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
     function(event) {
      xmms2_ctrl(portgroup_name, "prev");
    });

    $('#' + portgroup_name + '_xmms2_play' ).bind("click",
     function(event) {
      xmms2_ctrl(portgroup_name, "play");
    });

	  $('#' + portgroup_name + '_xmms2_pause' ).bind("click",
     function(event) {
	    xmms2_ctrl(portgroup_name, "pause");
	  });

    $('#' + portgroup_name + '_xmms2_stop' ).bind("click",
     function(event) {
	    xmms2_ctrl(portgroup_name, "stop");
	  });

	  $('#' + portgroup_name + '_xmms2_next' ).bind("click",
     function(event) {
	    xmms2_ctrl(portgroup_name, "next");
	  });
  }
}
