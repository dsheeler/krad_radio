Kr.prototype.comp_ctrl = function(num, type, control_name, value) {
  var t;
  if (type == "videoport") {
    t = "v";
  } else {
    t = type;
  }
  kr.ctrl(t + "/" + num + "/" + control_name, value);
}

Kr.prototype.load_interface = function() {
  this.interface_data.portgroup_volume_sliders = new Object();
  this.interface_data.portgroup_crossfade_sliders = new Object();
  this.interface_data.portgroup_effect_sliders = new Object();
  this.interface_data.portgroup_eq_sliders = new Object();
    /* set up the selection handle boxes */
  this.selectionHandles = [];

  for (var i = 0; i < 10; i ++) {
    var rect = new Shape;
    this.selectionHandles.push(rect);
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
	  kr.selectionHandles[0].x = this.x;
	  kr.selectionHandles[0].y = this.y;
	      
	  kr.selectionHandles[1].x = this.x+this.w/2;
	  kr.selectionHandles[1].y = this.y;
	      
	  kr.selectionHandles[2].x = this.x+this.w;
	  kr.selectionHandles[2].y = this.y;
	      
	  /*Middle left handle.*/
	  kr.selectionHandles[3].x = this.x;
	  kr.selectionHandles[3].y = this.y+this.h/2;
	      
	  /*Middle right handle.*/
	  kr.selectionHandles[4].x = this.x+this.w;
	  kr.selectionHandles[4].y = this.y+this.h/2;
	      
    /*Bottom left, middle, and right handles.*/
    kr.selectionHandles[6].x = this.x+this.w/2;
    kr.selectionHandles[6].y = this.y+this.h;
	      
    kr.selectionHandles[5].x = this.x;
    kr.selectionHandles[5].y = this.y+this.h;
    
    kr.selectionHandles[7].x = this.x+this.w;
    kr.selectionHandles[7].y = this.y+this.h;

    /*Rotation handle.*/
    kr.selectionHandles[8].x = this.x+0.5*this.w;
    kr.selectionHandles[8].y = this.y- 3*this.selectionBoxSize;
	      
    /*Opacity handle.*/
    kr.selectionHandles[9].x = this.x+0.5*this.w;
    kr.selectionHandles[9].y = this.y+0.5*this.h;

    for (var i = 0; i < 10; i ++) {
      var cur = kr.selectionHandles[i];
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

  ctx.fillText(this.type + "_" + this.num, this.x, tmpy);
  tmpy += lineHeight;

  lineHeight = lineHeight/3;
    
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
  ctx.fillText("yscale: " + this.ys.toFixed(2), this.x + xpad, tmpy);

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
	    kr.comp_ctrl(myState.selection.num, 
       myState.selection.type, "x", myState.selection.x);
	    kr.comp_ctrl(myState.selection.num, 
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
          var w = myState.selection.w + oldx - mx
          var h = myState.selection.h + oldy - my 
          var y;
          var x;
  
          if (w/h >= myState.selection.aspectRatio) {
            h = w/myState.selection.aspectRatio;
            y = oldy;
            x = mx;
          } else {
            w = h/myState.selection.aspectRatio;
            x = oldx;
            y = my;
          }
                  
	        kr.comp_ctrl(myState.selection.num, myState.selection.type, 
           "width", w);
	        kr.comp_ctrl(myState.selection.num, myState.selection.type, 
           "x", x);
	  	            
	  	    myState.selection.xs = 1;
          myState.selection.w = w
	  	    myState.selection.x = x;
  
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type, 
           "height", h);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type, 
           "y", y);
		            
	  	    myState.selection.ys = 1;
          myState.selection.h = h
	  	    myState.selection.y = y;
		            
	  	    break;
		            
	      case 1:
	  	    var ys = (myState.selection.ys * myState.selection.h + oldy - my) 
           / myState.selection.h ;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", ys);
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
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

          kr.comp_ctrl(myState.selection.num, myState.selection.type, 
           "y", y);
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale);
          myState.selection.xs = xscale;
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    break;
	      case 3:
	  	    var xs = (myState.selection.xs * myState.selection.w + oldx - mx)
           / myState.selection.w ;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xs);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "x", mx);
          myState.selection.xs = xs;
          myState.selection.x = mx;
	  	    break;
	  	  case 4:
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
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
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "x", x);
		            
	  	    myState.selection.xs = xscale;
	  	    myState.selection.x = x;
		            
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale); 
	  	    myState.selection.ys = xscale;
	  	    break;
		            
	      case 6:
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", (my - oldy) / myState.selection.h);
	  	    myState.selection.ys = (my - oldy) / myState.selection.h;
	  	    break;  
	      case 7:
          var xscale = (mx - oldx) / myState.selection.w;
          var yscale = (my - oldy) / myState.selection.h;
          xscale = xscale > yscale ? xscale : yscale;
                  
          myState.selection.xs = myState.selection.ys = xscale; 
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "xscale", xscale);
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "yscale", xscale);
	  	    break;  
	      case 8:
	  	    var angle = Math.atan2((mx - (myState.selection.x + 0.5
           * myState.selection.xs*myState.selection.w)), 
           -(my - (myState.selection.y+0.5*myState.selection.ys
           * myState.selection.h)));
	  	    myState.selection.r = angle;
	  	    kr.comp_ctrl(myState.selection.num, myState.selection.type,
           "rotation", angle *180/Math.PI);
	  	    break;
	      case 9:
	  	    myState.selection.o = -0.5+2*(myState.height-mouse.y)/myState.height;
          kr.comp_ctrl(myState.selection.num, myState.selection.type,
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
	  this.valid = true;
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

Kr.prototype.got_system_cpu_usage = function(system_cpu_usage) {
  this.system_cpu_usage = system_cpu_usage;
  $('.system_cpu_usage').html(this.system_cpu_usage);
}

Kr.prototype.got_sysname = function(sysname) {
  this.sysname = sysname;
  $('#kradradio').append("<div class='kradradio_station' id='"
   + this.sysname + "'>\
<h2 class='sysname'>" + this.sysname + "</h2>\
<br clear='both'>\
<div class='kradcompositor'></div>\
<br clear='both'>\
<div class='kradmixer'>\
</div>\
<br clear='both'>\
<div class='kradmixer_beta'></div>\
<br clear='both'>\
<div class='kradmixer_cappa'></div>\
<br clear='both'>\
<div class='kradradio'>\
<div class='kradradio_tools'></div>\
<br clear='both'>\
</div>\
<br clear='both'>\
<div class='kradlink'></div>\
<br clear='both'>\
<div class='kradmixer_info'></div>\
<br clear='both'>\
<div class='kradcompositor_info'></div>\
<br clear='both'>\
<div class='kradlink_info'></div>\
<br clear='both'>\
<div class='kradmixer_tools'></div>\
<br clear='both'>\
<div class='kradcompositor_tools'></div>\
<br clear='both'>\
<div class='kradlink_tools'></div>\
<br clear='both'>\
</div>");
    
}

Kr.prototype.kradcompositor_controls = function () {
  $('.kradcompositor').append("<br clear='both'/>");
}


Kr.prototype.got_tag = function (tag_item, tag_name, tag_value) {
  for (t in this.tags) {
    if ((this.tags[t].tag_item == tag_item)
     && (this.tags[t].tag_name == tag_name)) {
      this.tags[t].update_value (tag_value);
      return;
	  }
  }
  tag = new Tag (tag_item, tag_name, tag_value);
  this.tags.push (tag);
}

Kr.prototype.portgroup_draw_peak_meter 
 = function(portgroup_name, value) {

  canvas = document.getElementById(portgroup_name  + '_meter_canvas');
  if (!canvas) {
	  return;
  }
     
  ctx = canvas.getContext('2d');
  value = value * 2;
  inc = 5;
  dink = -4;
  width = 42;
  height = 200;
  ctx.fillStyle = '#11ff11';
  x = 0;
  while (x <= value) {
	  ctx.fillRect(0, height - x, width, dink);
	  x = x + inc;
  }
  ctx.fillStyle = '#22aa22';
  x = 0;
  while (x <= (height - value)) {
	  ctx.fillRect(0, 0 + x, width,  dink);
	  x = x + inc;
  }
}

Kr.prototype.got_peak_portgroup = function(portgroup_name, value) {
  value = Math.round(value);
  if (this.peaks_max[portgroup_name] < this.peaks[portgroup_name]) {
    this.peaks_max[portgroup_name] = this.peaks[portgroup_name];
  }
  if (this.peaks[portgroup_name] == value) {
    return;
  } else {
    this.peaks[portgroup_name] = value;
  }
  this.portgroup_draw_peak_meter (portgroup_name, value);
}

Kr.prototype.got_update_subunit = function(num, type, 
 control_name, value) {
  for (var i = 0; i < kr.compositor_canvas.shapes.length; i++) {
    if (kr.compositor_canvas.shapes[i].type == type
     && (kr.compositor_canvas.shapes[i].num == num)) {
      kr.compositor_canvas.shapes[i].setValue(control_name, value);
      kr.compositor_canvas.valid = false;
      break;
    }
  }
}

Kr.prototype.clear_max_peak = function(value) {
  this.peaks_max[value] = -1;
  this.portgroup_draw_peak_meter (value, -1);
}

Kr.prototype.got_control_portgroup = function(portgroup_name,
 control_name, value) {
  if ($('#' + portgroup_name)) {
    if (control_name == "volume") {
      this.interface_data.portgroup_volume_sliders[portgroup_name].set(value);
    } else {
      this.interface_data.portgroup_crossfade_sliders[portgroup_name].set(value);
    }
  }
}

Kr.prototype.got_effect_control = function(crate) {
  if (crate.effect_name == "eq") {
    if ($('#' + crate.portgroup_name + '_' + crate.effect_name
     + '_' + crate.effect_num + '_' + crate.control_name)) {
      this.interface_data.portgroup_eq_sliders[crate.portgroup_name + '_' + crate.effect_name
     + '_' + crate.effect_num + '_' + crate.control_name].set(crate.value);
    }
  } else {
    if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_'
     + crate.control_name)) {
      this.interface_data.portgroup_effect_sliders[crate.portgroup_name + '_'
       + crate.effect_name + '_'
       + crate.control_name].set(crate.value);
    }
  }
}

