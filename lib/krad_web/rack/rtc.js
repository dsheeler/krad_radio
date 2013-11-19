$(document).ready(function() {
  rack_units.push({'constructor': Rtc, 'aspect': [32,20], 'page': 1});
});

function LocalStream(stream, containerDiv, divName) {
  containerDiv.append('<video id="' + divName
   + '" autoplay muted></video>');
  this.videoDiv = document.querySelector('#' + divName);
  this.videoDiv.width = 200;
  this.divName = divName;
  this.stream = stream;
  var audio_tracks = stream.getAudioTracks();
  var video_tracks = stream.getVideoTracks();
  console.log(stream);
  console.log('audio tracks: ' + audio_tracks.length);
  console.log('vid tracks: ' + video_tracks.length);
  for (var i = 0; i < audio_tracks.length; i++) {
    $('#local_streams').append('<li>' + audio_tracks[i].label + '</li>');
  }
  attachMediaStream(this.videoDiv, stream);
  this.attachedTo = [];
}

function Peer(name) {
  var boundHandleRemoteStreamAdded = handleRemoteStreamAdded.bind(this);
  this.connection = new RTCPeerConnection(null);
  this.connection.onicecandidate = handleIceCandidate;
  this.connection.onaddstream = boundHandleRemoteStreamAdded;
  this.connection.onremovestream = handleRemoteStreamRemoved;
  this.name = name;
  this.streams = [];
  console.log('Created RTCPeerConnnection');
}

Peer.prototype.destroy = function() {
}

function Rtc(info_object) {
  kr.rtc = this;
  this.title = "Rtc";
  this.description = "Real time communication, baby";
  this.aspect_ratio = [16,9];
  this.x = 0;
  this.y = 1;
  this.width = 0;
  this.height = 0;
  this.address_masks = ['rtc'];
  this.div_text = "<div id='rtc' class='RCU rtc'></div>";
  this.sel = "#rtc";

  this.peers = [];
  this.remoteStreams = [];
  this.localStreams = [];

  this.name = null;

  this.audio_context = null;

  this.isJoined = false;
  this.isHost = false;
  this.isStarted = false;
  this.turnReady;
  this.pc_constraints = {'optional': [{'DtlsSrtpKeyAgreement': true}]};
  this.remoteVideo;
  /*Set up audio and video regardless of what devices are present.*/
  this.sdpConstraints = {'mandatory': {
  'OfferToReceiveAudio':true,
  'OfferToReceiveVideo':true }};

  info_object['parent_div'].append(this.div_text);

  $(this.sel).append("\
  <div style='width:100%;float:left'>\
    <div class='button_wrap'>\
      <div class='krad_button' id='start_media'>Add Stream</div>\
    </div>\
    <div style='float:left'>\
      <div><input id='webrtc_name' type='text'></input></div>\
      <div class='button_wrap'>\
        <div class='krad_button' id='register'>Register</div>\
      </div>\
      <div class='button_wrap'>\
        <div class='krad_button' id='unregister'>Unregister</div>\
      </div>\
           </div>\
    <div style='float:left'>\
      <div><input id='callee_name' type='text'></input></div>\
      <div class='button_wrap'>\
        <div class='krad_button' id='start_chat'>Chat</div>\
      </div><br>\
    </div>\
    <ul style='float:left' id='user_list'></ul>\
    <div style='margin:20px;float:left' id='localVideoContainer'></div>\
    <ul id='local_streams'></ul>\
  </div>\
  <div style='margin:20px;float:left' id='remoteVideoContainer'></div>\
  <div id='dialog'></div>\
  ");

  this.localVideosContainer = $('#localVideoContainer');
  this.remoteVideosContainer = $('#remoteVideoContainer');

  var actxCall = window.webkitAudioContext || window.AudioContext;
  this.audio_context = new actxCall();

  $('#register').bind('click', function(e) {
    kr.rtc.register($('#webrtc_name').val());
  });

  $('#unregister').bind('click', function(e) {
    kr.rtc.unregister();
  });

  $('#start_media').bind('click', function(e) {
    kr.rtc.startMedia();
  });

  $('#start_chat').bind('click', function(e) {
    kr.rtc.callPeer($('#callee_name').val());
  });

  window.onbeforeunload = function(){
    console.log("SAYING GOODBYE");
    kr.rtc.sendMessage('bye');
  }

}

