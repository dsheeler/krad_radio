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

function Kradwebsocket(port) {
  this.port = port;
  this.uri = 'ws://' + location.hostname + ':' + this.port + '/';
  this.websocket = "";
  this.reconnection_attempts = 0;
  this.reconnect = false;  
  this.connected = false;
  this.connecting = false;
  this.stay_connected = true;  
  this.debug("Created");
  this.connect();
}

Kradwebsocket.prototype.do_reconnect = function() {
  if (this.connected != true) {
    this.reconnection_attempts += 1;
    if (this.reconnection_attempts == 3) {
      $('#kradradio').append("<div id='websockets_connection_problem'>" 
       + "<h2>Websockets connection problem using port " + this.port 
       + "</h2></div>");
    }
    this.connect();
  }
}

Kradwebsocket.prototype.connect = function() {
  this.stay_connected = true;
  if (this.connected != true) {
    if (this.connecting != true) {
      this.connecting = true;
      this.debug("Connecting..");
      this.websocket = new WebSocket(this.uri, "krad-ws-api");
      this.websocket.onopen = create_handler(this, this.on_open);
      this.websocket.onclose = create_handler(this, this.on_close);
      this.websocket.onmessage = create_handler(this, this.on_message);
      this.websocket.onerror = create_handler(this, this.on_error);
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
  this.debug("Disconnecting..");
  if (this.reconnect != false) {
    window.clearInterval(this.reconnect);
    this.reconnect = false;
  }
  this.websocket.close();
}

Kradwebsocket.prototype.on_open = function(evt) {
  this.connected = true;
  this.debug("Connected!");
  this.connecting = false;
  this.reconnection_attempts = 0;

  if (this.reconnect != false) {
    window.clearInterval(this.reconnect);
    this.reconnect = false;
  }
  if ($('#websockets_connection_problem')) {
    $('#websockets_connection_problem').remove();
  }

  kradradio = new Kradradio();  
}

Kradwebsocket.prototype.on_close = function(evt) {

  this.connected = false;
  this.debug("Disconnected!");
  if (kradradio != false) {
    kradradio.destroy();
    kradradio = false;
  }
  this.connecting = false;
  if (this.stay_connected == true) {
    if (this.reconnect == false) {
      this.reconnect = setInterval(
       create_handler(this, this.do_reconnect), 1000);
    }
  }
}

Kradwebsocket.prototype.send = function(message) {
  if (this.connected == true) {
    this.debug("Sending: " + message); 
    this.websocket.send(message);
  } else {
    this.debug("NOT CONNECTED, wanted to send: " + message); 
  }
}

Kradwebsocket.prototype.on_error = function(evt) {
  this.debug("Error! " + evt.data);
}

Kradwebsocket.prototype.debug = function(message) {
  console.log(message);
}

Kradwebsocket.prototype.on_message = function(evt) {
  this.debug("Recved: " + evt.data); 
  kradradio.got_messages(evt.data);  
}


Kradradio.prototype.push_dtmf = function(value) {
  kradradio.ctrl("m/DTMF/t", value);
  kradradio.kode += value;
  kradradio.check_kode();
}

Kradradio.prototype.xmms2 = function(portgroup_name, xmms2_command) {
  var cmd = '{"xmms2":"' + portgroup_name + ' ' + xmms2_command + ' "}';

  kradwebsocket.send(cmd);
  //kradwebsocket.debug(cmd);
}

Kradradio.prototype.ctrl = function(path, val, dur) {

  if (typeof(dur) === 'undefined') dur = 0;

  var cmd = '{"ctrl":"' + path + ' ' + val + ' ' + dur + '"}';

  kradwebsocket.send(cmd);
  //kradwebsocket.debug(cmd);
}

Kradradio.prototype.crate_handler = function(crate) {
  if (crate.com == "kradmixer") {
    if (crate.ctrl == "peak_portgroup") {
      kradradio.got_peak_portgroup(crate.portgroup_name, crate.value);
      return;
    }
    if (crate.ctrl == "control_portgroup") {
      kradradio.got_control_portgroup(crate.portgroup_name, 
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "effect_control") {
      kradradio.got_effect_control(crate);
      return;
    }
    if (crate.ctrl == "update_portgroup") {
      kradradio.got_update_portgroup(crate.portgroup_name, 
       crate.control_name, crate.value);
      return;
    }      
    if (crate.ctrl == "add_portgroup") {
      kradradio.got_add_portgroup(crate);
      return;
    }
    if (crate.ctrl == "remove_portgroup") {
      kradradio.got_remove_portgroup(crate.portgroup_name);
      return;
    }
    if (crate.ctrl == "set_mixer_params") {
      kradradio.set_mixer_params(crate);
      return;
    }
  }
  if (crate.com == "kradcompositor") {
    if (crate.ctrl == "add_subunit") {
      kradradio.got_add_subunit(crate);
      return;
    }  
    if (crate.ctrl == "remove_subunit") {
      kradradio.got_remove_subunit(crate);
      return;
    }
    if (crate.ctrl == "update_subunit") {
      kradradio.got_update_subunit(crate.subunit_id, crate.subunit_type, 
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "set_frame_rate") {
      kradradio.got_frame_rate(crate.numerator, crate.denominator);
      return;
    }
    if (crate.ctrl == "set_frame_size") {
      kradradio.got_frame_size(crate.width, crate.height);
      return;
    }      
  }    
  if (crate.com == "kradradio") {
    if (crate.info == "cpu") {
      kradradio.got_system_cpu_usage(crate.system_cpu_usage);
      return;
    }
    if (crate.info == "sysname") {
      kradradio.got_sysname(crate.infoval);
      return;
    }
  }    
  if (crate.com == "kradlink") {
    if (crate.ctrl == "update_link") {
      kradradio.got_update_link(crate.link_num, 
       crate.update_item, crate.update_value);
      return;
    }    
    if (crate.ctrl == "add_link") {
      kradradio.got_add_link(crate);
      return;
    }
    if (crate.ctrl == "remove_link") {
      kradradio.got_remove_link(crate);
      return;
    }     
    if (crate.ctrl == "add_decklink_device") {
      kradradio.got_add_decklink_device(crate);
      return;
    }
  }
}

Kradradio.prototype.got_messages = function(msgs) {
  var msg_arr = JSON.parse(msgs);
  for (m in msg_arr) {
    this.crate_handler(msg_arr[m]);
  }
}

Kradradio.prototype.display = function() {

  var cmd = {};  
  cmd.com = "kradcompositor";  
  cmd.cmd = "display";

  var JSONcmd = JSON.stringify(cmd); 

  kradwebsocket.send(JSONcmd);
}

Kradradio.prototype.addsprite = function(filename) {

  var cmd = {};  
  cmd.com = "kradcompositor";  
  cmd.cmd = "add_subunit";
  cmd.subunit_type = "sprite";
  cmd.filename = filename;

  var JSONcmd = JSON.stringify(cmd); 

  kradwebsocket.send(JSONcmd);
}

function Kradradio() {
  this.sysname = "";
  this.sample_rate = 0;
 
  this.decklink_devices = new Array();
  this.v4l2_devices = new Array();

  this.peaks = new Array();
  this.peaks_max = new Array();
  this.showing_snapshot_buttons = 0;
  this.system_cpu_usage = 0;
  this.admin = 0;
  this.kode = "";
  this.secretkode = "31337";
  this.portgroup_count = 0;
  this.portgroup_volume_sliders = new Object();
  this.portgroup_crossfade_sliders = new Object();
  this.portgroup_effect_sliders = new Object();
  this.portgroup_eq_sliders = new Object();
  this.xmms2_portgroup_count = 0;
  this.prepare_link_operation_mode = "";
  this.compositor_canvas = null;
  this.compositor_mode = "Position";
  this.sprites = new Array();

  /* set up the selection handle boxes */
  this.selectionHandles = [];

  for (var i = 0; i < 10; i ++) {
    var rect = new Shape;
    this.selectionHandles.push(rect);
  }
}

Kradradio.prototype.destroy = function() {
  $('#' + this.sysname).remove();
}

Kradradio.prototype.set_mixer_params = function(mixer_params) {

  this.sample_rate = mixer_params.sample_rate;
  $('.kradmixer_info').append("<div>Mixer Sample Rate: " 
   + this.sample_rate + " </div>"); 

}