Kr.prototype.got_update_portgroup = function (portgroup_name,
 control_name, value) {
  if (control_name == "crossfade_group") {
    kr.portgroup_handle_crossfade_group(portgroup_name, value, -100.0);
  }
    
  if (control_name == "xmms2_ipc_path") {
    if (value.length > 0) {
      kr.portgroup_handle_xmms2(portgroup_name, 1);
    } else {
      kr.portgroup_handle_xmms2(portgroup_name, 0);
    }
  }
}

Kr.prototype.portgroup_handle_crossfade_group = function(portgroup_name,
 crossfade_name, crossfade) {
  if ($('#portgroup_' + portgroup_name + '_crossfade_wrap')) {
    $('#portgroup_' + portgroup_name + '_crossfade_wrap').remove();
  }

  if (crossfade_name.length > 0) {
	  $('#portgroup_' + portgroup_name + '_wrap').after("<div id='portgroup_"
     + portgroup_name + "_crossfade_wrap' " 
     + "class='kradmixer_control crossfade_control'> <div id='"
     + portgroup_name + "_crossfade' class='crossfade crossfade_control'>"
     + "<div class='fade'><div class='handle'></div></div><h2>" + portgroup_name
     + " - " + crossfade_name + "</h2></div></div>");
  
  var crossfade_element = document.id(portgroup_name + "_crossfade");
  var crossfade_slider = crossfade_element.getElement('.fade');

  this.interface_data.portgroup_crossfade_sliders[portgroup_name] = new Slider(crossfade_slider,
   crossfade_slider.getElement('.handle'), {
     range: [-100, 100],
     steps: 400,
     wheel: true,
     mode: "horizontal",
     initialStep: crossfade,
     onChange: function (value) {
       if (value) kr.ctrl("m/" + portgroup_name + "/c", value)
      }
    });
  }
}

