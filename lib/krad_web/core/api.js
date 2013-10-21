function create_handler (inst, func) {
  return (function(e){
    func.call(inst, e);
  });
}

window.requestAnimFrame = (function(){
  return  window.requestAnimationFrame
   || window.webkitRequestAnimationFrame
   || window.mozRequestAnimationFrame
   || window.oRequestAnimationFrame
   || window.msRequestAnimationFrame
   || function(callback){
      window.setTimeout(callback, 1000 / 60);
    };
})();

Kr.prototype.ws_do_reconnect = function() {
  if (this.ws_connected != true) {
    this.ws_reconnection_attempts += 1;
    if (this.ws_reconnection_attempts == 3) {
      $('#kradradio').append("<div id='websockets_connection_problem'>"
       + "<h2>Websockets connection problem using port " + window.location.port
       + "</h2></div>");
    }
    this.ws_connect();
  }
}

Kr.prototype.ws_connect = function() {
  this.ws_stay_connected = true;
  if (this.ws_connected != true) {
    if (this.ws_connecting != true) {
      this.ws_connecting = true;
      this.debug("Connecting..");
      this.ws = new WebSocket(this.ws_uri, "krad-ws-api");
      this.ws.onopen = create_handler(this, this.ws_on_open);
      this.ws.onclose = create_handler(this, this.ws_on_close);
      this.ws.onmessage = create_handler(this, this.ws_on_message);
      this.ws.onerror = create_handler(this, this.ws_on_error);
    } else {
      this.debug("Tried to connect but in the process of connecting.");
    }
  } else {
    this.debug("Tried to connect when already connected.");
  }
}

Kr.prototype.ws_disconnect = function() {
  this.ws_connected = false;
  this.ws_connecting = false;
  this.ws_stay_connected = false;
  this.debug("Disconnecting..");
  if (this.ws_reconnect != false) {
    window.clearInterval(this.ws_reconnect);
    this.ws_reconnect = false;
  }
  this.ws.close();
}

Kr.prototype.ws_on_open = function(evt) {
  this.ws_connected = true;
  this.debug("Connected!");
  this.ws_connecting = false;
  this.ws_reconnection_attempts = 0;

  if (this.ws_reconnect != false) {
    window.clearInterval(this.ws_reconnect);
    this.ws_reconnect = false;
  }
  if ($('#websockets_connection_problem')) {
    $('#websockets_connection_problem').remove();
  }
  this.load_interface();
}

Kr.prototype.ws_on_close = function(evt) {
  this.ws_connected = false;
  this.debug("Disconnected!");
  this.unload_interface();
  this.ws_connecting = false;
  if (this.ws_stay_connected == true) {
    if (this.ws_reconnect == false) {
      this.ws_reconnect = setInterval(
       create_handler(this, this.ws_do_reconnect), 1000);
    }
  }
}

Kr.prototype.ws_on_error = function(evt) {
  this.debug("Error! " + evt.data);
}

Kr.prototype.debug = function(message) {
  console.log(message);
}

Kr.prototype.ws_on_message = function(evt) {
  this.debug("Recved: " + evt.data);
  var msg_arr = JSON.parse(evt.data);
  for (var i = 0; i < msg_arr.length; i++) {
    this.ux.on_crate(msg_arr[i]);
  }
}

Kr.prototype.ctrl = function(path, val, dur) {
  if (typeof(dur) === 'undefined') dur = 0;
  var cmd = '{"ctrl":"' + path + ' ' + val + ' ' + dur + '"}';
  this.ws.send(cmd);
  this.debug(cmd);
}

Kr.prototype.rtc_send = function(cmd) {
  this.ws.send(cmd);
  this.debug(cmd);
}

function Kr() {
  this.ux = {};
  this.ws_uri = 'ws://' + location.hostname + ':' + window.location.port + '/';
  this.ws = "";
  this.ws_reconnection_attempts = 0;
  this.ws_reconnect = false;
  this.ws_connected = false;
  this.ws_connecting = false;
  this.ws_stay_connected = true;
  this.debug("Krad Radio client created");
  this.ws_connect();
}

Kr.prototype.unload_interface = function() {
	this.ux.destroy();
	this.ux = false;
}
