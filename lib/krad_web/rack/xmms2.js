Kr.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {
  if (xmms2 == 0) {
    if ($('#' + portgroup_name + '_xmms2_wrap')) {
      $('#' + portgroup_name + '_xmms2_wrap').remove();
    }
  } else {
	  xmms2_controls = "<div id='" + portgroup_name
      + "_xmms2_wrap' class='xmms2_control'><h2>" + portgroup_name + "</h2><div id='"
     + portgroup_name + "_xmms2'></div></div>";

    $('#xmms2s').append(xmms2_controls);

    /*if (this.xmms2_portgroup_count == 1) {
      $('.kradmixer_beta').prepend(xmms2_controls);
    } else {
      $('.kradmixer_beta').append(xmms2_controls);
    }*/

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
     function(event, ui) {
      kr.xmms2(portgroup_name, "prev");
    });

    $('#' + portgroup_name + '_xmms2_play' ).bind("click",
     function(event, ui) {
      kr.xmms2(portgroup_name, "play");
    });

	  $('#' + portgroup_name + '_xmms2_pause' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "pause");
	  });

    $('#' + portgroup_name + '_xmms2_stop' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "stop");
	  });

	  $('#' + portgroup_name + '_xmms2_next' ).bind("click",
     function(event, ui) {
	    kr.xmms2(portgroup_name, "next");
	  });
  }
}