Kr.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {
  if (xmms2 == 0) {
    if ($('#' + portgroup_name + '_xmms2_wrap')) {
      $('#' + portgroup_name + '_xmms2_wrap').remove();
    }
  } else {
	  xmms2_controls = "<div id='" + portgroup_name
     + "_xmms2_wrap' class='kradmixer_control xmms2_control'><div id='"
     + portgroup_name + "_xmms2'></div></div>";
    if (this.xmms2_portgroup_count == 1) {
      $('.kradmixer_beta').prepend(xmms2_controls);
    } else {
      $('.kradmixer_beta').append(xmms2_controls);
    }

    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>"
     + "<div class='krad_button_small' id='"
     + portgroup_name + "_xmms2_prev'>PREV</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>" 
     + "<div class='krad_button_small' id='" + portgroup_name
     + "_xmms2_play'>PLAY</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>" 
     + "<div class='krad_button_small' id='" + portgroup_name
     + "_xmms2_pause'>PAUSE</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>" 
     + "<div class='krad_button_small' id='" + portgroup_name
     + "_xmms2_stop'>STOP</div></div>");
    $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'>" 
     + "<div class='krad_button_small' id='" + portgroup_name
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

Kr.prototype.got_add_subunit = function (crate) {
  if (!document.getElementById("compositor_canvas")) {
	  $('.kradcompositor').append("<div class='kradcompositor_control'>"
     + "<canvas width='960px' height='540px' id='compositor_canvas'>"
     + "</canvas></div><br clear='both'><div width='960px'"
     + " class='kradcompositor_subunit_list'><ul id='subunit_list'>"
     + "</ul></div>");
	      
    kr.compositor_canvas = new CanvasState(document.getElementById
     ("compositor_canvas"), document.createElement('canvas'));
  }

  if ((crate.subunit_type != 'videoport') || (crate.direction != 'output')) {
    var shape = new Shape(crate.x,crate.y, crate.width,
     crate.height, crate.r * Math.PI / 180.0, crate.o,
     crate.xscale, crate.yscale,
     crate.subunit_type, crate.subunit_id);

    kr.compositor_canvas.addShape(shape);

    $('#subunit_list').append('<li class=subunit id=' + shape.type + '_'
     + shape.num + '>' + shape.type + '_' + shape.num + '</li>');

    $('li.subunit:even').removeClass("alt");
    $('li.subunit:odd').addClass("alt");


    $('#' + shape.type + '_' + shape.num).bind("click", function(e,ui) {
	    e.preventDefault();
      var alreadySelected = $(this).hasClass('selected');
      var shapes = kr.compositor_canvas.shapes;
      var l = shapes.length;

      /*Run removeClass on every element.*/
	    $('.subunit').removeClass('selected');
	    for (var i = 0; i < l;  i++) {
	      shapes[i].selected = false;
	      if (shapes[i].num == shape.num && shapes[i].type == shape.type) {
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
  }
}

Kr.prototype.got_remove_subunit = function(crate) {
  kr.compositor_canvas.removeShape(crate.subunit_type,
   crate.subunit_id);

  $('#' + crate.subunit_type + '_' + crate.subunit_id).remove();
  $('li.subunit:even').removeClass("alt");
  $('li.subunit:odd').addClass("alt");
  
  kr.compositor_canvas.valid = false;
}

Kr.prototype.toggle_show_effects = function(portgroup_name) {
/*  document.id(portgroup_name + '_effects').toggle("slide", {}, 500);
  $('#' + portgroup_name + '_effects_eq').toggle("slide", {}, 500);*/
}

Kr.prototype.got_add_portgroup = function(crate) {
  this.peaks[crate.portgroup_name] = -1;
  this.peaks_max[crate.portgroup_name] = -1;
  this.portgroup_count += 1;
  if (crate.xmms2 == 1) {
    this.xmms2_portgroup_count += 1;
  }

  volume_div = "<div class='kradmixer_control volume_control' id='portgroup_" 
   + crate.portgroup_name + "_wrap'><div class='meter_wrapper'>"
   + "<canvas width='21' height='200' class='meter' id='" 
   + crate.portgroup_name + "_meter_canvas'></canvas></div><div id='" 
   + crate.portgroup_name + "'><div class='volume'>"
   + "<div class='handle'></div></div></div><div class='button_wrap'>"
   + "<div class='krad_button' id='show_effects_"
   + crate.portgroup_name + "'>FX</div></div><h2>" + crate.portgroup_name 
   + "</h2></div>";
 
  if (crate.portgroup_name == "DTMF") {
	      
	  $('.kradmixer_cappa').append("<div class='kradmixer_control dtmf_pad'>" 
     + " <div id='" + crate.portgroup_name + "_dtmf'> </div> </div>");
	      
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_1'>1</div></div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_2'>2</div></div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_3'>3</div></div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button2' id='" + crate.portgroup_name
     + "_dtmf_a'>F0</div></div>");
	      
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_4'>4</div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_5'>5</div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_6'>6</div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button2' id='" + crate.portgroup_name
     + "_dtmf_b'>F</div></div>");
	      
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_7'>7</div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_8'>8</div>");
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_9'>9</div>");    
	  $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button2' id='" + crate.portgroup_name
     + "_dtmf_c'>I</div></div>");
	  
    $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_star'>*</div>");    
    $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_0'>0</div>");
    $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button' id='" + crate.portgroup_name
     + "_dtmf_hash'>#</div>");
    $('#' + crate.portgroup_name + '_dtmf').append("<div class='button_wrap'>"
     + "<div class='krad_button2' id='" + crate.portgroup_name
     + "_dtmf_d'>P</div></div>");
    $('#' + crate.portgroup_name + '_dtmf_1').bind("click",
     function(event, ui) {
      kr.push_dtmf ("1");
    });
	      
  	$('#' + crate.portgroup_name + '_dtmf_2').bind("click",
     function(event, ui) {
  	  kr.push_dtmf ("2");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_3').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("3");
	  });
	        
	  $('#' + crate.portgroup_name + '_dtmf_4').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("4");
	  });            

	  $('#' + crate.portgroup_name + '_dtmf_5').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("5");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_6').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("6");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_7').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("7");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_8').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("8");
	  });  
	      
	  $('#' + crate.portgroup_name + '_dtmf_9').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("9");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_0').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("0");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_star').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("*");
	  });
	     
    $('#' + crate.portgroup_name + '_dtmf_hash').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("#");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_a').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("A");
	  });
	      
	  $('#' + crate.portgroup_name + '_dtmf_b').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("B");
	  });
	        
	  $('#' + crate.portgroup_name + '_dtmf_c').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("C");
	  });          

	  $('#' + crate.portgroup_name + '_dtmf_d').bind("click",
     function(event, ui) {
	    kr.push_dtmf ("D");
	  });
  }
    
	$('.kradmixer').append(volume_div);


  var name = crate.portgroup_name;
  var volume_element = document.id(crate.portgroup_name);
  var volume_slider = volume_element.getElement('.volume');

  this.interface_data.portgroup_volume_sliders[name] = new Slider(volume_slider,
   volume_slider.getElement('.handle'), {
      range: [100, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.volume,
        onChange: function (value) {
         kr.ctrl("m/" + crate.portgroup_name + "/v", value)
       }
    });
 
  $('#' +  crate.portgroup_name + '_meter_canvas').bind("click",
   function(event, ui) {
	  kr.clear_max_peak(crate.portgroup_name);
  });

  if (crate.crossfade_name.length > 0) {
	  kr.portgroup_handle_crossfade_group(crate.portgroup_name,
     crate.crossfade_name, crate.crossfade);
  }
    
  if (crate.xmms2 == 1) {
	  kr.portgroup_handle_xmms2(crate.portgroup_name, crate.xmms2);
  }

  if (crate.direction == 1) {
	  $('.kradmixer_cappa').append("<div class='effect_area' float:left'>"
     + "<div id='" + crate.portgroup_name 
     + "_effects' style='float:left'><h2>Effects for "
     + crate.portgroup_name + "</h2></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_lowpass_hz'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_lowpass_bw'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_highpass_hz'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_highpass_bw'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_analog_drive'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

	  $('#' + crate.portgroup_name + '_effects').append
     ("<div class='kradmixer_control' id='"
     + crate.portgroup_name + "_analog_blend'><div class='volume'>"
     + "<div class='handle'></div></div></div>");

    
    var name = crate.portgroup_name + "_lowpass_hz";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider,
     effect_slider.getElement('.handle'), {
       range: [12000, 20],
       steps: 200,
       wheel: true,
       mode: "vertical",
       initialStep: crate.lowpass_hz,
       onChange: function (value) {
         if (value) kr.ctrl("e/" + crate.portgroup_name + "/lp/hz", value)
       }
     });

 	  var name = crate.portgroup_name + "_lowpass_bw";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider, 
     effect_slider.getElement('.handle'), {
       range: [5.0, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.lowpass_bw,
        onChange: function (value) {
          if (value) kr.ctrl("e/" + crate.portgroup_name + "/lp/bw", value)
        }
     });
 	      
	  var name = crate.portgroup_name + "_highpass_hz";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [20000, 20],
        steps: 1000,
        wheel: true,
        mode: "vertical",
        initialStep: crate.highpass_hz,
        onChange: function (value) {
          if (value) kr.ctrl("e/" + crate.portgroup_name + "/hp/hz", value)
        }
     });
	
	 	var name = crate.portgroup_name + "_highpass_bw";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [5, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.highpass_bw,
        onChange: function (value) {
          if (value) kr.ctrl("e/" + crate.portgroup_name + "/hp/bw", value)
        }
     });

    var name = crate.portgroup_name + "_analog_drive";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [10, 0],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.analog_drive,
        onChange: function (value) {
          if (value) kr.ctrl("e/" + crate.portgroup_name + "/a/drive", value)
        }
     });

    var name = crate.portgroup_name + "_analog_blend";
    var effect_element = document.id(name);
    var effect_slider = effect_element.getElement('.volume');

    this.interface_data.portgroup_effect_sliders[name] = new Slider(effect_slider, effect_slider.getElement('.handle'), {
      range: [10, -10],
        steps: 200,
        wheel: true,
        mode: "vertical",
        initialStep: crate.analog_blend,
        onChange: function (value) {
          if (value) kr.ctrl("e/" + crate.portgroup_name + "/a/blend", value)
        }
    });

    $('.kradmixer_cappa').append("<br clear='both'/>"
     + " <div id='" + crate.portgroup_name + "_effects_eq'"
     + "style='float:left'></div>");

    for (var i = 0; i < 32; i++) {
      $('#' + crate.portgroup_name + '_effects_eq').append
       ("<div class='kradmixer_control' id='"
       + crate.portgroup_name + "_eq_" + i + "_hz'><div class='volume'>"
       + "<div class='handle'></div></div></div>");

      var name = crate.portgroup_name + "_eq_" + i + "_hz";
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');
      var mini;
	    var maxi;

	    mini = crate.eq.bands[i].hz - crate.eq.bands[i].hz;
	    maxi = crate.eq.bands[i].hz + crate.eq.bands[i].hz;

      if (mini < 20) {
        mini = 20;
      }
      if (maxi > 20000) {
        maxi = 20000;
      }

      this.interface_data.portgroup_eq_sliders[name] = new Slider(eq_slider,
       eq_slider.getElement('.handle'), {
        range: [maxi, mini],
        steps: 200,
        wheel: true,
        mode: "vertical",
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/hz",
        initialStep: crate.eq.bands[i].hz,
        onChange: function (value) {
          if (value) kr.ctrl(this.options.address, value);
        }
      });
    }

    for (var i = 0; i < 32; i++) {
      $('#' + crate.portgroup_name + '_effects_eq').append
       ("<div class='kradmixer_control' id='"
       + crate.portgroup_name + "_eq_" + i + "_db'><div class='volume'>"
       + "<div class='handle'></div></div></div>");
      var name = crate.portgroup_name + "_eq_" + i.toString() + "_db";
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');

      this.interface_data[name] = new Slider(eq_slider,
       eq_slider.getElement('.handle'), {
        range: [12, -12],
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/db",
        initialStep: crate.eq.bands[i].db,
        onChange: function (value) {
          if (value) kr.ctrl(this.options.address, value);
        }
	    });
	  }
	        
	  for (var i = 0; i < 32; i++) {
	   
      $('#' + crate.portgroup_name + '_effects_eq').append
       ("<div class='kradmixer_control' id='"
       + crate.portgroup_name + "_eq_" + i + "_bw'><div class='volume'>"
       + "<div class='handle'></div></div></div>");
  		var name = crate.portgroup_name + "_eq_" + i + "_bw";
      var eq_element = document.id(name);
      var eq_slider = eq_element.getElement('.volume');
	    
      this.interface_data.portgroup_eq_sliders[name] = new Slider(eq_slider, eq_slider.getElement('.handle'), {
        range: [5.0, 0.1],
        steps: 200,
        wheel: true,
        mode: "vertical",
        address: "e/" + crate.portgroup_name + "/eq/" + i.toString() + "/bw",
        initialStep: crate.eq.bands[i].bw,
        onChange: function (value) {
          if (value) kr.ctrl(this.options.address, value);
        }
	    });
    }
  }
  
  this.portgroup_draw_peak_meter(crate.portgroup_name, 0);
 
}

Kr.prototype.got_remove_portgroup = function(name) {
  $('#portgroup_' + name + '_wrap').remove();
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
