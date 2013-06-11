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

function Kradwebsocket(port) {
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
      kradradio.destroy();
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
  } else {
    this.debug ("NOT CONNECTED, wanted to send: " + message); 
  }
}

Kradwebsocket.prototype.on_error = function(evt) {
  this.debug("Error! " + evt.data);
}

Kradwebsocket.prototype.debug = function(message) {
  console.log(message);
}

Kradwebsocket.prototype.on_message = function(evt) {
  kradradio.got_messages (evt.data);  
}

Kradradio.prototype.push_dtmf = function(value) {
  kradradio.ctrl("m/DTMF/t", value);
  kradradio.kode += value;
  kradradio.check_kode();
}

Kradradio.prototype.not_admin = function() {
  alert("Sorry your not an admin right now! But you could be one day!");
}

Kradradio.prototype.enable_admin = function() {
  this.admin = 1;
  kradwebsocket.debug("admin enabled");
}

Kradradio.prototype.check_kode = function() {
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

Kradradio.prototype.ctrl = function(path, val, dur) {

  if (typeof(dur) === 'undefined') dur = 0;

  var cmd = '{"ctrl":"' + path + ' ' + val + ' ' + dur + '"}';

  kradwebsocket.send(cmd);
  kradwebsocket.debug(cmd);
}

Kradradio.prototype.crate_handler = function (crate) {
    if (crate.com == "kradmixer") {
        if (crate.ctrl == "peak_portgroup") {
            kradradio.got_peak_portgroup (crate.portgroup_name, crate.value);
            return;
        }
        if (crate.ctrl == "control_portgroup") {
            kradradio.got_control_portgroup (crate.portgroup_name, crate.control_name, crate.value);
            return;
        }
        if (crate.ctrl == "effect_control") {
            kradradio.got_effect_control (crate);
            return;
        }
        if (crate.ctrl == "update_portgroup") {
            kradradio.got_update_portgroup (crate.portgroup_name, crate.control_name, crate.value);
            return;
        }      
        if (crate.ctrl == "add_portgroup") {
            kradradio.got_add_portgroup ( crate );
            return;
        }
        if (crate.ctrl == "remove_portgroup") {
            kradradio.got_remove_portgroup (crate.portgroup_name);
            return;
        }
        if (crate.ctrl == "set_mixer_params") {
            kradradio.set_mixer_params (crate);
            return;
        }
    }
    if (crate.com == "kradcompositor") {
        if (crate.ctrl == "add_subunit") {
            kradradio.got_add_subunit ( crate );
            return;
        }  
        if (crate.ctrl == "remove_subunit") {
            kradradio.got_remove_subunit (crate );
            return;
        }
        if (crate.ctrl == "update_subunit") {
            kradradio.got_update_subunit (crate.subunit_id, crate.subunit_type, crate.control_name, crate.value);
            return;
        }
        if (crate.ctrl == "set_frame_rate") {
            kradradio.got_frame_rate (crate.numerator, crate.denominator);
            return;
        }
        if (crate.ctrl == "set_frame_size") {
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
        if (crate.ctrl == "update_link") {
            kradradio.got_update_link (crate.link_num, crate.update_item, crate.update_value);
            return;
        }    
        if (crate.ctrl == "add_link") {
            kradradio.got_add_link (crate);
            return;
        }
        if (crate.ctrl == "remove_link") {
            kradradio.got_remove_link (crate);
            return;
        }     
        if (crate.ctrl == "add_decklink_device") {
            kradradio.got_add_decklink_device (crate);
            return;
        }
    }
}

Kradradio.prototype.got_messages = function (msgs) {
  var msg_arr = JSON.parse (msgs);
  for (m in msg_arr) {
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
}

Kradradio.prototype.set_mixer_params = function (mixer_params) {
  this.sample_rate = mixer_params.sample_rate;
  $('.kradmixer_info').append("<div>Mixer Sample Rate: " + this.sample_rate + " </div>"); 
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
