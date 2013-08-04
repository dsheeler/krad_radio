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
  if (this.connected != true) {
    this.reconnection_attempts += 1;
    if (this.reconnection_attempts == 3) {
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
}

Kr.prototype.ws_on_close = function(evt) {
  this.ws_connected = false;
  this.debug("Disconnected!");
  if (kr != false) {
    kr.destroy();
    kr = false;
  }
  this.ws_connecting = false;
  if (this.ws_stay_connected == true) {
    if (this.ws_reconnect == false) {
      this.ws_reconnect = setInterval(
       create_handler(this, this.ws_do_reconnect), 1000);
    }
  }
}

Kr.prototype.ws_send = function(message) {
  if (this.ws_connected == true) {
    this.ws.send(message);
  } else {
    this.debug("NOT CONNECTED, wanted to send: " + message);
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
  for (m in msg_arr) {
    this.crate_handler(msg_arr[m]);
  }
}

Kr.prototype.push_dtmf = function(value) {
  kr.ctrl("m/DTMF/t", value);
  kr.kode += value;
}

Kr.prototype.xmms2 = function(portgroup_name, xmms2_command) {
  var cmd = '{"xmms2":"' + portgroup_name + ' ' + xmms2_command + ' "}';
  this.ws_send(cmd);
  this.debug(cmd);
}

Kr.prototype.ctrl = function(path, val, dur) {
  if (typeof(dur) === 'undefined') dur = 0;
  var cmd = '{"ctrl":"' + path + ' ' + val + ' ' + dur + '"}';
  this.ws_send(cmd);
  this.debug(cmd);
}

Kr.prototype.crate_handler = function(crate) {
  if (crate.com == "kradmixer") {
    if (crate.ctrl == "peak_portgroup") {
      kr.got_peak_portgroup(crate.portgroup_name, crate.value);
      return;
    }
    if (crate.ctrl == "control_portgroup") {
      kr.got_control_portgroup(crate.portgroup_name,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "effect_control") {
      kr.got_effect_control(crate);
      return;
    }
    if (crate.ctrl == "update_portgroup") {
      kr.got_update_portgroup(crate.portgroup_name,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "add_portgroup") {
      kr.got_add_portgroup(crate);
      return;
    }
    if (crate.ctrl == "remove_portgroup") {
      kr.got_remove_portgroup(crate.portgroup_name);
      return;
    }
    if (crate.ctrl == "set_mixer_params") {
      kr.set_mixer_params(crate);
      return;
    }
  }
  if (crate.com == "kradcompositor") {
    if (crate.ctrl == "add_subunit") {
      kr.got_add_subunit(crate);
      return;
    }
    if (crate.ctrl == "remove_subunit") {
      kr.got_remove_subunit(crate);
      return;
    }
    if (crate.ctrl == "update_subunit") {
      kr.got_update_subunit(crate.subunit_id, crate.subunit_type,
       crate.control_name, crate.value);
      return;
    }
    if (crate.ctrl == "set_frame_rate") {
      kr.got_frame_rate(crate.numerator, crate.denominator);
      return;
    }
    if (crate.ctrl == "set_frame_size") {
      kr.got_frame_size(crate.width, crate.height);
      return;
    }
  }
  if (crate.com == "kradradio") {
    if (crate.info == "cpu") {
      kr.got_system_cpu_usage(crate.system_cpu_usage);
      return;
    }
    if (crate.info == "sysname") {
      kr.got_sysname(crate.infoval);
      return;
    }
  }
  if (crate.com == "kradlink") {
    if (crate.ctrl == "update_link") {
      kr.got_update_link(crate.link_num,
       crate.update_item, crate.update_value);
      return;
    }
    if (crate.ctrl == "add_link") {
      kr.got_add_link(crate);
      return;
    }
    if (crate.ctrl == "remove_link") {
      kr.got_remove_link(crate);
      return;
    }
    if (crate.ctrl == "add_decklink_device") {
      kr.got_add_decklink_device(crate);
      return;
    }
  }
}

Kr.prototype.display = function() {
  var cmd = {};
  cmd.com = "kradcompositor";
  cmd.cmd = "display";

  var JSONcmd = JSON.stringify(cmd);
  this.ws_send(JSONcmd);
}

Kr.prototype.addsprite = function(filename) {
  var cmd = {};
  cmd.com = "kradcompositor";
  cmd.cmd = "add_subunit";
  cmd.subunit_type = "sprite";
  cmd.filename = filename;

  var JSONcmd = JSON.stringify(cmd);
  this.ws_send(JSONcmd);
}

Kr.prototype.destroy = function() {
  $('#' + this.sysname).remove();
}

function Kr() {
  this.sysname = "";
  this.sample_rate = 0;
  this.peaks = new Array();
  this.peaks_max = new Array();
  this.system_cpu_usage = 0;
  this.ux = {};
  this.xmms2_portgroup_count = 0;
  this.compositor_canvas = null;
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
