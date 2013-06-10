function create_handler (inst, func) {
    
    return (function(e){
	      func.call(inst, e);            
    });
}

window.requestAnimFrame = (function(){
    return  window.requestAnimationFrame   || 
        window.webkitRequestAnimationFrame || 
        window.mozRequestAnimationFrame    || 
        window.oRequestAnimationFrame      || 
        window.msRequestAnimationFrame     || 
        function( callback ){
            window.setTimeout(callback, 1000 / 60);
        };
})();

function PerspectiveCorrectionView(x0, y0, x1, y1, x2, y2, x3, y3) {
    this.handles = [];
    this.handles[0] = new PerspectiveCorrectionHandle(0, x0, y0, this);
    this.handles[1] = new PerspectiveCorrectionHandle(1, x1, y1, this);
    this.handles[2] = new PerspectiveCorrectionHandle(2, x2, y2, this);
    this.handles[3] = new PerspectiveCorrectionHandle(3, x3, y3, this);
    this.on == false;
}

PerspectiveCorrectionView.prototype.draw = function(ctx, onGhost) {

    if (this.on == true && onGhost == false) {
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

	  // draw the perspective correction handles
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

    this.selectionHandleColor = '#117766'; // New for selection boxes
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
    // This is a very simple and unsafe constructor. All we're doing is checking if the values exist.
    // "x || 0" just means "if there is a value for x, use that. Otherwise use 0."
    // But we aren't checking anything else! We could put "Lalala" for the value of x 
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
    this.selectionBoxColor = 'darkred'; // New for selection boxes
    this.selectionBoxSize = 20;
    this.selectionColor = '#CC0000';
    this.selectionWidth = 2;  
    
    this.PCView = type == 'videoport' 
     ? new PerspectiveCorrectionView(0,0,this.w,0,0,this.h,this.w, this.h) 
     : null;
  

    
}

Shape.prototype.setValue = function(name, value) {
    if (name == "x") {
	      this.x = value;
    } else if (name == "y") {
	      this.y = value;
    } else if (name == "xscale") {
	      this.xs = value;
    } else if (name == "yscale") {
	      this.ys = value;
    } else if (name == "rotation") {
	      this.r = Math.PI / 180.0 * value;
    } else if (name == "opacity") {
	      this.o = value;
    } else if (name == "view_top_left_x") {
        this.PCView.handles[0].x = value;
    } else if (name == "view_top_left_y") {
        this.PCView.handles[0].y = value;
    } else if (name == "view_top_right_x") {
        this.PCView.handles[1].x = value;
    } else if (name == "view_top_right_y") {
        this.PCView.handles[1].y = value;
    } else if (name == "view_bottom_left_x") {
        this.PCView.handles[2].x = value;
    } else if (name == "view_bottom_left_y") {
        this.PCView.handles[2].y = value;
    } else if (name == "view_bottom_right_x") {
        this.PCView.handles[3].x = value;
    } else if (name == "view_bottom_right_y") {
        this.PCView.handles[3].y = value;
    }
}

// Draws this shape to a given context
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
    
    // draw selection
    // this is a stroke along the box and also new selection handles
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
	      // draw the handles
	      
	      // 0  1  2
	      // 3     4
	      // 5  6  7
	      
	      // top left, middle, right
	      kradradio.selectionHandles[0].x = this.x;
	      kradradio.selectionHandles[0].y = this.y;
	      
	      kradradio.selectionHandles[1].x = this.x+this.w/2;
	      kradradio.selectionHandles[1].y = this.y;
	      
	      kradradio.selectionHandles[2].x = this.x+this.w;
	      kradradio.selectionHandles[2].y = this.y;
	      
	      //middle left
	      kradradio.selectionHandles[3].x = this.x;
	      kradradio.selectionHandles[3].y = this.y+this.h/2;
	      
	      //middle right
	      kradradio.selectionHandles[4].x = this.x+this.w;
	      kradradio.selectionHandles[4].y = this.y+this.h/2;
	      
	      //bottom left, middle, right
	      kradradio.selectionHandles[6].x = this.x+this.w/2;
	      kradradio.selectionHandles[6].y = this.y+this.h;
	      
	      kradradio.selectionHandles[5].x = this.x;
	      kradradio.selectionHandles[5].y = this.y+this.h;
	      
	      kradradio.selectionHandles[7].x = this.x+this.w;
	      kradradio.selectionHandles[7].y = this.y+this.h;

	      kradradio.selectionHandles[8].x = this.x+0.5*this.w;
	      kradradio.selectionHandles[8].y = this.y- 3*this.selectionBoxSize;
	      
	      kradradio.selectionHandles[9].x = this.x+0.5*this.w;
	      kradradio.selectionHandles[9].y = this.y+0.5*this.h;


	      for (var i = 0; i < 10; i ++) {
	          var cur = kradradio.selectionHandles[i];
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

    ctx.fillText("width: " + this.w.toFixed(2), this.x + xpad, tmpy);
    tmpy += lineHeight;

    ctx.fillText("height: " + this.h.toFixed(2), this.x + xpad, tmpy);
    tmpy += lineHeight;

    ctx.fillText("rotation: " + (this.r * 180/Math.PI).toFixed(2), this.x + xpad, tmpy);

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
    // **** First some setup! ****
    
    this.canvas = canvas;
    this.width = canvas.width;
    this.height = canvas.height;
    this.ctx = canvas.getContext('2d');

    this.ghostcanvas = ghostcanvas;
    this.ghostcanvas.height = canvas.height;
    this.ghostcanvas.width = canvas.width;
    this.gctx = ghostcanvas.getContext('2d');


    //this.selectionFillColor = 'rgba(17,119,102,0.5)';

    // This complicates things a little but but fixes mouse co-ordinate problems
    // when there's a border or padding. See getMouse for more detail
    var stylePaddingLeft, stylePaddingTop, styleBorderLeft, styleBorderTop;
    if (document.defaultView && document.defaultView.getComputedStyle) {
	      this.stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(canvas, null)['paddingLeft'], 10)      || 0;
	      this.stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(canvas, null)['paddingTop'], 10)       || 0;
	      this.styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(canvas, null)['borderLeftWidth'], 10)  || 0;
	      this.styleBorderTop   = parseInt(document.defaultView.getComputedStyle(canvas, null)['borderTopWidth'], 10)   || 0;
    }
    // Some pages have fixed-position bars (like the stumbleupon bar) at the top or left of the page
    // They will mess up mouse coordinates and this fixes that
    var html = document.body.parentNode;
    this.htmlTop = html.offsetTop;
    this.htmlLeft = html.offsetLeft;

    // **** Keep track of state! ****
    
    this.valid = false; // when set to false, the canvas will redraw everything
    this.shapes = [];  // the collection of things to be drawn
    this.dragging = false; // Keep track of when we are dragging
    this.resizeDragging = false;
    this.perspectiveCorrectionDragging = false;
    this.expectResize = -1;
		this.expectMovePerspectiveCorrectionHandle = -1;
    // the current selected object. In the future we could turn this into an array for multiple selection
    this.selection = null;
    this.dragoffx = 0; // See mousedown and mousemove events for explanation
    this.dragoffy = 0;
    this.alt = false;

    
    // **** Then events! ****
    
    // This is an example of a closure!
    // Right here "this" means the CanvasState. But we are making events on the Canvas itself,
    // and when the events are fired on the canvas the variable "this" is going to mean the canvas!
    // Since we still want to use this particular CanvasState in the events we have to save a reference to it.
    // This is our reference!
    var myState = this;
    
    //fixes a problem where double clicking causes text to get selected on the canvas
    canvas.addEventListener('selectstart', function(e) { e.preventDefault(); return false; }, false);
    // Up, down, and move are for dragging


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
	          // draw shape onto ghost context
	          shapes[i].draw(myState.gctx, true);
	          
	          // get image data at the mouse x,y pixel
	          var imageData = myState.gctx.getImageData(mx, my, 1, 1);

	          // if the mouse pixel exists, select and break
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
	          $('#' + myState.selection.type + '_' + myState.selection.num).removeClass('selected');
	          myState.selection = null;
	          myState.valid = false; // Need to clear the old selection border
	      }    
    }, true);
    
    canvas.addEventListener('mousemove', function(e) {
	      if (myState.dragging){
	          myState.valid = false; // Something's dragging so we must redraw
	          var mouse = myState.getMouse(e);
	          // We don't want to drag the object by its top-left corner, we want to drag it
	          // from where we clicked. Thats why we saved the offset and use it here
	          myState.selection.x = mouse.x - myState.dragoffx;
	          myState.selection.y = mouse.y - myState.dragoffy;
	          kradradio.update_subunit(myState.selection.num, myState.selection.type, "x", myState.selection.x);
	          kradradio.update_subunit(myState.selection.num, myState.selection.type, "y", myState.selection.y);
	          return;

        } else if (myState.perspectiveCorrectionDragging) {
            var mouse = myState.getMouse(e);	
	          var mx = mouse.x;
	          var my = mouse.y;

            var Rcx = (myState.selection.x + 0.5 * myState.selection.w);
            var Rcy = (myState.selection.y + 0.5 * myState.selection.h);
            
//            mx = (mx - myState.selection.x) / myState.selection.x.xs; 
            var mtx =   Math.cos(myState.selection.r) * (mx - Rcx) / myState.selection.xs + Math.sin(myState.selection.r) * (my - Rcy) / myState.selection.ys;
            var mty =   -Math.sin(myState.selection.r) * (mx - Rcx)  / myState.selection.xs + Math.cos(myState.selection.r) * (my - Rcy)  / myState.selection.ys;
                          
            var seltx = Math.cos(myState.selection.r) *  (0.5*myState.selection.w/myState.selection.xs) 
                + Math.sin(myState.selection.r) * (0.5*myState.selection.h/myState.selection.ys); 
            var selty = -Math.sin(myState.selection.r) * (0.5*myState.selection.w/myState.selection.xs) 
                + Math.cos(myState.selection.r) * (0.5*myState.selection.h/myState.selection.ys); 

//            mtx = mtx / myState.slection.xs

            myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].x =  
                (mtx) + 0.5 * myState.selection.w / myState.selection.xs;   
            myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].y =  
                (mty) + 0.5 * myState.selection.h / myState.selection.ys;   


	          switch (myState.expectMovePerspectiveCorrectionHandle) {
	          case 0:
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_top_left_x",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].x);
                
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_top_left_y",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].y);
                break;

            case 1:

		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_top_right_x",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].x);
                
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_top_right_y",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].y);
                break;

            case 2:

		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_bottom_left_x",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].x);
                
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_bottom_left_y",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].y);
                break;

            case 3:

		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_bottom_right_x",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].x);
                
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "view_bottom_right_y",                 
                                         myState.selection.PCView.handles[myState.expectMovePerspectiveCorrectionHandle].y);
                break;
            }


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
	          
	          // 0  1  2
	          // 3     4
	          // 5  6  7
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
                
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", xscale);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "x", x);
		            
		            myState.selection.xs = xscale;
		            myState.selection.x = x;


		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", xscale);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "y", y);
		            
		            myState.selection.ys = xscale;
		            myState.selection.y = y;
		            
		            break;
		            
	          case 1:
		            var ys = (myState.selection.ys * myState.selection.h + oldy - my) / myState.selection.h ;
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", ys);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "y", my);
		            
		            myState.selection.ys = ys;
		            myState.selection.y = my;
		            
		            break;
	          case 2:
		            var yscale = (myState.selection.ys * myState.selection.h + oldy - my) / myState.selection.h ;
                var xscale = (mx - oldx) / myState.selection.w;
                var y;
                if (xscale > yscale) {
                    yscale = xscale;
                    y = oldy + myState.selection.h * myState.selection.ys - yscale * myState.selection.h;
                    
                } else {
                    xscale = yscale;
                    y = my;


                }
                
                myState.selection.y = y;
		            myState.selection.ys = xscale;

                kradradio.update_subunit(myState.selection.num, myState.selection.type, "y", y);
                kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", xscale);		      
		            
                myState.selection.xs = xscale;
				        
                kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", xscale);
				        
		            break;
	          case 3:
		            var xs = (myState.selection.xs * myState.selection.w + oldx - mx) / myState.selection.w ;
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", xs);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "x", mx);

		            myState.selection.xs = xs;
		            myState.selection.x = mx;
		            
		            break;
		            
	          case 4:
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", (mx - oldx) / myState.selection.w);
		            myState.selection.xs = (mx - oldx) / myState.selection.w;
		            break;
		            
	          case 5:
		            var xscale = (myState.selection.xs * myState.selection.w + oldx - mx) / myState.selection.w ;
                var yscale = (my - oldy) / myState.selection.h;
                var x;

                if (xscale > yscale) {
                    x = mx;
                } else {
                    xscale = yscale;
                    x = oldx + myState.selection.w * myState.selection.xs - xscale * myState.selection.w;
                }
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", xscale);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "x", x);
		            
		            myState.selection.xs = xscale;
		            myState.selection.x = x;
		            
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", xscale); 
		            myState.selection.ys = xscale;
		            break;
		            
	          case 6:
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", (my - oldy) / myState.selection.h);
		            myState.selection.ys = (my - oldy) / myState.selection.h;
		            break;
		            
	          case 7:
                var xscale = (mx - oldx) / myState.selection.w;
                var yscale = (my - oldy) / myState.selection.h;
                xscale = xscale > yscale ? xscale : yscale;
                
                
		            myState.selection.xs = myState.selection.ys = xscale; 
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "xscale", xscale);
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "yscale", xscale);
		            break;
		            
	          case 8:
		            var angle = Math.atan2((mx - (myState.selection.x+0.5*myState.selection.xs*myState.selection.w)), 
                                       -(my - (myState.selection.y+0.5*myState.selection.ys * myState.selection.h)));
		            myState.selection.r = angle;
		            kradradio.update_subunit(myState.selection.num, myState.selection.type, "rotation", angle *180/Math.PI);
		            break;

	          case 9:
		            myState.selection.o = -0.5+2*(myState.height-mouse.y)/myState.height;
                kradradio.update_subunit(myState.selection.num, myState.selection.type, "opacity", myState.selection.o);

		            break;
	          }
	          
	          myState.valid = false;
	      }

	      if (myState.selection !== null && !myState.perspectiveCorrectionDragging) {
	          var mouse = myState.getMouse(e);	
	          var mx = mouse.x;
	          var my = mouse.y;

            // look for mouse on a perspective correction handle
            myState.clearGhost();
            
	          myState.selection.draw(myState.gctx, true);
	              
	          // get image data at the mouse x,y pixel
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
		            // draw shape onto ghost context
		            shapes[i].draw(myState.gctx, true);
                
		            // get image data at the mouse x,y pixel
		            var imageData = myState.gctx.getImageData(mx, my, 1, 1);
                
		            // if the mouse pixel exists, select and break
		            if (imageData.data[0] == 255) {
		                myState.expectResize = imageData.data[1];
		                myState.clearGhost();
                    
		                myState.valid = false;
                    
		                // we found one!
		                
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
	          

	          // not over a selection box, return to normal
           
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
    // double click for making new shapes
    //canvas.addEventListener('dblclick', function(e) {
    //  var mouse = myState.getMouse(e);
    //  myState.addShape(new Shape(mouse.x - 10, mouse.y - 10, 20, 20, 'rgba(0,255,0,.6)'));
    // }, true);
    
    // **** Options! ****

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
        if (this.shapes[i].num == subunit_id && this.shapes[i].type == subunit_type) {
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

// While draw is called as often as the INTERVAL variable demands,
// It only ever does something if the canvas gets invalidated by our code
CanvasState.prototype.draw = function() {
    // if our state is invalid, redraw and validate!
    if (!this.valid) {
	      var ctx = this.ctx;
	      var shapes = this.shapes;
	      this.clear();
	      
	      // ** Add stuff you want drawn in the background all the time here **
	      
	      // draw all shapes
	      var l = shapes.length;
	      for (var i = 0; i < l; i++) {
	          var shape = shapes[i];
	          // We can skip the drawing of elements that have moved off the screen:
	          //    if (shape.x > this.width || shape.y > this.height ||
	          //        shape.x + shape.w < 0 || shape.y + shape.h < 0) continue;
	          shapes[i].draw(ctx);
	      }
	      
//	      this.PCView.draw(ctx, false);
	      // ** Add stuff you want drawn on top all the time here **
	      
	      this.valid = true;
    }
}


// Creates an object with x and y defined, set to the mouse position relative to the state's canvas
// If you wanna be super-correct this can be tricky, we have to worry about padding and borders
CanvasState.prototype.getMouse = function(e) {
    var element = this.canvas, offsetX = 0, offsetY = 0, mx, my;
    
    // Compute the total offset
    if (element.offsetParent !== undefined) {
	      do {
	          offsetX += element.offsetLeft;
	          offsetY += element.offsetTop;
	      } while ((element = element.offsetParent));
    }

    // Add padding and border style widths to offset
    // Also add the <html> offsets in case there's a position:fixed bar
    offsetX += this.stylePaddingLeft + this.styleBorderLeft + this.htmlLeft;
    offsetY += this.stylePaddingTop + this.styleBorderTop + this.htmlTop;

    mx = e.pageX - offsetX;
    my = e.pageY - offsetY;
    
    // We return a simple javascript object (a hash) with x and y defined
    return {x: mx, y: my};
}

function Tag (tag_item, tag_name, tag_value) {

    this.tag_item = tag_item;
    this.tag_name = tag_name;
    this.tag_value = tag_value;

    this.tag_num = Math.floor(Math.random()*122231);

    this.show();

}

Tag.prototype.show = function() {

    if (this.tag_item == "station") {
	      
	      text = "Station Tag: " + this.tag_name + " - " + this.tag_value;
	      
	      kradwebsocket.debug(text);
	      
	      if (this.tag_name.indexOf("_bool") != -1) {

	          $('.kradradio_tools').append("<div class='button_wrap'><div class='krad_button_toggle' id='" + this.tag_name + "'>" + this.tag_name.replace("_bool","").replace("_", " ") + "</div>");
	          $('.kradlink_tools').append("<br clear='both'/>");  
	          
	          kradradio.add_bool(this.tag_name);
	          kradradio.got_set_bool(this.tag_name, this.tag_value);

	          $( '#' + this.tag_name).bind( "click", {bool: this.tag_name}, function(event, ui) {
		            kradradio.toggle_bool(event.data.bool);
	          });
	          
	          
	      } else {
	          $('.station_tags').append("<div id='ktag_" + this.tag_num + "' class='tag'>" + this.tag_item + ": " + this.tag_name + " - <span class='tag_value'>" + this.tag_value + "</span></div>");    
	      }

    } else {

	      if ((this.tag_name != "artist") && (this.tag_name != "title")) {

	          if (this.tag_name == "now_playing") {
		            $('#ktags_' + this.tag_item).append("<div id='ktag_" + this.tag_num + "' class='tag nowplaying'><span class='tag_value'>" + this.tag_value + "</span></div>");
		            return;
	          }
	          if (this.tag_name == "playtime") {
		            $('#ktags_' + this.tag_item).append("<div id='ktag_" + this.tag_num + "' class='tag playtime'><span class='tag_value'>" + this.tag_value + "</span></div>");
		            return;
	          }
	          
	          $('#ktags_' + this.tag_item).append("<div id='ktag_" + this.tag_num + "' class='tag'>" + this.tag_item + ": " + this.tag_name + " - <span class='tag_value'>" + this.tag_value + "</span></div>");          
	      }
    }
}

Tag.prototype.update_value = function(tag_value) {

    this.tag_value = tag_value;
    if (this.tag_name.indexOf("_bool") != -1) {  
	      kradradio.got_set_bool(this.tag_name, this.tag_value);
    }  
    if ((this.tag_name != "artist") && (this.tag_name != "title")) {
	      $('#ktag_' + this.tag_num + ' .tag_value').html(this.tag_value);
    }
}




function Kradradio () {

    this.sysname = "";
    this.sample_rate = 0;
    //this.real_interfaces = new Array();
    //this.ignoreupdate = false;
    //this.update_rate = 50;
    //this.timer;
    this.tags = new Array();
    
    this.decklink_devices = new Array();
    this.v4l2_devices = new Array();
    
    this.bools = new Object();  
    
    this.peaks = new Array();
    this.peaks_max = new Array();
    this.showing_snapshot_buttons = 0;
    this.system_cpu_usage = 0;
    this.admin = 0;
    this.kode = "";
    this.secretkode = "31337";
    this.portgroup_count = 0;
    this.xmms2_portgroup_count = 0;
    this.prepare_link_operation_mode = "";
    this.compositor_canvas = null;
    this.compositor_mode = "Position";
    this.sprites = new Array();

    // set up the selection handle boxes
    this.selectionHandles = [];

    for (var i = 0; i < 10; i ++) {
	      var rect = new Shape;
	      this.selectionHandles.push(rect);
    }


}

Kradradio.prototype.destroy = function () {


    $('#' + this.sysname).remove();

    //clearTimeout(this.timer);

    //for (real_interface in this.real_interfaces) {
    //  this.real_interfaces[real_interface].destroy();
    //}

}

Kradradio.prototype.add_bool = function (name) {
    this.bools[name] = 'false';
}

Kradradio.prototype.set_bool = function (name, value) {
    this.bools[name] = value.toString();
    if (this.bools[name] == 'false') {
	      $( '#' + name).removeClass("krad_button_toggle_on");    
    } else {
	      $( '#' + name).addClass("krad_button_toggle_on");    
    }
    this.set_tag(name, this.bools[name]);
}

Kradradio.prototype.got_set_bool = function (name, value) {
    this.bools[name] = value.toString();
    if (this.bools[name] == 'false') {
	      $( '#' + name).removeClass("krad_button_toggle_on");    
    } else {
	      $( '#' + name).addClass("krad_button_toggle_on");    
    }
}

Kradradio.prototype.get_bool = function (name) {
    return this.bools[name];
}

Kradradio.prototype.toggle_bool = function (name) {
    if (this.bools[name] == 'false') {
	      this.bools[name] = 'true';
	      $( '#' + name).addClass("krad_button_toggle_on");    
	      this.set_tag(name, this.bools[name]);
    } else {
	      if (this.bools[name] == 'true') {
	          this.bools[name] = 'false';
	      }
	      $( '#' + name).removeClass("krad_button_toggle_on");
	      this.set_tag(name, this.bools[name]);
    }
}

Kradradio.prototype.remove_bool = function (name) {
    this.bools[name] = 'false';
}

Kradradio.prototype.set_tag = function (name, value) {

    var cmd = {};  
    cmd.com = "kradradio";  
    cmd.cmd = "stag";
    cmd.tag_name = name;
    cmd.tag_value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.set_mixer_params = function (mixer_params) {
    this.sample_rate = mixer_params.sample_rate;
    $('.kradradio_mixer_info').append("<h2 class='sysinfo'>Sample Rate: " + this.sample_rate + " </h2>"); 
}

Kradradio.prototype.add_link = function () {

    var cmd = {};  
    cmd.com = "kradlink";  
    cmd.cmd = "add_link";

    cmd.operation_mode = this.prepare_link_operation_mode;
    cmd.av_mode = $('[name=prepare_link_avmode]:radio:checked').val();

    if (cmd.operation_mode == "transmit") {
	      cmd.host = $('[name=prepare_link_server_info_url]').val();
	      cmd.port = $('[name=prepare_link_server_info_port]').val();
	      cmd.mount = $('[name=prepare_link_server_info_mount]').val();
	      cmd.password = $('[name=prepare_link_server_info_password]').val();
    }

    if (cmd.operation_mode == "record") {
	      cmd.filename = $('[name=prepare_link_file_info_path]').val();
    }

    if ((cmd.operation_mode == "record") || (cmd.operation_mode == "transmit")) {
	      if ((cmd.av_mode == "audio_only") || (cmd.av_mode == "audio_and_video")) {
            cmd.audio_codec = $('[name=prepare_link_audio_codec]:radio:checked').val();  
	      }
	      if ((cmd.av_mode == "video_only") || (cmd.av_mode == "audio_and_video")) {
            cmd.video_codec = $('[name=prepare_link_video_codec]:radio:checked').val();
	      }
    }

    if (cmd.operation_mode == "capture") {
	      cmd.video_width = 1280;
	      cmd.video_height = 720;
	      cmd.fps_numerator = 60000;
	      cmd.fps_denominator = 1000;
	      cmd.video_source = "decklink";
	      cmd.video_device = $('[name=prepare_link_device]:radio:checked').val();
    }


    cmd.port = parseInt(cmd.port);
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}


Kradradio.prototype.add_kradlink_tools = function () {
    $('.kradlink_tools').append("<div class='kradlink_add_link'></div>");
    
    $('.kradlink_add_link').append("<div class='button_wrap'><div class='krad_button2' id='capture'>Capture</div>");  
    $('.kradlink_add_link').append("<div class='button_wrap'><div class='krad_button6' id='record'>Record</div>");
    $('.kradlink_add_link').append("<div class='button_wrap'><div class='krad_button7' id='transmit'>Transmit</div>");
    
    $('.kradlink_tools').append("<br clear='both'/>");  
    
    $('.kradlink_tools').append("<div class='kradlink_prepare_link'></div>");
    
    $('.kradlink_tools').append("<br clear='both'/>");  
    
    $( '#capture').bind( "click", function(event, ui) {
	      $('.kradlink_prepare_link').empty();  
	      $('.kradlink_prepare_link').append("<h3>Add Capture</h3>");
	      
	      if ((kradradio.decklink_devices.length == 0) && (kradradio.v4l2_devices.length == 0)) {
	          
	          $('.kradlink_prepare_link').append("<p>No V4L2 or Decklink devices detected.</p><br clear='both'/>");  
	          
	      } else {
	          kradradio.prepare_link_operation_mode = "capture";
	          
	          avmode = '<div id="prepare_link_avmode">\
<input type="radio" id="radio1900" name="prepare_link_avmode" value="audio_only"/><labelfor="radio1900">Audio</label>\
<input type="radio" id="radio2900" name="prepare_link_avmode" value="video_only"/><label for="radio2900">Video</label>\
<input type="radio" id="radio3900" name="prepare_link_avmode" value="audio_and_video"/><label for="radio3900">Audio and Video</label>\
</div>';
	          
	          audio_input = '<div id="prepare_link_audio_input">\
<input type="radio" id="radio431700" name="prepare_link_audio_input" value="opus"/><label for="radio431700">Embedded</label>\
<input type="radio" id="radio432700" name="prepare_link_audio_input" value="vorbis"/><label for="radio432700">AES/EBU</label>\
<input type="radio" id="radio433700" name="prepare_link_audio_input" value="flac"/><label for="radio433700">Analog</label>\
</div>';
	          
	          video_resolution = '<div id="prepare_link_video_resolution">\
<input type="radio" id="radio91600" name="prepare_link_video_resolution" value="320x240"/><label for="radio91600">320x240</label>\
<input type="radio" id="radio92600" name="prepare_link_video_resolution" value="640x480"/><label for="radio92600">640x480</label>\
<input type="radio" id="radio92601" name="prepare_link_video_resolution" value="480x270"/><label for="radio92601">480x270</label>\
<input type="radio" id="radio92611" name="prepare_link_video_resolution" value="640x360"/><label for="radio92611">640x360</label>\
<input type="radio" id="radio92602" name="prepare_link_video_resolution" value="960x540"/><label for="radio92602">960x540</label>\
<input type="radio" id="radio92603" name="prepare_link_video_resolution" value="1280x720"/><label for="radio92603">1280x720</label>\
<input type="radio" id="radio92644" name="prepare_link_video_resolution" value="1920x816"/><label for="radio92644">1920x816</label>\
<input type="radio" id="radio92604" name="prepare_link_video_resolution" value="1920x1080"/><label for="radio92604">1920x1080</label>\
<input type="radio" id="radio92655" name="prepare_link_video_resolution" value="2048x1556"/><label for="radio92655">2048x1556</label>\
<input type="radio" id="radio92654" name="prepare_link_video_resolution" value="3840x2160"/><label for="radio92654">3840x2160</label>\
<input type="radio" id="radio93654" name="prepare_link_video_resolution" value="custom"/><label for="radio93654">Custom</label>\
</div>';

	          video_framerate = '<div id="prepare_link_video_framerate">\
<input type="radio" id="radio191600" name="prepare_link_video_framerate" value="24000/1001"/><label for="radio191600">23.98 - 24000/1001</label>\
<input type="radio" id="radio192600" name="prepare_link_video_framerate" value="24000/1000"/><label for="radio192600">24 - 24000/1000</label>\
<input type="radio" id="radio192601" name="prepare_link_video_framerate" value="30000/1001"/><label for="radio192601">29.97 - 30000/1001</label>\
<input type="radio" id="radio192611" name="prepare_link_video_framerate" value="30000/1000"/><label for="radio192611">30 - 30000/1000</label>\
<input type="radio" id="radio192602" name="prepare_link_video_framerate" value="60000/1001"/><label for="radio192602">59.94 - 60000/1001</label>\
<input type="radio" id="radio192603" name="prepare_link_video_framerate" value="60000/1000"/><label for="radio192603">60 - 60000/1000</label>\
</div>';

	          $('.kradlink_prepare_link').append(audio_input);    
	          $('.kradlink_prepare_link').append("<br clear='both'/>");
	          $('.kradlink_prepare_link').append(avmode);    
	          $('.kradlink_prepare_link').append("<br clear='both'/>");
	          
	          devices = '<div id="prepare_link_device">';
            for (i = 0; i < kradradio.decklink_devices.length; i++) {
		            devices += '<input type="radio" id="radio1991' + i + '" name="prepare_link_device" value="' + i + '"/><label for="radio1991' + i + '">' + kradradio.decklink_devices[i] + '</label>';
            }
	          devices += '</div>';
            
	          $('.kradlink_prepare_link').append(devices);    
	          $('.kradlink_prepare_link').append("<br clear='both'/>");
            
	          $('.kradlink_prepare_link').append(video_resolution);    
	          $('.kradlink_prepare_link').append("<br clear='both'/>");

	          $('.kradlink_prepare_link').append(video_framerate);    
	          $('.kradlink_prepare_link').append("<br clear='both'/>");
            
	          $('#prepare_link_audio_input').buttonset();
	          $('#prepare_link_avmode').buttonset();
	          $('#prepare_link_device').buttonset();
	          $('#prepare_link_video_resolution').buttonset();
	          $('#prepare_link_video_framerate').buttonset();      

	          if (kradradio.admin) {
		            $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button7' id='add_link'>Begin</div>");
		            $( '#add_link').bind( "click", function(event, ui) {
		                kradradio.add_link();
		                $('.kradlink_prepare_link').empty();
		            });      
	          } else {
		            $('.kradlink_prepare_link').append("<p>Sorry your not an admin.. but you could be one day!</p><br clear='both'/>");
	          }

	          $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button3' id='cancel_add_link'>Cancel</div>");
	          $( '#cancel_add_link').bind( "click", function(event, ui) {
		            $('.kradlink_prepare_link').empty();
	          });
	          
	      }
	      
    });  

    $( '#record').bind( "click", function(event, ui) {
	      $('.kradlink_prepare_link').empty();  
	      $('.kradlink_prepare_link').append("<h3>Create Recording</h3>");
	      
	      kradradio.prepare_link_operation_mode = "record";    
	      
	      avmode = '<div id="prepare_link_avmode">\
<input type="radio" id="radio1900" name="prepare_link_avmode" value="audio_only"/><label for="radio1900">Audio</label>\
<input type="radio" id="radio2900" name="prepare_link_avmode" value="video_only"/><label for="radio2900">Video</label>\
<input type="radio" id="radio3900" name="prepare_link_avmode" value="audio_and_video"/><label for="radio3900">Audio and Video</label>\
</div>';
	      
	      container = '<div id="prepare_link_container">\
<input type="radio" id="radio1800" name="prepare_link_container" value="ogg"/><label for="radio1800">Ogg</label>\
<input type="radio" id="radio2800" name="prepare_link_container" value="mkv"/><label for="radio2800">MKV</label>\
<input type="radio" id="radio3800" name="prepare_link_container" value="webm"/><label for="radio3800">WebM</label>\
</div>';

	      video_codec = '<div id="prepare_link_video_codec">\
<input type="radio" id="radio1600" name="prepare_link_video_codec" value="theora"/><label for="radio1600">Theora</label>\
<input type="radio" id="radio2600" name="prepare_link_video_codec" value="vp8"/><label for="radio2600">VP8</label>\
</div>';
	      
	      audio_codec = '<div id="prepare_link_audio_codec">\
<input type="radio" id="radio1700" name="prepare_link_audio_codec" value="opus"/><label for="radio1700">Opus</label>\
<input type="radio" id="radio2700" name="prepare_link_audio_codec" value="vorbis"/><label for="radio2700">Vorbis</label>\
<input type="radio" id="radio3700" name="prepare_link_audio_codec" value="flac"/><label for="radio3700">FLAC</label>\
</div>';
	      
	      file_info = '<div id="prepare_link_file_info" class="prepare_link_file_info">\
<label for="radio1550">Filename</label><input style="width: 400px;" type="text" id="radio1550" name="prepare_link_file_info_path" value=""/>\
<br clear="both"/>\
</div>';

	      
	      $('.kradlink_prepare_link').append(container);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(avmode);    
	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(video_codec);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(audio_codec);

	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(file_info);
        
        
	      $('#prepare_link_avmode').buttonset();
	      $('#prepare_link_container').buttonset();
	      $('#prepare_link_audio_codec').buttonset();    
	      $('#prepare_link_video_codec').buttonset();
	      
	      $('.kradlink_prepare_link').append("<br clear='both'/>");
	      if (kradradio.admin) {
	          $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button7' id='add_link'>Start</div>");
	          $( '#add_link').bind( "click", function(event, ui) {
		            kradradio.add_link();
		            $('.kradlink_prepare_link').empty();
	          });        
	      } else {
	          $('.kradlink_prepare_link').append("<p>Sorry your not an admin.. but you could be one day!</p><br clear='both'/>");
	      }
	      $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button3' id='cancel_add_link'>Cancel</div>");
	      $( '#cancel_add_link').bind( "click", function(event, ui) {
	          $('.kradlink_prepare_link').empty();
	      });
    });  
    
    $( '#transmit').bind( "click", function(event, ui) {
	      $('.kradlink_prepare_link').empty();  
	      $('.kradlink_prepare_link').append("<h3>Add Transmission</h3>");
	      
	      kradradio.prepare_link_operation_mode = "transmit";    
	      
	      avmode = '<div id="prepare_link_avmode">\
<input type="radio" id="radio1900" name="prepare_link_avmode" value="audio_only"/><label for="radio1900">Audio</label>\
<input type="radio" id="radio2900" name="prepare_link_avmode" value="video_only"/><label for="radio2900">Video</label>\
<input type="radio" id="radio3900" name="prepare_link_avmode" value="audio_and_video"/><label for="radio3900">Audio and Video</label>\
</div>';
	      
	      container = '<div id="prepare_link_container">\
<input type="radio" id="radio1800" name="prepare_link_container" value="ogg"/><label for="radio1800">Ogg</label>\
<input type="radio" id="radio2800" name="prepare_link_container" value="mkv"/><label for="radio2800">MKV</label>\
<input type="radio" id="radio3800" name="prepare_link_container" value="webm"/><label for="radio3800">WebM</label>\
</div>';

	      video_codec = '<div id="prepare_link_video_codec">\
<input type="radio" id="radio1600" name="prepare_link_video_codec" value="theora"/><label for="radio1600">Theora</label>\
<input type="radio" id="radio2600" name="prepare_link_video_codec" value="vp8"/><label for="radio2600">VP8</label>\
</div>';
	      
	      video_resolution = '<div id="prepare_link_video_resolution">\
<input type="radio" id="radio91600" name="prepare_link_video_resolution" value="320x240"/><label for="radio91600">320x240</label>\
<input type="radio" id="radio92600" name="prepare_link_video_resolution" value="640x480"/><label for="radio92600">640x480</label>\
<input type="radio" id="radio92601" name="prepare_link_video_resolution" value="480x270"/><label for="radio92601">480x270</label>\
<input type="radio" id="radio92611" name="prepare_link_video_resolution" value="640x360"/><label for="radio92611">640x360</label>\
<input type="radio" id="radio92602" name="prepare_link_video_resolution" value="960x540"/><label for="radio92602">960x540</label>\
<input type="radio" id="radio92603" name="prepare_link_video_resolution" value="1280x720"/><label for="radio92603">1280x720</label>\
<input type="radio" id="radio92644" name="prepare_link_video_resolution" value="1920x816"/><label for="radio92644">1920x816</label>\
<input type="radio" id="radio92604" name="prepare_link_video_resolution" value="1920x1080"/><label for="radio92604">1920x1080</label>\
<input type="radio" id="radio92655" name="prepare_link_video_resolution" value="2048x1556"/><label for="radio92655">2048x1556</label>\
<input type="radio" id="radio92654" name="prepare_link_video_resolution" value="3840x2160"/><label for="radio92654">3840x2160</label>\
<input type="radio" id="radio93654" name="prepare_link_video_resolution" value="custom"/><label for="radio93654">Custom</label>\
</div>';
	      
	      audio_codec = '<div id="prepare_link_audio_codec">\
<input type="radio" id="radio1700" name="prepare_link_audio_codec" value="opus"/><label for="radio1700">Opus</label>\
<input type="radio" id="radio2700" name="prepare_link_audio_codec" value="vorbis"/><label for="radio2700">Vorbis</label>\
<input type="radio" id="radio3700" name="prepare_link_audio_codec" value="flac"/><label for="radio3700">FLAC</label>\
</div>';
	      
	      
	      server_info = '<div id="prepare_link_server_info" class="prepare_link_server_info">\
<label for="radio1500">Host</label><input style="width: 400px;" type="text" id="radio1500" name="prepare_link_server_info_url" value=""/>\
<br clear="both"/>\
<label for="radio1501">Port</label><input style="width: 60px;" type="text" id="radio1501" name="prepare_link_server_info_port" value=""/>\
<br clear="both"/>\
<label for="radio1502">Mount</label><input style="width: 190px;" type="text" id="radio1502" name="prepare_link_server_info_mount" value=""/>\
<br clear="both"/>\
<label for="radio2400">Password</label><input style="width: 120px;" type="password" id="radio2400" name="prepare_link_server_info_password" value=""/>\
</div>';
	      
	      $('.kradlink_prepare_link').append(container);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(avmode);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");
	      $('.kradlink_prepare_link').append(video_resolution);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");          
	      $('.kradlink_prepare_link').append(video_codec);
	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(audio_codec);

	      $('.kradlink_prepare_link').append("<br clear='both'/>");      
	      $('.kradlink_prepare_link').append(server_info);            
        
	      $('#prepare_link_avmode').buttonset();
	      $('#prepare_link_video_resolution').buttonset();    
	      $('#prepare_link_container').buttonset();
	      $('#prepare_link_audio_codec').buttonset();    
	      $('#prepare_link_video_codec').buttonset();
	      
	      $('.kradlink_prepare_link').append("<br clear='both'/>");    
	      if (kradradio.admin) {
	          $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button7' id='add_link'>Activate</div>");    
	          $( '#add_link').bind( "click", function(event, ui) {
		            kradradio.add_link();
		            $('.kradlink_prepare_link').empty();
	          });
	      } else {
	          $('.kradlink_prepare_link').append("<p>Sorry your not an admin.. but you could be one day!</p><br clear='both'/>");
	      }
	      $('.kradlink_prepare_link').append("<div class='button_wrap'><div class='krad_button3' id='cancel_add_link'>Cancel</div>");
	      $( '#cancel_add_link').bind( "click", function(event, ui) {
	          $('.kradlink_prepare_link').empty();
	      });
    });      

}

Kradradio.prototype.got_system_cpu_usage = function (system_cpu_usage) {
    this.system_cpu_usage = system_cpu_usage;
    $('#Station_Info').append("<h2 class='sysinfo'>CPU Usage: " 
    + this.system_cpu_usage + " %");
}

Kradradio.prototype.got_sysname = function (sysname) {

    this.sysname = sysname;

    $('#kradradio').append("<div class='kradradio_station' id='" + this.sysname + "'>\
<div class='kradradio_station_info' id='Station_Info'>\
<h1 class='info_title'>Station</h1>\
<h2 class='sysinfo'>Sysname: " + this.sysname + "</h2>\
</div>\
<div class='kradradio_mixer_info' id='Mixer_Info'>\
<h1 class='info_title'>Mixer</h1>\
<ul class='mixer_subunit_list' id='mixer_subunits'></ul>\
</div>\
<div class='kradradio_compositor_info'>\
<h1 class='info_title'>Compositor</h1>\
<br clear='both'>\
</div>\
<div class='kradradio_transponder_info'>\
<h1 class='info_title'>Transponder</h1>\
<br clear='both'>\
</div>\
</div>");


    
}

Kradradio.prototype.kradcompositor_controls = function () {
    $('.kradcompositor').append("<br clear='both'/>");
    
}

Kradradio.prototype.show_snapshot_buttons = function () {

    if (this.showing_snapshot_buttons == 0) {
	      
	      kradradio.jsnapshot();
	      
	      $('.kradcompositor').append("<br clear='both'/>");

	      $('.kradcompositor').append("<div class='button_wrap'><div class='krad_button3' id='snapshot'>Snapshot</div>");
	      
	      $('.kradcompositor').append("<div class='button_wrap'><div class='krad_button3' id='jsnapshot'>JPEG Snapshot</div>");  
	      
	      $('.kradcompositor').append("<div class='button_wrap'><div class='krad_button3' id='view_snapshot'><a target='_new' href='/snapshot'>View Last</a></div>");  
	      
	      $('.kradcompositor').append("<br clear='both'/>");
	      
	      $( '#snapshot').bind( "click", function(event, ui) {
	          kradradio.snapshot();
	      });
	      
	      $( '#jsnapshot').bind( "click", function(event, ui) {
	          kradradio.jsnapshot();
	      });
	      
	      this.showing_snapshot_buttons = 1;  
    }
}

Kradradio.prototype.got_tag = function (tag_item, tag_name, tag_value) {

    for (t in this.tags) {
	      if ((this.tags[t].tag_item == tag_item) && (this.tags[t].tag_name == tag_name)) {
	          this.tags[t].update_value (tag_value);
	          return;
	      }
    }
    
    tag = new Tag (tag_item, tag_name, tag_value);
    
    this.tags.push (tag);
    
}


Kradradio.prototype.portgroup_draw_peak_meter = function (portgroup_name, value) {

    canvas = document.getElementById(portgroup_name  + '_meter_canvas');
    if (!canvas) {
	      return;
    }
    
    
    ctx = canvas.getContext('2d');

    ctx.beginPath();

    ctx.fillStyle = '#11ff11'
    ctx.font="Ubuntu";
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';
    ctx.fillRect(0, 220, 42, -20);
    ctx.fillStyle = '#000000';
    ctx.fillText(this.peaks_max[portgroup_name].toString(),10,220);
    
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

Kradradio.prototype.got_peak_portgroup = function (portgroup_name, value) {

    //kradwebsocket.debug(portgroup_name);
    
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

Kradradio.prototype.crate_handler = function (crate) {
    if (crate.com == "kradmixer") {
	      if (crate.cmd == "peak_portgroup") {
	          kradradio.got_peak_portgroup (crate.portgroup_name, crate.value);
	          return;
	      }
	      if (crate.cmd == "control_portgroup") {
	          kradradio.got_control_portgroup (crate.portgroup_name, crate.control_name, crate.value);
	          return;
	      }
	      if (crate.cmd == "effect_control") {
	          kradradio.got_effect_control (crate);
	          return;
	      }
	      if (crate.cmd == "update_portgroup") {
	          kradradio.got_update_portgroup (crate.portgroup_name, crate.control_name, crate.value);
	          return;
	      }      
	      if (crate.cmd == "add_portgroup") {
	          kradradio.got_add_portgroup ( crate );
	          return;
	      }
	      if (crate.cmd == "remove_portgroup") {
	          kradradio.got_remove_portgroup (crate.portgroup_name);
	          return;
	      }
	      if (crate.cmd == "set_mixer_params") {
	          kradradio.set_mixer_params (crate);
	          return;
	      }
    }
    if (crate.com == "kradcompositor") {

        kradwebsocket.debug(crate);

	      if (crate.cmd == "add_subunit") {
	          kradradio.got_add_subunit ( crate );
	          return;
	      }  
        if (crate.cmd == "remove_subunit") {
            kradradio.got_remove_subunit (crate );
            return;
        }
	      if (crate.cmd == "update_subunit") {
	          kradradio.got_update_subunit (crate.subunit_id, crate.subunit_type, crate.control_name, crate.value);
	          return;
	      }
	      if (crate.cmd == "set_frame_rate") {
	          kradradio.got_frame_rate (crate.numerator, crate.denominator);
	          return;
	      }
	      if (crate.cmd == "set_frame_size") {
	          kradradio.got_frame_size (crate.width, crate.height);
	          return;
	      }      
    }    
    if (crate.com == "kradradio") {
	      if (crate.info == "cpu") {
	          kradradio.got_system_cpu_usage (crate.system_cpu_usage);
	          return;
	      }
	      if (crate.info == "sysname") {
	          kradradio.got_sysname (crate.infoval);
	          return;
	      }
	      if (crate.info == "tag") {
	          kradradio.got_tag (crate.tag_item, crate.tag_name, crate.tag_value);
	          return;
	      }
    }
    if (crate.com == "kradlink") {
	      if (crate.cmd == "update_link") {
	          kradradio.got_update_link (crate.link_num, crate.update_item, crate.update_value);
	          return;
	      }    
	      if (crate.cmd == "add_link") {
	          kradradio.got_add_link (crate);
	          return;
	      }
	      if (crate.cmd == "remove_link") {
	          kradradio.got_remove_link (crate);
	          return;
	      }     
	      if (crate.cmd == "add_decklink_device") {
	          kradradio.got_add_decklink_device (crate);
	          return;
	      }
    }
}

Kradradio.prototype.got_messages = function (msgs) {

    //kradwebsocket.debug(msgs);

    var msg_arr = JSON.parse (msgs);

    for (m in msg_arr) {
	      //kradwebsocket.debug(msg_arr[m]);  
	      this.crate_handler(msg_arr[m]);
    }
}

Kradradio.prototype.display = function () {

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "display";

    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);
}

Kradradio.prototype.addsprite = function (filename) {

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "add_subunit";
    cmd.subunit_type = "sprite";
    cmd.filename = filename;

    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);
}

Kradradio.prototype.setsprite = function (sprite_num, x, y) {

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "update_subunit";
    cmd.sprite_num = sprite_num.toString();
    cmd.control_name = "x";  
    cmd.value = x;  
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "update_subunit";
    cmd.sprite_num = sprite_num.toString();
    cmd.control_name = "y";  
    cmd.value = y;  
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.update_portgroup = function (portgroup_name, control_name, value) {

    var cmd = {};  
    cmd.com = "kradmixer";  
    cmd.cmd = "update_portgroup";
    cmd.portgroup_name = portgroup_name;  
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //  kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.update_subunit = function (num, type, control_name, value) {

    var millis = 50;
 
    var date = new Date();
    var curDate = null;
    do { curDate = new Date(); }
    while(curDate-date < millis);

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "update_subunit";
    cmd.subunit_type = type;
    cmd.subunit_id = num;  
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.debug(cmd);

    kradwebsocket.send(JSONcmd);

}


Kradradio.prototype.got_update_subunit = function (num, type, control_name, value) {
    
    for (var i = 0; i < kradradio.compositor_canvas.shapes.length; i++) {
	      if (kradradio.compositor_canvas.shapes[i].type == type && (kradradio.compositor_canvas.shapes[i].num == num)) {
	          kradradio.compositor_canvas.shapes[i].setValue(control_name, value);
	          kradradio.compositor_canvas.valid = false;
	          break;
	      }
    }
}


Kradradio.prototype.control_eq_effect = function (portgroup_name, effect_name, effect_num, control_name, value) {

    var cmd = {};  
    cmd.com = "kradmixer";  
    cmd.cmd = "control_eq_effect";
    cmd.portgroup_name = portgroup_name;
    cmd.effect_name = effect_name;
    cmd.effect_num = effect_num;
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.control_effect = function (portgroup_name, effect_name, control_name, value) {

    var cmd = {};  
    cmd.com = "kradmixer";  
    cmd.cmd = "control_effect";
    cmd.portgroup_name = portgroup_name;
    cmd.effect_name = effect_name;
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.jsnapshot = function () {

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "jsnap";
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.snapshot = function () {

    var cmd = {};  
    cmd.com = "kradcompositor";  
    cmd.cmd = "snap";
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}

Kradradio.prototype.not_admin = function () {

    alert("Sorry your not an admin right now! But you could be one day!");

}

Kradradio.prototype.enable_admin = function () {

    this.admin = 1;
    kradwebsocket.debug("admin enabled");
    

}

Kradradio.prototype.check_kode = function () {

    if (kradradio.kode == kradradio.secretkode) {
	      kradradio.enable_admin();
	      kradradio.kode = "";    
    }

    for (i = 0; i < kradradio.kode.length; i++) {
	      if (kradradio.kode.charAt(i) != kradradio.secretkode.charAt(i)) {
	          kradradio.kode = "";
	      }
    }

}

Kradradio.prototype.clear_max_peak = function (value) {
    this.peaks_max[value] = -1;
    this.portgroup_draw_peak_meter (value, -1);
}

Kradradio.prototype.push_dtmf = function (value) {

    var cmd = {};  
    cmd.com = "kradmixer";  
    cmd.cmd = "push_dtmf";
    cmd.dtmf = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send(JSONcmd);

    //kradwebsocket.debug(JSONcmd);
    
    kradradio.kode += value;

    kradradio.check_kode();
    
}

Kradradio.prototype.got_control_portgroup = function (portgroup_name, control_name, value) {

    //kradwebsocket.debug("control portgroup " + portgroup_name + " " + value);

    if ($('#' + portgroup_name)) {
	      if (control_name == "volume") {
	          $('#' + portgroup_name).slider( "value" , value );
	      } else {
	          $('#' + portgroup_name + '_crossfade').slider( "value" , value );
	      }
    }
}

Kradradio.prototype.got_effect_control = function (crate) {

    //kradwebsocket.debug(crate);

    if (crate.effect_name == "eq") {
	      if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.effect_num + '_' + crate.control_name)) {
	          $('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.effect_num + '_' + crate.control_name).slider( "value" , crate.value );
	      }
    } else {
	      if ($('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.control_name)) {
	          $('#' + crate.portgroup_name + '_' + crate.effect_name + '_' + crate.control_name).slider( "value" , crate.value );
	      }
    }
}

Kradradio.prototype.got_update_portgroup = function (portgroup_name, control_name, value) {

    //    kradwebsocket.debug("got_update_portgroup " + portgroup_name + " " + control_name + " " + value);

    if (control_name == "crossfade_group") {
	      kradradio.portgroup_handle_crossfade_group(portgroup_name, value, -100.0);
    }
    
    if (control_name == "xmms2_ipc_path") {
	      if (value.length > 0) {
	          kradradio.portgroup_handle_xmms2(portgroup_name, 1);
	      } else {
	          kradradio.portgroup_handle_xmms2(portgroup_name, 0);
	      }
    }
}

Kradradio.prototype.portgroup_handle_crossfade_group = function (portgroup_name, crossfade_name, crossfade) {

    if ($('#portgroup_' + portgroup_name + '_crossfade_wrap')) {
	      $('#portgroup_' + portgroup_name + '_crossfade_wrap').remove();
    }

    if (crossfade_name.length > 0) {
	      $('#portgroup_' + portgroup_name + '_wrap').after("<div id='portgroup_" + portgroup_name + "_crossfade_wrap' class='kradmixer_control crossfade_control'> <div id='" + portgroup_name + "_crossfade'></div> <h2>" + portgroup_name + " - " + crossfade_name + "</h2></div>");

	      $('#' + portgroup_name + '_crossfade').slider({orientation: 'horizontal', value: crossfade, min: -100, max: 100 });

	      $( '#' + portgroup_name + '_crossfade' ).bind( "slide", function(event, ui) {
	          kradradio.update_portgroup (portgroup_name, "crossfade", ui.value);
	      });
    }
}

Kradradio.prototype.portgroup_handle_xmms2 = function (portgroup_name, xmms2) {

    if (xmms2 == 0) {
	      if ($('#' + portgroup_name + '_xmms2_wrap')) {
	          $('#' + portgroup_name + '_xmms2_wrap').remove();
	      }
    } else {
	      xmms2_controls = "<div id='" + portgroup_name + "_xmms2_wrap' class='kradmixer_control xmms2_control'><div id='" + portgroup_name + "_xmms2'></div></div>";

	      if (this.xmms2_portgroup_count == 1) {
	          $('.kradmixer_beta').prepend(xmms2_controls);
	      } else {
	          $('.kradmixer_beta').append(xmms2_controls);
	      }

	      $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'><div class='krad_button_small' id='" + portgroup_name + "_xmms2_prev'>PREV</div></div>");
	      $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'><div class='krad_button_small' id='" + portgroup_name + "_xmms2_play'>PLAY</div></div>");
	      $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'><div class='krad_button_small' id='" + portgroup_name + "_xmms2_pause'>PAUSE</div></div>");
	      $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'><div class='krad_button_small' id='" + portgroup_name + "_xmms2_stop'>STOP</div></div>");
	      $('#' + portgroup_name + '_xmms2').append("<div class='button_wrap'><div class='krad_button_small' id='" + portgroup_name + "_xmms2_next'>NEXT</div></div>");        

	      $( '#' + portgroup_name + '_xmms2_prev' ).bind( "click", function(event, ui) {
	          kradradio.update_portgroup(portgroup_name, "xmms2", "prev");
	      });

	      $( '#' + portgroup_name + '_xmms2_play' ).bind( "click", function(event, ui) {
	          kradradio.update_portgroup(portgroup_name, "xmms2", "play");
	      });
	      
	      $( '#' + portgroup_name + '_xmms2_pause' ).bind( "click", function(event, ui) {
	          kradradio.update_portgroup(portgroup_name, "xmms2", "pause");
	      });
	      
	      $( '#' + portgroup_name + '_xmms2_stop' ).bind( "click", function(event, ui) {
	          kradradio.update_portgroup(portgroup_name, "xmms2", "stop");
	      });
	      
	      $( '#' + portgroup_name + '_xmms2_next' ).bind( "click", function(event, ui) {
	          kradradio.update_portgroup(portgroup_name, "xmms2", "next");
	      });
    }
}

Kradradio.prototype.got_add_subunit = function (crate) {
    
    kradwebsocket.debug(crate);

    if (! document.getElementById("compositor_canvas")) {
	      $('.kradcompositor').append("<div class='kradcompositor_control'><canvas width='960px' height='540px' id='compositor_canvas'></canvas></div><br clear='both'><div width='960px'class='kradcompositor_subunit_list'><ul id='subunit_list'></ul></div>");
	             
	      kradradio.compositor_canvas = new CanvasState(document.getElementById("compositor_canvas"), document.createElement('canvas'));
    }
    
    //function Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {

    if ((crate.subunit_type == 'videoport') && (crate.direction == 'output')) {
        return;
    }
    //Shape(x, y, w, h, r, o, xs, ys, type, num, fill) {
    var shape = new Shape(crate.x,crate.y, crate.width, crate.height, crate.r * Math.PI / 180.0, crate.o, crate.xscale, crate.yscale,
                          crate.subunit_type, crate.subunit_id);
    
    kradradio.compositor_canvas.addShape(shape);
    if (crate.subunit_type == 'videoport') {
        shape.PCView.handles[0].x = crate.view_top_left_x;
        shape.PCView.handles[0].y = crate.view_top_left_y;
        shape.PCView.handles[1].x = crate.view_top_right_x;
        shape.PCView.handles[1].y = crate.view_top_right_y;
        shape.PCView.handles[2].x = crate.view_bottom_left_x;
        shape.PCView.handles[2].y = crate.view_bottom_left_y;
        shape.PCView.handles[3].x = crate.view_bottom_right_x;
        shape.PCView.handles[3].y = crate.view_bottom_right_y;
    }
    
    $('#subunit_list').append('<li class=subunit id=' + shape.type + '_' + shape.num + '>' + shape.type + '_' + shape.num + '</li>');
    
    $('li').removeClass("alt");
    $('li:even').addClass("alt");
    
    $('#' + shape.type + '_' + shape.num).bind("click", function(e,ui) {
	      e.preventDefault();
        
	      var alreadySelected = $(this).hasClass('selected');
	      var shapes = kradradio.compositor_canvas.shapes;
	      var l = shapes.length;
        
	      //run removeClass on every element
	      $('.subunit').removeClass('selected');
	      for (var i = 0; i < l;  i++) {
	          shapes[i].selected = false;
	          
	          if (shapes[i].num == shape.num && shapes[i].type == shape.type) {
		            shapes[i].selected = !alreadySelected;
		            if (!alreadySelected) {
		                kradradio.compositor_canvas.selection = shapes[i];		    
		            }
	          }
	      }
	      kradradio.compositor_canvas.valid = false;
        
	      //toggle class on the currently clicked element (this)
	      if (!alreadySelected) {
	          $(this).addClass('selected');
	      }
    });                                              
}



Kradradio.prototype.got_remove_subunit = function (crate) {
    
    //    kradwebsocket.debug(crate);
    
    kradradio.compositor_canvas.removeShape(crate.subunit_type, crate.subunit_id);
    //var altVar = kradradio.compositor_canvas.even;
    //kradradio.compositor_canvas.even = ! altVar;

    $('#' + crate.subunit_type + '_' + crate.subunit_id).remove();
   
    $('li').removeClass("alt");
    $('li:even').addClass("alt");
    
   
    // if (altVar) {
	  //     $('#'  + shape.type + '_' + shape.num).addClass("alt");
    // }
    
    // $('#' + shape.type + '_' + shape.num).bind("click", function(e,ui) {
	  //e.preventDefault();

	  //var alreadySelected = $(this).hasClass('selected');
	  //var shapes = kradradio.compositor_canvas.shapes;
	  //var l = shapes.length;


	  //run removeClass on every element
	  //$('.subunit').removeClass('selected');
	  //for (var i = 0; i < l;  i++) {
	  //    shapes[i].selected = false;
	  
	  //    if (shapes[i].num == shape.num && shapes[i].type == shape.type) {
	  //	shapes[i].selected = !alreadySelected;
	  //	if (!alreadySelected) {
	  //	    kradradio.compositor_canvas.selection = shapes[i];		    
	  //	}
	  //   }
	  //}
	  kradradio.compositor_canvas.valid = false;

	  //toggle class on the currently clicked element (this)
	  //if (!alreadySelected) {
	  //    $(this).addClass('selected');
    //
    //	}
}


Kradradio.prototype.got_add_portgroup = function (crate) {

    //kradwebsocket.debug(crate);

    this.peaks[crate.portgroup_name] = -1;
    this.peaks_max[crate.portgroup_name] = -1;
    this.portgroup_count += 1;

   
//*[@id="mixer_subunits"]
    $('#mixer_subunits').append("<li id='" + crate.portgroup_name + "_li'>Portgroup: " + crate.portgroup_name + "</li>");
     
    if (crate.xmms2 == 1) {
	      this.xmms2_portgroup_count += 1;
	      $('#' + crate.portgroup_name + '_li').append("<br>Has xmms2");
    }
 
    if (crate.crossfade_name.length > 0) {
        $('#mixer_subunits').append("<li>Crossfade: " + crate.portgroup_name + "  " + crate.crossfade_name + "</li>");
	      
    }
    
      
    $('li').removeClass("alt");
    $('li:even').addClass("alt");
    if (crate.xmms2 == 1) {
	      kradradio.portgroup_handle_xmms2(crate.portgroup_name, crate.xmms2);
    }

    $('#portgroup_' + crate.portgroup_name + '_wrap').append("<div id='ktags_" + crate.portgroup_name + "'></div>");

    if (crate.direction == 1) {
	      $('.kradmixer_cappa').append("<div class='effect_area'> <div id='" + crate.portgroup_name + "_effects'><h2>Effects for " + crate.portgroup_name + "</h2></div> </div>");

	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_lowpass_hz'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_lowpass_bw'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_highpass_hz'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_highpass_bw'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_analog_drive'></div>");
	      $('#' + crate.portgroup_name + '_effects').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_analog_blend'></div>");
	      
	      $('#' + crate.portgroup_name + '_lowpass_hz').slider({orientation: 'vertical', value: crate.lowpass_hz, step: 1, min: 20.0, max: 12000.0 });
	      $('#' + crate.portgroup_name + '_lowpass_bw').slider({orientation: 'vertical', value: crate.lowpass_bw, step: 0.1, min: 0.1, max: 5.0 });
	      $('#' + crate.portgroup_name + '_highpass_hz').slider({orientation: 'vertical', value: crate.highpass_hz, step: 1, min: 20.0, max: 20000.0 });
	      $('#' + crate.portgroup_name + '_highpass_bw').slider({orientation: 'vertical', value: crate.highpass_bw, step: 0.1, min: 0.1, max: 5.0 });
	      $('#' + crate.portgroup_name + '_analog_drive').slider({orientation: 'vertical', value: crate.analog_drive, step: 0.1, min: 0.1, max: 10.0 });
	      $('#' + crate.portgroup_name + '_analog_blend').slider({orientation: 'vertical', value: crate.analog_blend, step: 0.1, min: -10.0, max: 10.0 });
	      
	      $( '#' + crate.portgroup_name + '_lowpass_hz' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "lp", "hz", ui.value);
	      });

	      $( '#' + crate.portgroup_name + '_lowpass_bw' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "lp", "bw", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_highpass_hz' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "hp", "hz", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_highpass_bw' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "hp", "bw", ui.value);
	      });

	      $( '#' + crate.portgroup_name + '_analog_drive' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "a", "drive", ui.value);
	      });
	      
	      $( '#' + crate.portgroup_name + '_analog_blend' ).bind( "slide", function(event, ui) {
	          kradradio.control_effect (crate.portgroup_name, "a", "blend", ui.value);
	      });

	      $('.kradmixer_cappa').append("<br clear='both'/><div class='effect_area'> <div id='" + crate.portgroup_name + "_effects_eq'></div> </div>");

	      //for (band in crate.eq.bands) {
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_hz'></div>");
	          
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
	          
            
	          $('#' + crate.portgroup_name + '_eq_' + i + '_hz').slider({orientation: 'vertical', value: crate.eq.bands[i].hz, step: 1, min: mini, max: maxi });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_hz' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "hz", ui.value);
	          });
	      }
	      
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_db'></div>");
	          $('#' + crate.portgroup_name + '_eq_' + i + '_db').slider({orientation: 'vertical', value: crate.eq.bands[i].db, step: 0.1, min: -12.0, max: 12.0 });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_db' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "db", ui.value);
	          });
	      }
	      
	      for (var i = 0; i < 32; i++) {
	          //kradwebsocket.debug(crate.eq.bands[i]);  
	          $('#' + crate.portgroup_name + '_effects_eq').append("<div class='kradmixer_control' id='" + crate.portgroup_name + "_eq_" + i + "_bw'></div>");
	          $('#' + crate.portgroup_name + '_eq_' + i + '_bw').slider({orientation: 'vertical', value: crate.eq.bands[i].bw, step: 0.1, min: 0.1, max: 5.0 });
	          $( '#' + crate.portgroup_name + '_eq_' + i + '_bw' ).bind( "slide", {band: i}, function(event, ui) {
		            kradradio.control_eq_effect (crate.portgroup_name, "eq", event.data.band, "bw", ui.value);
	          });
	      }
	      
	      
    }

    this.portgroup_draw_peak_meter (crate.portgroup_name, 0);

    if ($.browser.webkit) {
	      $('.krad_button_small').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(151,151,151,1) 12%,rgba(171,171,171,1) 100%)');
	      $('.krad_button_small:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 0%,rgba(254,182,69,1) 100%)');
	      $('.krad_button').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 12%,rgba(254,182,69,1) 100%)');
	      $('.krad_button:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(241,231,103,1) 12%,rgba(254,182,69,1) 100%)');
	      $('.krad_button2').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(83,203,241,1) 0%,rgba(5,171,224,1) 100%)');
	      $('.krad_button2:active').css('background', '-webkit-radial-gradient(center, ellipse cover, rgba(83,203,241,1) 0%,rgba(5,171,224,1) 100%)');
    }

}

