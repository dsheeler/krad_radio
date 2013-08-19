$(document).ready(function() {
  rack_units.push({'constructor': Comper, 'aspect': [16,9]});
});

comp_ctrl = function(num, type, control_name, value, duration) {
  var t;
  var d = duration || 0;
  if (type == "videoport") {
    t = "v";
  } else {
    t = type;
  }
  kr.ctrl(t + "/" + num + "/" + control_name, value, d);
}

function Comper(info_object) {
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
  
  $(this.sel).append("<canvas "
  + "class='yelleryellow kradcompositor_canvas'"
   + "id='compositor_canvas'>"
   + "</canvas>");

  this.compositor_canvas = new CanvasState(document.getElementById
  ("compositor_canvas"), document.createElement('canvas'));
}

Comper.prototype.update = function(crate) {
  if (crate.ctrl == "add_subunit") {
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

Comper.prototype.shared = function(key, shared_object) {
}

Comper.prototype.got_add_subunit = function(crate) {
  if ((crate.subunit_type != 'videoport') || (crate.direction != 'output')) {
    var shape = new Shape(crate.x,crate.y, crate.width,
     crate.height, crate.r * Math.PI / 180.0, crate.o,
     crate.xscale, crate.yscale,
     crate.subunit_type, crate.subunit_id);
    this.compositor_canvas.addShape(shape);
  }
}

Comper.prototype.got_remove_subunit = function(crate) {
  this.compositor_canvas.removeShape(crate.subunit_type, crate.subunit_id);
  this.compositor_canvas.valid = false;
}

Comper.prototype.got_update_subunit = function(num, type, control_name, value) {
  for (var i = 0; i < this.compositor_canvas.shapes.length; i++) {
    if (this.compositor_canvas.shapes[i].type == type
     && (this.compositor_canvas.shapes[i].num == num)) {
      this.compositor_canvas.shapes[i].setValue(control_name, value);
      this.compositor_canvas.valid = false;
      break;
    }
  }
}

function PerspectiveCorrectionView(x0, y0, x1, y1, x2, y2, x3, y3) {
  this.handles = [];
  this.handles[0] = new PerspectiveCorrectionHandle(0, x0, y0, this);
  this.handles[1] = new PerspectiveCorrectionHandle(1, x1, y1, this);
  this.handles[2] = new PerspectiveCorrectionHandle(2, x2, y2, this);
  this.handles[3] = new PerspectiveCorrectionHandle(3, x3, y3, this);
  this.on == false;
}

PerspectiveCorrectionView.prototype.draw = function(ctx, onGhost) {
  if (this.on == true) {
    ctx.save();
    ctx.beginPath();
    ctx.moveTo(this.handles[0].x, this.handles[0].y);
    ctx.lineTo(this.handles[1].x, this.handles[1].y);
    ctx.lineTo(this.handles[3].x, this.handles[3].y);
    ctx.lineTo(this.handles[2].x, this.handles[2].y);
    ctx.closePath();
    ctx.fillStyle = 'rgba(17,119,102,0.5)';
	  ctx.fill();
    ctx.restore();
  }

	/* draw the perspective correction handles */
  l = this.handles.length;
  for (var i = 0; i < l; i++) {
    this.handles[i].draw(ctx, onGhost);
  }
}

function PerspectiveCorrectionHandle(id, x, y, view) {
  this.x = x || 0;
  this.y = y || 0;
  this.id = id || 0;
  this.view = view || null;
  this.selectionHandleColor = '#117766';
  this.selectionHandleSize = 20;
}

PerspectiveCorrectionHandle.prototype.draw = function(ctx, onGhost) {
  ctx.save();
  var half = this.selectionHandleSize / 2;

	if (onGhost == true) {
    ctx.fillStyle = '#fe0' + this.id.toString(16) + 'ff';
    ctx.strokeStyle = '#fe0' + this.id.toString(16) + 'ff';
	} else {
	  ctx.fillStyle = this.selectionHandleColor;
    ctx.strokeStyle = this.selectionHandleColor;
	}

  ctx.lineWidth = 12;
  ctx.lineJoin = 'round';
  ctx.lineCap = 'round';
  ctx.beginPath();

  var id1, id2;
  if (this.id == 0) {
    id1 = 1;
    id2 = 2;
  } else if (this.id == 1) {
    id1 = 3;
    id2 = 0;
  } else if (this.id == 2) {
    id1 = 0;
    id2 = 3;
  } else if (this.id == 3) {
    id1 = 2;
    id2 = 1;
  }

  var aX, aY, A;
  aX = this.view.handles[id1].x - this.x;
  aY = this.view.handles[id1].y - this.y;
  A = Math.sqrt(aX*aX + aY*aY);
  aX = aX/A;
  aY = aY/A;

  ctx.moveTo(this.x + 0.333 * A * aX, this.y + 0.333 * A * aY);
  ctx.lineTo(this.x, this.y);
  aX = this.view.handles[id2].x - this.x;
  aY = this.view.handles[id2].y - this.y;
  A = Math.sqrt(aX*aX + aY*aY);
  aX = aX/A;
  aY = aY/A;
  ctx.lineTo(this.x + 0.333 * A * aX, this.y + 0.333 * A * aY);
  ctx.stroke();
  ctx.restore();
}

function Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {
  /*This is a very simple and unsafe constructor.
    All we're doing is checking if the values exist.
    "x || 0" just means "if there is a value for x, use that.
    Otherwise use 0."
    But we aren't checking anything else!
    We could put "Lalala" for the value of x. */
  this.x = x || 0;
  this.y = y || 0;
  this.w = w || 1;
  this.h = h || 1;
  this.r = r || 0;
  this.o = o || 1;
  this.xs = xs || 1;
  this.ys = ys || 1;
  this.fill = fill || '#AAAAAA';
  this.selected = false;
  this.type = type || "";
  this.num = num || 0;
  this.selectionBoxColor = 'darkred';
  this.selectionBoxSize = 20;
  this.selectionColor = '#CC0000';
  this.selectionWidth = 2;
  this.aspectRatio = this.w/this.h;
  this.selectionHandles = [];

  for (var i = 0; i < 10; i ++) {
    var pt = {x: 0, y: 0};
    this.selectionHandles.push(pt);
  }

  this.PCView = type == 'videoport' ? new PerspectiveCorrectionView(0,0,
   this.w,0,0,this.h,this.w, this.h) : null;

  if (type == 'text') {
    this.h = 40;
    this.xs = 1;
    this.ys = 1;
  }
}

Shape.prototype.setValue = function(name, value) {
  if (name == "x") {
	  this.x = value;
  } else if (name == "y") {
	  this.y = value;
  } else if (name == "height") {
	  this.h = value;
  } else if (name == "width") {
	  this.w = value;
  } else if (name == "xscale") {
	  this.xs = value;
  } else if (name == "yscale") {
	  this.ys = value;
  } else if (name == "rotation") {
	  this.r = Math.PI / 180.0 * value;
  } else if (name == "opacity") {
	  this.o = value;
  }
}

/* Draws this shape to a given context. */
Shape.prototype.draw = function(ctx, onGhost) {

  ctx.save();

  ctx.translate(this.x, this.y);
  ctx.scale(this.xs, this.ys);
  ctx.translate(-this.x, -this.y);

  ctx.translate(this.x, this.y);
  ctx.translate(this.w*0.5, this.h*0.5);
  ctx.rotate(this.r);
  ctx.translate(-this.w*0.5, -this.h*0.5);
  ctx.translate(-this.x, -this.y);

  if (onGhost) {
	  ctx.fillStyle = '#000000';
  } else {
    if (this.selected == true) {
		ctx.shadowColor = '#3060a0';
  		ctx.shadowBlur = 10;
  		ctx.shadowOffsetX = 0;
  		ctx.shadowOffsetY = 0;
	
    } 
    ctx.fillStyle = this.fill;
  }
  ctx.fillRect(this.x, this.y, this.w, this.h);

  ctx.strokeStyle = '#5588cc';
  ctx.lineWidth = this.selectionWidth;
  ctx.strokeRect(this.x,this.y,this.w,this.h);

  /*Draw selection which is a stroke along the box
     and also selection handles. */
  this.selectionBoxSize = 0.05 * (this.h > this.w ? this.w : this.h);
  if (this.selected == true) {
    var half = this.selectionBoxSize / 2;
	  ctx.strokeStyle = this.selectionColor;
	  ctx.lineWidth = this.selectionWidth;
	  ctx.strokeRect(this.x,this.y,this.w,this.h);
	  ctx.beginPath();
	  ctx.moveTo(this.x+this.w/2, this.y);
	  ctx.lineTo(this.x+0.5*this.w, this.y-3*this.selectionBoxSize);
	  ctx.stroke();
	  /*Draw the handles.
	    0  1  2
	    3     4
	    5  6  7*/

	  /*Top left, middle, and right handles.*/
	  this.selectionHandles[0].x = this.x;
	  this.selectionHandles[0].y = this.y;

	  this.selectionHandles[1].x = this.x+this.w/2;
	  this.selectionHandles[1].y = this.y;

	  this.selectionHandles[2].x = this.x+this.w;
	  this.selectionHandles[2].y = this.y;

	  /*Middle left handle.*/
	  this.selectionHandles[3].x = this.x;
	  this.selectionHandles[3].y = this.y+this.h/2;

	  /*Middle right handle.*/
	  this.selectionHandles[4].x = this.x+this.w;
	  this.selectionHandles[4].y = this.y+this.h/2;

    /*Bottom left, middle, and right handles.*/
    this.selectionHandles[6].x = this.x+this.w/2;
    this.selectionHandles[6].y = this.y+this.h;

    this.selectionHandles[5].x = this.x;
    this.selectionHandles[5].y = this.y+this.h;

    this.selectionHandles[7].x = this.x+this.w;
    this.selectionHandles[7].y = this.y+this.h;

    /*Rotation handle.*/
    this.selectionHandles[8].x = this.x+0.5*this.w;
    this.selectionHandles[8].y = this.y- 3*this.selectionBoxSize;

    /*Opacity handle.*/
    this.selectionHandles[9].x = this.x+0.5*this.w;
    this.selectionHandles[9].y = this.y+0.5*this.h;

    for (var i = 0; i < 10; i ++) {
      var cur = this.selectionHandles[i];
      ctx.beginPath();
      ctx.arc(cur.x,cur.y,this.selectionBoxSize,0,2*Math.PI);
      if (onGhost == true) {
        ctx.fillStyle = '#ff0' + i.toString(16) + 'ff';
        ctx.fill();
      } else {
		    ctx.fillStyle = this.selectionBoxColor;
		    ctx.fill();
	    }
	  }
  }

  var xpad = 5;
  ctx.fillStyle = '#000000';

  var lineHeight = 2*this.selectionBoxSize;

  ctx.font = lineHeight.toString() + "px Ubuntu";
  lineHeight = parseInt(ctx.font.match(/^(\d+)/)[0]);

  var tmpy = this.y + lineHeight + this.selectionBoxSize;

  ctx.fillText(this.type + " " + this.num, this.x, tmpy);
  tmpy += lineHeight;

  /*lineHeight = lineHeight/3;

  ctx.font = lineHeight.toString() + "px Ubuntu";

  lineHeight = parseInt(ctx.font.match(/^(\d+)/)[0]);

  ctx.fillText("x: " + this.x.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("y: " + this.y.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("height: " + this.h.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("width: " + this.w.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;

  ctx.fillText("rotation: " + this.r.toFixed(2), this.x + xpad, tmpy);
  tmpy += lineHeight;
  ctx.fillText("opacity: " + this.o.toFixed(2), this.x + xpad, tmpy);

  tmpy += lineHeight;
  ctx.fillText("xscale: " + this.xs.toFixed(2), this.x + xpad, tmpy);

  tmpy += lineHeight;
  ctx.fillText("yscale: " + this.ys.toFixed(2), this.x + xpad, tmpy);*/

  if (this.PCView) {
    ctx.translate(this.x, this.y);
    this.PCView.draw(ctx, onGhost);
  }

  ctx.restore();
}

function CanvasState(canvas, ghostcanvas) {
  /*First some setup!*/
  this.canvas = canvas;
  this.width = canvas.width;
  this.height = canvas.height;
  this.ctx = canvas.getContext('2d');

  this.ghostcanvas = ghostcanvas;
  this.ghostcanvas.height = canvas.height;
  this.ghostcanvas.width = canvas.width;
  this.gctx = ghostcanvas.getContext('2d');

  /*This complicates things a little but
     fixes mouse co-ordinate problems
     when there's a border or padding.
    See getMouse for more detail.*/

  var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
  if (document.defaultView && document.defaultView.getComputedStyle) {
	  this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['paddingLeft'], 10) || 0;
	  this.stylePaddingTop = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['paddingTop'], 10) || 0;
	  this.styleBorderLeft = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['borderLeftWidth'], 10) || 0;
	  this.styleBorderTop = parseInt(document.defaultView.getComputedStyle(
     canvas, null)['borderTopWidth'], 10)   || 0;
  }

  /*Some pages have fixed-position bars (like the stumbleupon bar)
     at the top or left of the page.
    They will mess up mouse coordinates and this fixes that.*/

  var html = document.body.parentNode;
  this.htmlTop = html.offsetTop;
  this.htmlLeft = html.offsetLeft;

  /*Keep track of state!*/
  /*When set to false, the canvas will redraw everything.*/
  this.valid = false;
  /*The collection of things to be drawn.*/
  this.shapes = [];
  /*Keep track of when we are dragging.*/
  this.dragging = false;
  this.resizeDragging = false;
  this.perspectiveCorrectionDragging = false;
  this.expectResize = -1;
	this.expectMovePerspectiveCorrectionHandle = -1;
  /*The current selected object.
    In the future we could turn this into an array for multiple selection.*/
  this.selection = null;

  /*See mousedown and mousemove events for explanation.*/
  this.dragoffx = 0;
  this.dragoffy = 0;
  this.alt = false;

  /*Then events!*/

  /*This is an example of a closure!
    Right here "this" means the CanvasState.
    But we are making events on the Canvas itself,
     and when the events are fired on the canvas the variable
     "this" is going to mean the canvas!
    Since we still want to use this particular CanvasState
     in the events we have to save a reference to it.
    This is our reference!*/
  var myState = this;

  /*Fixes a problem where double clicking causes text
     to get selected on the canvas.*/
  canvas.addEventListener('selectstart', function(e) {
    e.preventDefault(); return false;
  }, false);

  canvas.addEventListener('mousedown', function(e) {
    var mouse = myState.getMouse(e);
    var mx = mouse.x;
    var my = mouse.y;
    var shapes = myState.shapes;
	  var l = shapes.length;
    var mySel = null;

    if (myState.expectMovePerspectiveCorrectionHandle !== -1) {
      myState.perspectiveCorrectionDragging = true;
      return;
    }

	  if (myState.expectResize !== -1) {
	    myState.resizeDragging = true;
	    return;
	  }

    for (var i = l-1; i >= 0; i--) {
      shapes[i].selected = false;
        $('#' + shapes[i].type + '_' + shapes[i].num).removeClass('selected');
    }

    myState.clearGhost();
    for (var i = l-1; i >= 0; i--) {
      /*Draw shape onto ghost context.*/
	    shapes[i].draw(myState.gctx, true);

	    /*Get image data at the mouse x,y pixel.*/
	    var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	    /*If the mouse pixel exists, select and break.*/
	    if (imageData.data[3] > 0) {
		    mySel = shapes[i];
		    myState.dragoffx = mx - mySel.x;
		    myState.dragoffy = my - mySel.y;
		    myState.dragging = true;
		    mySel.selected = true;
  		  myState.selection = mySel;
		    myState.valid = false;
		    myState.clearGhost();
		    $('#' + mySel.type + '_' + mySel.num).addClass('selected');
		    return;
	    }
	  }

    myState.clearGhost();

    if (myState.selection) {
      myState.selection.selected = false;
      $('#' + myState.selection.type + '_'
       + myState.selection.num).removeClass('selected');
      myState.selection = null;
      /*Need to clear the old selection border.*/
      myState.valid = false;
	  }
  }, true);

  canvas.addEventListener('mousemove', function(e) {
	  if (myState.dragging) {
      /*Something's dragging so we must redraw.*/
	    myState.valid = false;
	    var mouse = myState.getMouse(e);
	    /*We don't want to drag the object by its top-left corner,
         we want to drag it from where we clicked.
        Thats why we saved the offset and use it here.*/
      myState.selection.x = mouse.x - myState.dragoffx;
	    myState.selection.y = mouse.y - myState.dragoffy;
	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num,
       myState.selection.type, "x", myState.selection.x);
	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num,
       myState.selection.type, "y", myState.selection.y);
	    return;
    } else if (myState.perspectiveCorrectionDragging) {
      var mouse = myState.getMouse(e);
      var mx = mouse.x;
      var my = mouse.y;

      mx = mx - myState.selection.x - myState.selection.w*0.5;
      my = my - myState.selection.y - myState.selection.h*0.5;
      mx = Math.cos(myState.selection.r) * mx
       + Math.sin(myState.selection.r) * my;
      my = -Math.sin(myState.selection.r) * mx
       + Math.cos(myState.selection.r) * my;

      myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].x
       = mx/myState.selection.xs + myState.selection.w*0.5;
      myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].y
       = my/myState.selection.ys + myState.selection.h*0.5;
      kr.debug(myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].x
       + " " + myState.selection.PCView.handles[
       myState.expectMovePerspectiveCorrectionHandle].y);

      myState.valid = false;
      return;

	  } else if (myState.resizeDragging) {
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;
	    var oldx = myState.selection.x;
	    var oldy = myState.selection.y;
	    var shapes = myState.shapes;
	    var l = shapes.length;

      /*0  1  2
        3     4
        5  6  7*/
      switch (myState.expectResize) {
	      case 0:
          var xscale = (myState.selection.xs * myState.selection.w + oldx - mx) / myState.selection.w ;
          var yscale = (myState.selection.ys * myState.selection.h + oldy - my) / myState.selection.h ;
          var y;
          var x;

          if (xscale > yscale) {
            yscale = xscale;
            y = oldy + myState.selection.h * myState.selection.ys - yscale * myState.selection.h;
            x = mx;
          } else {
            xscale = yscale;
            x = oldx + myState.selection.w * myState.selection.xs - xscale * myState.selection.w;
            y = my;
          }

          kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type, "xscale", xscale);
          kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type, "x", x);

          myState.selection.xs = xscale;
          myState.selection.x = x;

          kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type, "yscale", xscale);
          kr.ux.rack_comp_ctrl("Comper", myState.selection.type, "y", y);

          myState.selection.ys = xscale;
          myState.selection.y = y;
          break;

        case 1:
	  	    var ys = (myState.selection.ys * myState.selection.h + oldy - my)
           / myState.selection.h ;
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.type,
           "yscale", ys);
          kr.ux.rack_comp_ctrl("Comper", myState.selection.type,
           "y", my);

          myState.selection.ys = ys;
          myState.selection.y = my;

          break;
        case 2:
          var yscale = (myState.selection.ys * myState.selection.h + oldy - my)
           / myState.selection.h ;
          var xscale = (mx - oldx) / myState.selection.w;
          var y;
          if (xscale > yscale) {
            yscale = xscale;
            y = oldy + myState.selection.h * myState.selection.ys
             - yscale * myState.selection.h;
          } else {
            xscale = yscale;
            y = my;
          }
          myState.selection.y = y;
          myState.selection.ys = xscale;

          kr.ux.rack_comp_ctrl("Comper", myState.selection.type,
           "y", y);
          kr.ux.rack_comp_ctrl("Comper", myState.selection.type,
           "yscale", xscale);
          myState.selection.xs = xscale;
          kr.ux.rack_comp_ctrl("Comper", myState.selection.type,
           "xscale", xscale);
	  	    break;
	      case 3:
	  	    var xs = (myState.selection.xs * myState.selection.w + oldx - mx)
           / myState.selection.w ;
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "xscale", xs);
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "x", mx);
          myState.selection.xs = xs;
          myState.selection.x = mx;
	  	    break;
	  	  case 4:
          kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "xscale", (mx - oldx) / myState.selection.w);
          myState.selection.xs = (mx - oldx) / myState.selection.w;
          break;

        case 5:
          var xscale = (myState.selection.xs * myState.selection.w + oldx - mx)
           / myState.selection.w ;
          var yscale = (my - oldy) / myState.selection.h;
          var x;

          if (xscale > yscale) {
            x = mx;
          } else {
            xscale = yscale;
            x = oldx + myState.selection.w * myState.selection.xs
             - xscale * myState.selection.w;
          }
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "x", x);

	  	    myState.selection.xs = xscale;
	  	    myState.selection.x = x;

	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "yscale", xscale);
	  	    myState.selection.ys = xscale;
	  	    break;

	      case 6:
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "yscale", (my - oldy) / myState.selection.h);
	  	    myState.selection.ys = (my - oldy) / myState.selection.h;
	  	    break;
	      case 7:
          var xscale = (mx - oldx) / myState.selection.w;
          var yscale = (my - oldy) / myState.selection.h;
          xscale = xscale > yscale ? xscale : yscale;

          myState.selection.xs = myState.selection.ys = xscale;
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "yscale", xscale);
	  	    break;
	      case 8:
	  	    var angle = Math.atan2((mx - (myState.selection.x + 0.5
           * myState.selection.xs*myState.selection.w)),
           -(my - (myState.selection.y+0.5*myState.selection.ys
           * myState.selection.h)));
	  	    myState.selection.r = angle;
	  	    kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "rotation", angle *180/Math.PI);
	  	    break;
	      case 9:
	  	    myState.selection.o = -0.5+2*(myState.height-mouse.y)/myState.height;
          kr.ux.rack_comp_ctrl("Comper", myState.selection.num, myState.selection.type,
           "opacity", myState.selection.o);
	  	    break;
	    }

	    myState.valid = false;
	  }

	  if (myState.selection !== null && !myState.perspectiveCorrectionDragging) {
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;

      /*Look for mouse on a perspective correction handle.*/
      myState.clearGhost();

	    myState.selection.draw(myState.gctx, true);

      /*Get image data at the mouse x,y pixel.*/
	    var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	    if (imageData.data[0] == 254) {
	  	  myState.expectMovePerspectiveCorrectionHandle = imageData.data[1];
	  	  myState.valid = false;
        this.style.cursor='move';
	  	  myState.clearGhost();
	  	  return;
	    }
      this.style.cursor='auto';
      myState.expectMovePerspectiveCorrectionHandle = -1;
	    myState.clearGhost();
    }
    if (myState.selection !== null && !myState.resizeDragging) {
	    myState.clearGhost();
	    var found = false;
	    var shapes = myState.shapes;
	    var l = shapes.length;
	    var mouse = myState.getMouse(e);
	    var mx = mouse.x;
	    var my = mouse.y;

	    for (var i = l-1; i >= 0; i--) {
	  	  /*Draw shape onto ghost context.*/
	  	  shapes[i].draw(myState.gctx, true);

	  	  /*Get image data at the mouse x,y pixel.*/
	  	  var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	  	  /*If the mouse pixel exists, select and break.*/
	  	  if (imageData.data[0] == 255) {
	  	    myState.expectResize = imageData.data[1];
	  	    myState.clearGhost();
	  	    myState.valid = false;

	  	    /*We found one!*/
	  	    switch (myState.expectResize) {
	  	      case 0:
	  		      this.style.cursor='nw-resize';
	  		      break;
	  	      case 1:
	  		      this.style.cursor='n-resize';
	  		      break;
	  	      case 2:
	  		      this.style.cursor='ne-resize';
	  		      break;
	  	      case 3:
	  		      this.style.cursor='w-resize';
	            break;
	  	      case 4:
	  		      this.style.cursor='e-resize';
	  		      break;
	  	      case 5:
	  		      this.style.cursor='sw-resize';
	  		      break;
	  	      case 6:
	  		      this.style.cursor='s-resize';
	  		      break;
	  	      case 7:
	  		      this.style.cursor='se-resize';
	  		      break;
	  	      case 8:
	  		      this.style.cursor='move';
	  		      break;
            case 9:
              this.style.cursor='move';
	            break;
	        }
	  	    return;
	  	  }
	    }
      /*Not over a selection box, return to normal.*/
	    myState.resizeDragging = false;
	    myState.expectResize = -1;
	    this.style.cursor='auto';
	  }
  }, true);

  canvas.addEventListener('mouseup', function(e) {
    myState.perspectiveCorrectionDragging = false;
    myState.expectMovePerspectiveCorrectionHandle = -1;
    myState.dragging = false;
    myState.resizeDragging = false;
  }, true);

  this.interval = 100;
  (function animloop(){
	  requestAnimFrame(animloop);
	  myState.draw();
  })();
}