Rtc.prototype.ring = function() {
  for (var i = 0; i < 8; i++) {
    setTimeout(this.bell, 120*i);
  }
}

Rtc.prototype.bell = function() {
  playTone(80, 1.0,  kr.rtc.audio_context);
  playTone(120, 80/120.0, kr.rtc.audio_context);
  playTone(200, 80/200.0, kr.rtc.audio_context);
  playTone(333, 80/333.0, kr.rtc.audio_context);
  playTone(500, 80/500.0, kr.rtc.audio_context);
  playTone(800, 80/800.0, kr.rtc.audio_context);
  playTone(1100, 80/1100.0, kr.rtc.audio_context);
  playTone(2000, 80/2000.0, kr.rtc.audio_context);
  playTone(1500, 80/1500.0, kr.rtc.audio_context);
  playTone(2500, 80/2500.0, kr.rtc.audio_context);
}


function playTone(freq, amp, ctx) {
  var ctx;
  var o = ctx.createOscillator();
  var g = ctx.createGain();
  o.frequency.value = freq;
  o.type = "sine";
  o.connect(g);
  g.connect(ctx.destination);
  o.start(ctx.currentTime);
  g.gain.setValueAtTime(0.0,ctx.currentTime);
  g.gain.linearRampToValueAtTime(amp * 0.7, ctx.currentTime+0.01);
  g.gain.linearRampToValueAtTime(amp * 0.5, ctx.currentTime+0.02);
  g.gain.linearRampToValueAtTime(amp * 0.5, ctx.currentTime+0.08);
  g.gain.linearRampToValueAtTime(0.0, ctx.currentTime+0.1);
}

Rtc.prototype.register = function(name) {
  if (name != "") {
    this.name = name;
    console.log('Register Name: ' + this.name);

    var cmd = '{"rtc":"register","name":"' + this.name + '"}';
    this.sendMessage(cmd);
  }
}

Rtc.prototype.unregister = function() {
  var cmd = '{"rtc":"unregister"}';
  this.sendMessage(cmd);
}

Rtc.prototype.userRegistered = function(name) {
  $('#user_list').append('<li id="' + name + '">' + name
   + '</li>');
  var found = 0;
  for (var j = 0; j < this.peers.length; j++) {
    if (this.peers[j].name == name) {
      found = 1;
      break;
    }
  }

  if (name == this.name) {
    $('#' + name).html("<span>" + this.name + " (you)</span>");
  }

  if (name != this.name) {
    if (found == 0 && name != "") {
      console.log('creating peer', name);
      this.peers.push(new Peer(name));
    }
  }
}

Rtc.prototype.userUnregistered = function(name) {
  if (name == this.name) {
    $('#user_list > li').remove();
  } else {
    $('#' + name).remove();
  }

  for (var j = 0; j < this.peers.length; j++) {
    if ((this.name == name) || (this.peers[j].name == name)) {
      this.peers[j].destroy();
      this.peers.splice(j, 1);
    }
  }
}

Rtc.prototype.callPeer = function(name) {
  for (var i = 0; i < this.peers.length; i++) {
    if (this.peers[i].name == name) {
      for (var j = 0; j < this.localStreams.length; j++) {
        this.peers[i].connection.addStream(this.localStreams[j].stream);
        var stream = this.localStreams[j].stream;
        var audio_tracks = stream.getAudioTracks();
        var video_tracks = stream.getVideoTracks();
        console.log('audio tracks: ' + audio_tracks.length);
        console.log('vid tracks: ' + video_tracks.length);

      }
      this.peers[i].connection.createOffer(setLocalAndSendOffer, handleCreateOfferError);
    }
  }
}