Kradradio.prototype.got_remove_portgroup = function (name) {

    $('#portgroup_' + name + '_wrap').remove();

}

Kradradio.prototype.remove_link = function (link_num) {

    if (!kradradio.admin) {
	      kradradio.not_admin();
	      return;
    }

    var cmd = {};  
    cmd.com = "kradlink";  
    cmd.cmd = "remove_link";
    cmd.link_num = link_num;  
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send (JSONcmd);

    //kradwebsocket.debug(JSONcmd);
    
    $('#link_' + link_num + '_wrap').remove();
    
}

Kradradio.prototype.got_update_link = function (link_num, control_name, value) {

    kradwebsocket.debug("got_update_link");

    kradwebsocket.debug(link_num);
    kradwebsocket.debug(control_name);
    kradwebsocket.debug(value);    

    if ($('#link_' + link_num + '_wrap')) {
	      if ((control_name == "opus_frame_size") || (control_name == "opus_signal") || (control_name == "opus_bandwidth")) {
	          
	          control_radio = "link_" + link_num + "_" + control_name;
	          
	          $('[name=' + control_radio + ']').prop('checked',false);
	          $('[name=' + control_radio + '][value="' + value + '"]').prop('checked',true);
	          
	          $('#link_' + link_num + '_' + control_name + '_setting').buttonset( "refresh" );
            
	      } else {
	          $('#link_' + link_num + '_' + control_name + '_value').html(value);              
	          $('#link_' + link_num + '_' + control_name + '_slider').slider( "value" , value );
	      }
    }

}