CanvasState.prototype.addShape = function(shape) {
  this.shapes.push(shape);
  this.valid = false;
}

CanvasState.prototype.removeShape = function(subunit_type, subunit_id) {
  var l = this.shapes.length;
  for (var i = 0; i < l; i++) {
    if (this.shapes[i].num == subunit_id
     && this.shapes[i].type == subunit_type) {
      this.shapes.splice(i, 1);
      this.valid = false;
      return;
    }
  }
}

CanvasState.prototype.clear = function() {
  this.ctx.clearRect(0, 0, this.width, this.height);
}

CanvasState.prototype.clearGhost = function() {
  this.gctx.clearRect(0, 0, this.width, this.height);
}

/*While draw is called as often as the INTERVAL variable demands,
   it only ever does something if the canvas gets invalidated by our code.*/
CanvasState.prototype.draw = function() {
  /*If our state is invalid, redraw and validate!*/
  if (!this.valid) {
    var ctx = this.ctx;
    var shapes = this.shapes;
    this.clear();

    /*Add stuff you want drawn in the background all the time here.*/

	  /*Draw all shapes.*/
	  var l = shapes.length;
	  for (var i = 0; i < l; i++) {
	    var shape = shapes[i];
      shape.draw(ctx);
    }

    /*Add stuff you want drawn on top all the time here.*/
	  this.valid = false;
  }
}

/*Creates an object with x and y defined, set to the mouse position
   relative to the state's canvas if you wanna be super-correct
   this can be tricky, we have to worry about padding and borders.*/
CanvasState.prototype.getMouse = function(e) {
  var element = this.canvas, offsetX = 0, offsetY = 0, mx, my;
  /*Compute the total offset.*/
  if (element.offsetParent !== undefined) {
    do {
      offsetX += element.offsetLeft;
      offsetY += element.offsetTop;
    } while (element = element.offsetParent);
  }

  /*Add padding and border style widths to offset.
    Also add the <html> offsets in case there's a position:fixed bar*/
  offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
  offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;
  mx = e.pageX - offsetX;
  my = e.pageY - offsetY;

  /*We return a simple javascript object (a hash) with x and y defined.*/
  return {x: mx, y: my};
}