Rtc.prototype.answerPeer = function(name) {
  for (var i = 0; i < this.peers.length; i++) {
    if (this.peers[i].name == name) {
      $('#callee_name').val(name);
      for (var j = 0; j < this.localStreams.length; j++) {
        try {
          this.peers[i].connection.addStream(this.localStreams[j].stream);
        } catch (e) {
          if (e.name != "NS_ERROR_NOT_AVAILABLE") {
            throw e;
          }
        }
      }
      this.peers[i].connection.createAnswer(setLocalAndSendAnswer, handleCreateAnswerError, this.sdpConstraints);
    }
  }
}

function setLocalAndSendOffer(sessionDescription) {
  // Set Opus as the preferred codec in SDP if Opus is present.
  sessionDescription.sdp = kr.rtc.preferOpus(sessionDescription.sdp);
  var callee_name = $('#callee_name').val();
  var cmd = '{"rtc":"call","name":"' + callee_name + '","sdp":"'
   + sessionDescription.sdp + '"}';
  console.log('setLocalAndSendMessage sending message', cmd);
  for (var i = 0; i < kr.rtc.peers.length; i++) {
    if (kr.rtc.peers[i].name == callee_name) {
      kr.rtc.peers[i].connection.setLocalDescription(sessionDescription);
    }
  }
  kr.rtc.sendMessage(cmd);
}

function setLocalAndSendAnswer(sessionDescription) {
  // Set Opus as the preferred codec in SDP if Opus is present.
  sessionDescription.sdp = kr.rtc.preferOpus(sessionDescription.sdp);
  var callee_name = $('#callee_name').val();
  var cmd = '{"rtc":"answer","name":"' + callee_name + '","sdp":"'
   + sessionDescription.sdp + '"}';
  for (var i = 0; i < kr.rtc.peers.length; i++) {
    if (kr.rtc.peers[i].name == callee_name) {
      kr.rtc.peers[i].connection.setLocalDescription(sessionDescription);
    }
  }
  kr.rtc.sendMessage(cmd);
}

Rtc.prototype.startMedia = function() {
  var constraints = {video: true, audio: true};
  getUserMedia(constraints, handleUserMedia, handleUserMediaError);
  console.log('Getting user media with constraints', constraints);
}

Rtc.prototype.sendMessage = function(message) {
  console.log('Sending: ', message);
  kr.rtc_send(message);
}

function handleUserMedia(stream) {
  console.log('Adding local stream.');
  var numLocalStreams = kr.rtc.localStreams.length;
  var divName = 'localVideo' + numLocalStreams;
  kr.rtc.localStreams.push(new LocalStream(stream, kr.rtc.localVideosContainer,
   divName));
}

function handleUserMediaError(error){
  console.log('navigator.getUserMedia error: ', error);
}

function handleIceCandidate(event) {
  console.log('handleIceCandidate event: ', event);
  if (event.candidate) {
    var callee_name = $('#callee_name').val();
    var cmd = '{"rtc":"candidate","user":"' + callee_name + '",'
     + '"type":"candidate","label":"'
     + event.candidate.sdpMLineIndex + '","id":"' + event.candidate.sdpMid
     + '","candidate":"' + event.candidate.candidate + '"}';
    kr.rtc.sendMessage(cmd);
  } else {
    console.log('End of candidates.');
  }
}