Kradradio.prototype.update_link = function (link_num, control_name, value) {

    var cmd = {};  
    cmd.com = "kradlink";  
    cmd.cmd = "update_link";
    cmd.link_num = link_num;  
    cmd.control_name = control_name;  
    cmd.value = value;
    
    var JSONcmd = JSON.stringify(cmd); 

    kradwebsocket.send (JSONcmd);

    //kradwebsocket.debug(JSONcmd);
}


Kradradio.prototype.got_add_decklink_device = function (decklink_device) {


    this.decklink_devices[decklink_device.decklink_device_num] = decklink_device.decklink_device_name;

    //$('.kradlink').append("<div class='kradlink_link'>" + this.decklink_devices[decklink_device.decklink_device_num] + "</div>");


}


Kradradio.prototype.got_remove_link = function (link) {

    $('#link_' + link.link_num + '_wrap').remove();

}


Kradradio.prototype.got_add_link = function (link) {

    if (link.host == "transmitter") {
	      link.host = window.document.location.hostname;
    }

    if (link.operation_mode == "capture") {
	      this.show_snapshot_buttons();
    }

    $('.kradlink').append("<div class='kradlink_link' id='link_" + link.link_num + "_wrap'> <div id='link_" + link.link_num + "'></div></div>");

    $('#link_' + link.link_num).append("<h3>" + link.operation_mode.replace("capture","capturing").replace("transmit","transmitting").replace("record","recording") + " " + link.av_mode.replace(" only","") + "</h3>");

    $('#link_' + link.link_num).append("<div id='ktags_link" + link.link_num + "'></div>");

    if (link.operation_mode == "capture") {
	      $('#link_' + link.link_num).append("<h5>Device: " + link.video_source + "</h5>");
    }

    if (link.operation_mode == "transmit") {

	      if ((link.av_mode == "video only") || (link.av_mode == "audio and video")) {
	          $('#link_' + link.link_num).append("<h5>Video Codec: " + link.video_codec + " Resolution: " + link.video_width + "x" + link.video_height + " Frame Rate: " + link.video_fps_numerator + " / " + link.video_fps_denominator + " Color Depth: " + link.video_color_depth + "</h5>");
	      }
	      if ((link.av_mode == "audio only") || (link.av_mode == "audio and video")) {
	          $('#link_' + link.link_num).append("<h5>Audio Codec: " + link.audio_codec + " Channels: " + link.audio_channels + " Sample Rate: " + link.audio_sample_rate + "</h5>");
	      }

	      if ((link.av_mode == "audio only") || (link.av_mode == "audio and video")) {
	          
	          if (link.audio_codec == "FLAC") {
		            $('#link_' + link.link_num).append("<h5>FLAC Bit Depth: " + link.flac_bit_depth + "</h5>");
	          }    
	          
	          if (link.audio_codec == "Vorbis") {
		            $('#link_' + link.link_num).append("<h5>Vorbis Quality: " + link.vorbis_quality + "</h5>");
	          }
	      }

	      $('#link_' + link.link_num).append("<h5><a href='http://" + link.host + ":" + link.port + link.mount + "'>" + link.host + ":" + link.port + link.mount + "</a></h5>");
	      
	      if (link.av_mode == "audio only") {
	          $('#link_' + link.link_num).append("<audio controls preload='none' src='http://" + link.host + ":" + link.port + link.mount + "'>Audio tag should be here.</audio>");    
	      }

	      if ((link.av_mode == "video only") || (link.av_mode == "audio and video")) {
	          $('#link_' + link.link_num).append("<video controls poster='/snapshot' preload='none' width='480' height='270' src='http://" + link.host + ":" + link.port + link.mount + "'>Video tag should be here.</video>");    
	      }

	      $('#link_' + link.link_num).append("<br clear='both'/><br clear='both'/>");

	      if ((link.av_mode == "video only") || (link.av_mode == "audio and video")) {
	          
	          if (link.video_codec == "Theora") {

		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_theora_quality_slider'></div><h2>Quality&nbsp;<span id='link_" + link.link_num + "_theora_quality_value'>" + link.theora_quality + "</span></h2></div>");

		            $('#link_' + link.link_num + '_theora_quality_slider').slider({orientation: 'vertical', value: link.theora_quality, min: 0, max: 63 });

		            $( '#link_' + link.link_num + '_theora_quality_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_theora_quality_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "theora_quality", ui.value);
		            });

	          }

	          if (link.video_codec == "VP8") {
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_vp8_bitrate_slider'></div><h2>Bitrate&nbsp;<span id='link_" + link.link_num + "_vp8_bitrate_value'>" + link.vp8_bitrate + "</span></h2></div>");

		            $('#link_' + link.link_num + '_vp8_bitrate_slider').slider({orientation: 'vertical', value: link.vp8_bitrate, min: 100, max: 10000 });

		            $( '#link_' + link.link_num + '_vp8_bitrate_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_vp8_bitrate_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "vp8_bitrate", ui.value);
		            });
		            
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_vp8_min_quantizer_slider'></div><h2>Quantizer&nbsp;Min&nbsp;<span id='link_" + link.link_num + "_vp8_min_quantizer_value'>" + link.vp8_min_quantizer + "</span></h2></div>");

		            $('#link_' + link.link_num + '_vp8_min_quantizer_slider').slider({orientation: 'vertical', value: link.vp8_min_quantizer, min: 0, max: 33 });

		            $( '#link_' + link.link_num + '_vp8_min_quantizer_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_vp8_min_quantizer_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "vp8_min_quantizer", ui.value);
		            });
		            
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_vp8_max_quantizer_slider'></div><h2>Quantizer&nbsp;Max&nbsp;<span id='link_" + link.link_num + "_vp8_max_quantizer_value'>" + link.vp8_max_quantizer + "</span></h2></div>");

		            $('#link_' + link.link_num + '_vp8_max_quantizer_slider').slider({orientation: 'vertical', value: link.vp8_max_quantizer, min: 34, max: 63 });

		            $( '#link_' + link.link_num + '_vp8_max_quantizer_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_vp8_max_quantizer_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "vp8_max_quantizer", ui.value);
		            });
		            
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_vp8_deadline_slider'></div><h2>Deadline&nbsp;<span id='link_" + link.link_num + "_vp8_deadline_value'>" + link.vp8_deadline + "</span></h2></div>");

		            $('#link_' + link.link_num + '_vp8_deadline_slider').slider({orientation: 'vertical', value: link.vp8_deadline, min: 1, max: 33000 });

		            $( '#link_' + link.link_num + '_vp8_deadline_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_vp8_deadline_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "vp8_deadline", ui.value);
		            });    
		            
	          }
	      }  
	      
	      if ((link.av_mode == "audio only") || (link.av_mode == "audio and video")) {
	          
	          if (link.audio_codec == "Opus") {
		            
		            
		            $('#link_' + link.link_num).append("<h5>Opus frame size: </h5>");

		            frame_size_controls = '<div id="link_' + link.link_num + '_opus_frame_size_setting">\
<input type="radio" id="radio1" name="link_' + link.link_num + '_opus_frame_size" value="120"/><label for="radio1">120</label>\
<input type="radio" id="radio2" name="link_' + link.link_num + '_opus_frame_size" value="240"/><label for="radio2">240</label>\
<input type="radio" id="radio3" name="link_' + link.link_num + '_opus_frame_size" value="480"/><label for="radio3">480</label>\
<input type="radio" id="radio4" name="link_' + link.link_num + '_opus_frame_size" value="960"/><label for="radio4">960</label>\
<input type="radio" id="radio5" name="link_' + link.link_num + '_opus_frame_size" value="1920"/><label for="radio5">1920</label>\
<input type="radio" id="radio6" name="link_' + link.link_num + '_opus_frame_size" value="2880"/><label for="radio6">2880</label>\
</div>';
		            
		            $('#link_' + link.link_num).append(frame_size_controls);

		            $("input[name=link_" + link.link_num + "_opus_frame_size][value=" + link.opus_frame_size + "]").attr('checked', 'checked');

		            $('#link_' + link.link_num + '_opus_frame_size_setting').buttonset();      

		            $( "input[name=link_" + link.link_num + "_opus_frame_size]" ).bind( "change", function(event, ui) {
		                var valu = $('input[name=link_' + link.link_num + '_opus_frame_size]:checked').val();
		                kradradio.update_link (link.link_num, "opus_frame_size", parseInt (valu));
		            });

		            signal_controls = '<h5>Opus Signal Type: </h5><div id="link_' + link.link_num + '_opus_signal_setting">\
<input type="radio" id="radio7" name="link_' + link.link_num + '_opus_signal" value="OPUS_AUTO"/><label for="radio7">Auto</label>\
<input type="radio" id="radio8" name="link_' + link.link_num + '_opus_signal" value="OPUS_SIGNAL_VOICE"/><label for="radio8">Voice</label>\
<input type="radio" id="radio9" name="link_' + link.link_num + '_opus_signal" value="OPUS_SIGNAL_MUSIC"/><label for="radio9">Music</label>\
</div>';
		            
		            $('#link_' + link.link_num).append(signal_controls);

		            $("input[name=link_" + link.link_num + "_opus_signal][value=" + link.opus_signal + "]").attr('checked', 'checked');
		            
		            $('#link_' + link.link_num + '_opus_signal_setting').buttonset();

		            $( "input[name=link_" + link.link_num + "_opus_signal]" ).bind( "change", function(event, ui) {
		                var valus = $('input[name=link_' + link.link_num + '_opus_signal]:checked').val();
		                kradradio.update_link (link.link_num, "opus_signal", valus);
		            });  
		            
		            
		            bandwidth_controls = '<h5>Opus Audio Bandwidth: </h5><div id="link_' + link.link_num + '_opus_bandwidth_setting">\
<input type="radio" id="radio10" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_AUTO"/><label for="radio10">Auto</label>\
<input type="radio" id="radio11" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_BANDWIDTH_NARROWBAND"/><label for="radio11">Narrowband</label>\
<input type="radio" id="radio12" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_BANDWIDTH_MEDIUMBAND"/><label for="radio12">Mediumband</label>\
<input type="radio" id="radio13" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_BANDWIDTH_WIDEBAND"/><label for="radio13">Wideband</label>\
<input type="radio" id="radio14" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_BANDWIDTH_SUPERWIDEBAND"/><label for="radio14">Super Wideband</label>\
<input type="radio" id="radio15" name="link_' + link.link_num + '_opus_bandwidth" value="OPUS_BANDWIDTH_FULLBAND"/><label for="radio15">Fullband</label>\
</div>';
		            
		            $('#link_' + link.link_num).append(bandwidth_controls);

		            $("input[name=link_" + link.link_num + "_opus_bandwidth][value=" + link.opus_bandwidth + "]").attr('checked', 'checked');
		            
		            $('#link_' + link.link_num + '_opus_bandwidth_setting').buttonset();

		            $( "input[name=link_" + link.link_num + "_opus_bandwidth]" ).bind( "change", function(event, ui) {
		                var valub = $('input[name=link_' + link.link_num + '_opus_bandwidth]:checked').val();
		                kradradio.update_link (link.link_num, "opus_bandwidth", valub);
		            });
		            
		            $('#link_' + link.link_num).append("<br clear='both'/>");      
		            
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_opus_bitrate_slider'></div><h2>Bitrate&nbsp;<span id='link_" + link.link_num + "_opus_bitrate_value'>" + link.opus_bitrate + "</span></h2></div>");
		            $('#link_' + link.link_num).append("<div class='kradlink_control kradlink_param_control'><div id='link_" + link.link_num + "_opus_complexity_slider'></div><h2>Complexity&nbsp;<span id='link_" + link.link_num + "_opus_complexity_value'>" + link.opus_complexity + "</span></h2></div>");


		            $('#link_' + link.link_num + '_opus_bitrate_slider').slider({orientation: 'vertical', value: link.opus_bitrate, min: 3000, max: 320000 });

		            $( '#link_' + link.link_num + '_opus_bitrate_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_opus_bitrate_value' ).html(ui.value);      
		                kradradio.update_link (link.link_num, "opus_bitrate", ui.value);
		            });    

		            $('#link_' + link.link_num + '_opus_complexity_slider').slider({orientation: 'vertical', value: link.opus_complexity, min: 0, max: 10 });

		            $( '#link_' + link.link_num + '_opus_complexity_slider' ).bind( "slide", function(event, ui) {
		                $( '#link_' + link.link_num + '_opus_complexity_value' ).html(ui.value);
		                kradradio.update_link (link.link_num, "opus_complexity", ui.value);
		            });
	          }
	      }
    }
    
    $('#link_' + link.link_num).append("<br clear='both'/>");

    $('#link_' + link.link_num).append("<div class='button_wrap'><div class='krad_button6' id='" + link.link_num + "_remove'>Remove</div>");

    $( '#' + link.link_num + '_remove').bind( "click", function(event, ui) {
	      kradradio.remove_link(link.link_num);
    });
    
    $('#link_' + link.link_num).append("<br clear='both'/><br clear='both'/>");  

}



