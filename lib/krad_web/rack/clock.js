$(document).ready(function() {
  rack_units.push({'constructor': Clock, 'aspect': [4,1]});
});
 
function Clock(info_object) {
  arguments.callee.count = ++arguments.callee.count || 1
    this.title = "Comper List";
  this.description = "List of composited duders, dudes.";
  this.aspect_ratio = [4,2];
  this.x = 28;
  this.y = 0;
  this.width = 0;
  this.height = 0;
  this.address_masks = [];
  this.id = 'clock_' + arguments.callee.count;
  this.div_text = "<div width='100%' height='100%' class='RCU clock' id='" + this.id + "'></div>";
  this.sel = "#" + this.id;
  info_object['parent_div'].append(this.div_text);
  this.updateTime();
  var clock = this;
  $(window).resize(function(){
    $(clock.sel).css('font-size',0.9*$(clock.sel).height()+'px');
  });
}
 
Clock.prototype.shared = function(key, shared_object) {
}
 
Clock.prototype.updateTime = function() {
  $(this.sel).css('font-size',0.9*$(this.sel).height()+'px');
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


