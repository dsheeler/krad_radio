Kr.prototype.subunit_list_got_add_subunit = function(crate) {    
  if ((crate.subunit_type != 'videoport') || (crate.direction != 'output')) {
    var type_num = crate.subunit_type + "_" + crate.subunit_id;

    $('#subunit_list').append("<li class=subunit id='" + type_num + "'>" + crate.subunit_type
     + " " + crate.subunit_id + " <div class='subunit_buttons_wrap' style='float: left, width: 33%'>"
     + "    <div class='button_wrap'>"
     + "<div class='krad_button_very_small' id='fade_"
     + type_num + "'>FADE</div></div><div class='button_wrap'>"
     + "<div class='krad_button_very_small' id='move_to_origin_"
     + type_num + "'>(0,0)</div></div>"
     + "<div class='button_wrap'><div class='krad_button_very_small' id='reset_w_h_"
     + type_num + "'>ORIG W/H</div></div>"
     + "<div class='button_wrap'><div class='krad_button_very_small happy_button'"
     + "id='happy_button_"
     + type_num + "'>HAPPY</div></div>"
     + "</div></li>");

    $('#' + type_num).bind("click", function(e,ui) {
      e.preventDefault();
      var alreadySelected = $(this).hasClass('selected');
      var shapes = kr.compositor_canvas.shapes;
      var l = shapes.length;

      /*Run removeClass on every element.*/
	    $('.subunit').removeClass('selected');
	    for (var i = 0; i < l;  i++) {
	      shapes[i].selected = false;
	      if (shapes[i].num == crate.subunit_id && shapes[i].type == crate.subunit_type) {
		      shapes[i].selected = !alreadySelected;
          if (!alreadySelected) {
            kr.compositor_canvas.selection = shapes[i];
          }
        }
      }
      kr.compositor_canvas.valid = false;
      if (!alreadySelected) {
        $(this).addClass('selected');
      }
    });

    if (crate.subunit_type == "sprite") {
       var paths = crate.filename.split("/");
       var path_tail = paths[paths.length-1];
       $('#' + type_num).append(" <div style='display: inline' id='" + type_num
        + "_filename'>'" + path_tail + "'</div>");

        $('#' + type_num).append(" rate: <div style='display: inline' id='" + type_num
        + "_rate'>" + crate.rate + "</div>");
     }
     if (crate.subunit_type == "text") {
       $('#' + type_num).append(" text: <div style='display: inline' id='" + type_num
        + "_text'>''" + crate.text + "''</div>");
     }

     $("#" + type_num).append(

      " x: <div style='display: inline' id='" + type_num + "_x'>" + crate.x + "</div>"
      + " y: <div style='display: inline' id='" + type_num + "_y'>" + crate.y + "</div>"
      + " width: <div style='display: inline' id='" + type_num + "_w'>" + crate.width + "</div>"
      + " height: <div style='display: inline' id='" + type_num + "_h'>" + crate.height + "</div>"
      + " rotation: <div style='display: inline' id='" + type_num + "_r'>" + crate.r.toFixed(3) + "</div>"
      + " opacity: <div style='display: inline' id='" + type_num + "_o'>" + crate.o.toFixed(2) + "</div>"
     );

    $('li.subunit:even').removeClass("alt");
    $('li.subunit:odd').addClass("alt");


     $('#reset_w_h_' + type_num).on("click", {w: crate.w, h: crate.h}, function(e) {
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "xscale", crate.xscale, 60);
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "yscale", crate.yscale, 60);
      return false;
    });

    $('#happy_button_' + type_num).on("click", function() {
      var x = 960*Math.random();
      var y = 540*Math.random();
      var o = Math.random();
      var r = 360*Math.random();
      var xs = 3*Math.random();
      var ys = 3*Math.random();
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "x", x, 120*Math.random());
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "y", y, 120*Math.random());
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "o", o, 120*Math.random());
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "r", r, 120*Math.random());
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "xscale", xs, 120*Math.random());
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "yscale", ys, 120*Math.random());
      return false;
    });

    $('#move_to_origin_' + type_num).on("click", function() {
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "x", 0, 60);
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "y", 0, 60);
      return false;
    });

    $('#fade_' + type_num).bind("click", function() {
      var new_value = 0;
      if (kr.ux.opacity_toggle[type_num] == 0) {
        new_value = 1;
        kr.ux.opacity_toggle[type_num] = 1;
      } else {
        new_value = 0;
        kr.ux.opacity_toggle[type_num] = 0;
      }
      kr.ux.comp_ctrl(crate.subunit_id, crate.subunit_type, "o", new_value, 60);
      return false;
    });
  }
}

Kr.prototype.subunit_list_got_remove_subunit = function(crate) {
  $('#' + crate.subunit_type + '_' + crate.subunit_id).remove();
  $('li.subunit:even').removeClass("alt");
  $('li.subunit:odd').addClass("alt");
}


Kr.prototype.subunit_list_update_subunit = function(num, type, control_name, value) {
  var type_num = type + "_" + num;
  this.debug(type_num);
  switch (control_name) {
    case "x":
      $('#' + type_num + '_x').html(value.toFixed(0));
      break;
    case "y":
      $('#' + type_num + '_y').html(value.toFixed(0));
      break;
    case "width":
      $('#' + type_num + '_w').html(value);
      break;
    case "height":
      $('#' + type_num + '_h').html(value);
      break;
    case "rotation":
      $('#' + type_num + '_r').html(value.toFixed(3));
      break;
    case "opacity":
      $('#' + type_num + '_o').html(value.toFixed(3));
      break;
  }
}