function Kradwebsocket (port) {

    this.port = port;
    this.uri = 'ws://' + location.hostname + ':' + this.port + '/';
    this.websocket = "";
    this.reconnection_attempts = 0;
    this.reconnect = false;  
    this.connected = false;
    this.connecting = false;
    this.stay_connected = true;  
    this.debug ("Created");
    this.connect();

}

Kradwebsocket.prototype.do_reconnect = function() {

    if (this.connected != true) {
	      this.reconnection_attempts += 1;
	      if (this.reconnection_attempts == 3) {
	          $('#kradradio').append("<div id='websockets_connection_problem'><h2>Websockets connection problem using port " + this.port + "</h2></div>");
	      }
	      this.connect();
    }

}

Kradwebsocket.prototype.connect = function() {

    this.stay_connected = true;
    if (this.connected != true) {
	      if (this.connecting != true) {
	          this.connecting = true;
	          this.debug ("Connecting..");
	          this.websocket = new WebSocket (this.uri, "krad-ws-api");
	          this.websocket.onopen = create_handler (this, this.on_open);
	          this.websocket.onclose = create_handler (this, this.on_close);
	          this.websocket.onmessage = create_handler (this, this.on_message);
	          this.websocket.onerror = create_handler (this, this.on_error);
	      } else {
	          this.debug("Tried to connect but in the process of connecting."); 
	      }
    } else {
	      this.debug("Tried to connect when already connected."); 
    }
}

