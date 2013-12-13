$(document).ready(function() {
  rack_units.push({'constructor': Clock, 'aspect': [4,1]});
});

function Clock(info_object) {
  this.title = "Clock";
  this.description = "Digital clock Rack unit";
  this.aspect_ratio = [4,2];
  this.x = 28;
  this.y = 1;
  this.width = 0;
  this.height = 0;
  this.address_masks = [];
  this.id = 'clock';
  this.div_text = "<div width='100%' height='100%' class='RCU clock' id='" + this.id + "'></div>";
  this.sel = "#" + this.id;
  info_object['parent_div'].append(this.div_text);
  this.updateTime();
  var clock = this;
  this.resizeTimer;
/*
  $(window).load(function() {
    setTimeout(page_resize, 200);
  });*/
}

Clock.prototype.page_resize = function() {
  $('.clock').css('font-size',0.8*$('#clock').height()+'px');
}

Clock.prototype.window_resize = function() {
  clearTimeout(this.resizeTimer);
  this.resizeTimer = setTimeout(this.page_resize, 100);
}

Clock.prototype.update = function(crate) {
}

Clock.prototype.shared = function(key, shared_object) {
}

Clock.prototype.updateTime = function() {

  var currentTime = new Date();
  var currentHours = currentTime.getHours ( );
  var currentMinutes = currentTime.getMinutes ( );
  var currentSeconds = currentTime.getSeconds ( );

  /*Pad leading zeros, if required.*/
  currentHours = ( currentHours < 10 ? "0" : "" ) + currentHours;
  currentMinutes = ( currentMinutes < 10 ? "0" : "" ) + currentMinutes;
  currentSeconds = ( currentSeconds < 10 ? "0" : "" ) + currentSeconds;
  var currentTimeString = currentHours + ":" + currentMinutes
    + ":" + currentSeconds;
  var clock = this;
  $(this.sel).html(currentTimeString);
  setTimeout(function(){clock.updateTime()},500);
}


