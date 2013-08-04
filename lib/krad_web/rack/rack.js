
Kr.prototype.calc_rack = function() {

  this.ux.rack_density = 16;

  var view_width = window.innerWidth;
  var view_height = window.innerHeight;
  var rack_space_sz = view_width / this.ux.rack_density;

  this.ux.rack_width = Math.floor(view_width / rack_space_sz);
  this.ux.rack_width_per = (rack_space_sz / view_width) * 100;
  this.ux.rack_height = Math.floor(view_height / rack_space_sz);
  this.ux.rack_height_per = (rack_space_sz / view_height) * 100;

  kr.debug("rw: " + this.ux.rack_width + "rh: " + this.ux.rack_height);
  kr.debug("rwp: " + this.ux.rack_width_per + "rhp: " + this.ux.rack_height_per);

}


Kr.prototype.first_open_rack_position = function(RCU, positioned_RCUs) {
  var ret = null;
  var fits = 1;
  for (var j = 0; j < kr.ux.rack_height; j++) {
    for (var i = 0; i < kr.ux.rack_width; i++) {
      for (var r = 0; r < positioned_RCUs.length; r++) {
       if (
           ((i >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i)
            && (j >= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j))
           ||
             ((i+RCU.width >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i + RCU.width)
            && (j + RCU.height>= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j+RCU.height))
           ||
             ((i+RCU.width >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i + RCU.width)
            && (j >= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j))
           ||
             ((i >= positioned_RCUs[r].x && positioned_RCUs[r].x + positioned_RCUs[r].width > i)
            && (j + RCU.height>= positioned_RCUs[r].y && positioned_RCUs[r].y + positioned_RCUs[r].height > j+RCU.height))
            ||
             (i+RCU.width > kr.ux.rack_width || j + RCU.height > kr.ux.rack_height)
           ) {

          fits = 0;
          kr.debug(i + " " +j + " " +  positioned_RCUs[r].x + " " + positioned_RCUs[r].y + " " + positioned_RCUs[r].width + " " + positioned_RCUs[r].height);
          break;
        }
      }
      if (fits == 1) {
        kr.debug('IT FIT ' + i + " " + j);
        return [i, j];
      } else {
        fits = 1;
      }
    }
  }
  kr.debug('NO FIT');
  return null;
}

Kr.prototype.position_RCUs = function() {

  var positioned_RCUs = new Array();
  this.calc_rack();

  for (var i = 0; i < kr.ux.RCUs.length; i++) {

        var result =kr.first_open_rack_position(kr.ux.RCUs[i], positioned_RCUs);
        if (result != null) {
          positioned_RCUs.push(kr.ux.RCUs[i]);
            $(kr.ux.RCUs[i].sel).show();
          kr.ux.RCUs[i].x = result[0];
          kr.ux.RCUs[i].y = result[1];
        } else {
          $(kr.ux.RCUs[i].sel).hide();
        }

        var top = kr.ux.rack_height_per * kr.ux.RCUs[i].y;
        var left = kr.ux.rack_width_per * kr.ux.RCUs[i].x;
        var height = kr.ux.rack_height_per * kr.ux.RCUs[i].height;
        var width = kr.ux.rack_width_per * kr.ux.RCUs[i].width;

        $(kr.ux.RCUs[i].sel).css({
            'top': top + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'left': left + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'width': width + '%'
        });
        $(kr.ux.RCUs[i].sel).css({
            'height': height + '%'
        });
    }
}