Kradwebsocket.prototype.disconnect = function() {
    this.connected = false;
    this.connecting = false;
    this.stay_connected = false;  
    this.debug ("Disconnecting..");
    if (this.reconnect != false) {
	      window.clearInterval(this.reconnect);
	      this.reconnect = false;
    }
    this.websocket.close ();
    //this.websocket.onopen = false;
    //this.websocket.onclose = false;
    //this.websocket.onmessage = false;
    //this.websocket.onerror = false;
}

Kradwebsocket.prototype.on_open = function(evt) {
    this.connected = true;
    this.debug ("Connected!");
    this.connecting = false;
    this.reconnection_attempts = 0;

    if (this.reconnect != false) {
	      window.clearInterval(this.reconnect);
	      this.reconnect = false;
    }
    if ($('#websockets_connection_problem')) {
	      $('#websockets_connection_problem').remove();
    }

    kradradio = new Kradradio ();  
}

Kradwebsocket.prototype.on_close = function(evt) {
    this.connected = false;
    this.debug ("Disconnected!");
    if (kradradio != false) {
	      kradradio.destroy ();
	      kradradio = false;
    }
    this.connecting = false;
    if (this.stay_connected == true) {
	      if (this.reconnect == false) {
	          this.reconnect = setInterval(create_handler(this, this.do_reconnect), 1000);
	      }
    }
}

Kradwebsocket.prototype.send = function(message) {
    if (this.connected == true) {
	      this.websocket.send (message);
	      //this.debug ("Sent " + message); 
    } else {
	      this.debug ("NOT CONNECTED, wanted to send: " + message); 
    }
}

Kradwebsocket.prototype.on_error = function(evt) {
    this.debug ("Error! " + evt.data);

}

Kradwebsocket.prototype.debug = function(message) {
    console.log (message);
}

Kradwebsocket.prototype.on_message = function(evt) {
//    this.debug ("got message: " + evt.data);

    kradradio.got_messages (evt.data);  
}