function handleRemoteStreamAdded(event) {
  console.log('Remote stream added.');
  this.streams.push(event.stream);
  var streamId = 'remoteVideo' + this.name + this.streams.length;
  var call_container_id = 'call_container_' + kr.rtc.name + '_' + this.name;
  $('#remoteVideoContainer').append('<div id="' + call_container_id 
   + '" class="call_container"><p class="remote_video_label">' + this.name 
   + '</p><video id="' + streamId 
   + '" class="remote_video" autoplay></video></div>');

  $('#' + call_container_id).draggable({stack: ".call_container"});
  $('#' + call_container_id).resizable({ aspectRatio: true});

  for(var i = 0; i < kr.rtc.localStreams.length; i++) {
    var vidId = 'call_local_video_' + this.name + '_' +i;
    var local_video_label_id = 'local_video_label_' + this.name + '_' + i;
    var local_video_div_id = 'local_video_div_' + this.name + '_' + i;
    $('#' + call_container_id).append('<div id="' + local_video_div_id 
     + '" class="local_video_div"><p id="' + local_video_label_id + '" '
     + 'class="local_video_label">' + kr.rtc.name
     + '</p><video id="' + vidId + '" class="local_video" '
     + 'muted autoplay></video></div>');
    attachMediaStream(document.querySelector('#' + vidId),
     kr.rtc.localStreams[i].stream);
    $('#' + local_video_label_id).onresize = function() {
      $(this).css('font-size', $(this).height() + 'px');
    };
    $('#' + local_video_div_id).css('top', (6 + 40*i) + '%');
    $('#' + local_video_div_id).draggable({
      containment: '#' + call_container_id,
      stop: function(e, ui) {
        var w = $('#' + call_container_id).width();
        var h = $('#' + call_container_id).height();
        $('#' + local_video_div_id).css('top', 100.0*ui.position.top/h + '%');
        $('#' + local_video_div_id).css('left', 100.0*ui.position.left/w + '%');
      }
    });
    $('#' + local_video_div_id).resizable({
      aspectRatio: true,
      containment: '#' + call_container_id,
      stop: function(e, ui) {
        var w = $('#' + call_container_id).width();
        $('#' + local_video_div_id).css('width', 100.0*ui.size.width/w + '%');
      }
    });
  }

  var vidElement = document.querySelector('#' + streamId);
  attachMediaStream(vidElement, event.stream);
}

function handleCreateOfferError(event){
  console.log('createOffer() error: ', event);
}

function handleCreateAnswerError(event){
  console.log('createAnswer() error: ', event);
}

Rtc.prototype.requestTurn = function(turn_url) {
  var pc_config = {'iceServers': [{'url': 'stun:stun.l.google.com:19302'}]};
  var turnExists = false;
  for (var i=0; i < pc_config.iceServers.length; i++) {
    if (pc_config.iceServers[i].url.substr(0, 5) === 'turn:') {
      turnExists = true;
      this.turnReady = true;
      break;
    }
  }
  if (!turnExists) {
    console.log('Getting TURN server from ', turn_url);
    // No TURN server. Get one from computeengineondemand.appspot.com:
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function(){
      if (xhr.readyState === 4 && xhr.status === 200) {
        var turnServer = JSON.parse(xhr.responseText);
        console.log('Got TURN server: ', turnServer);
        pc_config.iceServers.push({
          'url': 'turn:' + turnServer.username + '@' + turnServer.turn,
          'credential': turnServer.password
        });
        this.turnReady = true;
      }
    };
    xhr.open('GET', turn_url, true);
    xhr.send();
  }
}


function handleRemoteStreamRemoved(event) {
  console.log('Remote stream removed. Event: ', event);

}

Rtc.prototype.hangup = function() {
  console.log('Hanging up.');
  this.stop();
  this.sendMessage('bye');
}

Rtc.prototype.handleRemoteHangup = function() {
  console.log('Session terminated.');
  this.stop();
  this.isHost = true;
}

Rtc.prototype.stop = function() {
  this.isStarted = false;
  if (this.pc != null) this.pc.close();
  this.pc = null;
}

/*Set Opus as the default audio codec if it's present.*/
Rtc.prototype.preferOpus = function(sdp) {
  var sdpLines = sdp.split('\r\n');
  var mLineIndex = null;
  // Search for m line.
  for (var i = 0; i < sdpLines.length; i++) {
      if (sdpLines[i].search('m=audio') !== -1) {
        mLineIndex = i;
        break;
      }
  }
  if (mLineIndex === null) {
    return sdp;
  }

  // If Opus is available, set it as the default in m line.
  for (i = 0; i < sdpLines.length; i++) {
    if (sdpLines[i].search('opus/48000') !== -1) {
      var opusPayload = this.extractSdp(sdpLines[i], /:(\d+) opus\/48000/i);
      if (opusPayload) {
        sdpLines[mLineIndex] = this.setDefaultCodec(sdpLines[mLineIndex], opusPayload);
      }
      break;
    }
  }

  // Remove CN in m line and sdp.
  sdpLines = this.removeCN(sdpLines, mLineIndex);

  sdp = sdpLines.join('\r\n');
  return sdp;
}

Rtc.prototype.extractSdp = function(sdpLine, pattern) {
  var result = sdpLine.match(pattern);
  return result && result.length === 2 ? result[1] : null;
}

/*Set the selected codec to the first in m line.*/
Rtc.prototype.setDefaultCodec = function(mLine, payload) {
  var elements = mLine.split(' ');
  var newLine = [];
  var index = 0;
  for (var i = 0; i < elements.length; i++) {
    if (index === 3) { // Format of media starts from the fourth.
      newLine[index++] = payload; // Put target payload to the first.
    }
    if (elements[i] !== payload) {
      newLine[index++] = elements[i];
    }
  }
  return newLine.join(' ');
}

/*Strip CN from sdp before CN constraints is ready.*/
Rtc.prototype.removeCN = function(sdpLines, mLineIndex) {
  var mLineElements = sdpLines[mLineIndex].split(' ');

  for (var i = sdpLines.length-1; i >= 0; i--) {
    var payload = this.extractSdp(sdpLines[i], /a=rtpmap:(\d+) CN\/\d+/i);
    if (payload) {
      var cnPos = mLineElements.indexOf(payload);

      if (cnPos !== -1) {
        // Remove CN payload from m line.
        mLineElements.splice(cnPos, 1);
      }
      // Remove CN line in sdp
      sdpLines.splice(i, 1);
    }
  }

  sdpLines[mLineIndex] = mLineElements.join(' ');
  return sdpLines;
}


Rtc.prototype.update = function(crate) {

  if (crate.ctrl == 'user_registered') {
    console.log("Received user_registered");
    this.userRegistered(crate.user);
  }

  if (crate.ctrl == 'user_unregistered') {
    console.log("Received user_unregistered");
    this.userUnregistered(crate.user);
  }

  if (crate.ctrl == 'call') {
    this.ring();
    console.log('Received call', crate);
    $('#dialog').html("Answer incoming call from '" + crate.user + "'?");
    $('#dialog').dialog({
      autoOpen: true,
      buttons: {
        Ok: function() {
          for (var i = 0; i < kr.rtc.peers.length; i++) {
            if (kr.rtc.peers[i].name == crate.user) {
              kr.rtc.peers[i].connection.setRemoteDescription(
              new RTCSessionDescription({"sdp":crate.sdp, "type":"offer"}));
              kr.rtc.answerPeer(crate.user);
              break;
            }
          }
          $(this).dialog('close');
        },
        Cancel: function() {
          $(this).dialog('close');
        }
      }
    });

  } else if (crate.ctrl == 'answer') {
    console.log('Received Answer', crate);
    for (var i = 0; i < this.peers.length; i++) {
      if (this.peers[i].name == crate.user) {
        this.peers[i].connection.setRemoteDescription(
         new RTCSessionDescription({"sdp":crate.sdp, "type":"answer"}));
        break;
      }
    }
  } else if (crate.ctrl == 'candidate') {
    console.log('Received Candidate', crate);
    var candidate = new RTCIceCandidate({
     sdpMLineIndex: crate.label,
     candidate: crate.candidate
    });
    for (var i = 0; i < this.peers.length; i++) {
      if (this.peers[i].name == crate.user) {
        this.peers[i].connection.addIceCandidate(candidate);
      }
    }
  } else if (crate.message === 'bye') {
    this.handleRemoteHangup();
  }
}

Rtc.prototype.shared = function(key, shared_object) {
}






