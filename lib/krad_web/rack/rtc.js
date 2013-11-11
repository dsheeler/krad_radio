$(document).ready(function() {
  rack_units.push({'constructor': Rtc, 'aspect': [32,20], 'page': 1});
});

function LocalStream(stream, containerDiv, divName) {
  containerDiv.append('<video id="' + divName
   + '"></video>');
  this.videoDiv = document.querySelector('#' + divName);
  this.divName = divName;
  this.stream = stream;
  attachMediaStream(this.videoDiv, stream);
  this.attachedTo = [];
}

function Peer(name) {
  this.connection = new RTCPeerConnection(null);
  this.connection.onicecandidate = handleIceCandidate;
  this.connection.onaddstream = handleRemoteStreamAdded;
  this.connection.onremovestream = handleRemoteStreamRemoved;
  this.name = name;
  console.log('Created RTCPeerConnnection');
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
  <div style='float:left'>\
  <div><input id='webrtc_name' type='text'></input></div>\
  <div class='button_wrap'>\
  <div class='krad_button' id='start_webrtc'>Register</div>\
  </div>\
  <div class='button_wrap'>\
  <div class='krad_button' id='start_media'>Add Camera</div>\
  </div>\
  <div style='flaot:left'>\
  <div><input id='callee_name' type='text'></input></div>\
  <div class='button_wrap'>\
  <div class='krad_button' id='start_chat'>Chat</div>\
  </div>\
  <div id='localVideoContainer'></div>\
  <div id='remoteVideoContainer'>\
  </div></div>");

  this.localVideosContainer = $('#localVideoContainer');
  this.remoteVideosContainer = $('#remoteVideoContainer');

  $('#start_webrtc').bind('click', function(e) {
    kr.rtc.name = $('#webrtc_name').val();
    console.log('Register Name: ' + kr.rtc.name);

    var cmd = '{"rtc":"register","name":"' + kr.rtc.name + '"}';
    kr.rtc_send(cmd);
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

Rtc.prototype.callPeer = function(name) {
  for (var i = 0; i < this.peers.length; i++) {
    if (this.peers[i].name == name) {
      for (var j = 0; j < this.localStreams.length; j++) {
        this.peers[i].connection.addStream(this.localStreams[j].stream);
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
        this.peers[i].connection.addStream(this.localStreams[j].stream);
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
  console.log('setLocalAndSendMessage sending message', sessionDescription.sdp);
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
  var divName = 'localVideo' + (numLocalStreams-1);
  kr.rtc.localStreams.push(new LocalStream(stream, kr.rtc.localVideosContainer,
   divName));
}


function handleUserMediaError(error){
  console.log('navigator.getUserMedia error: ', error);
}

Rtc.prototype.maybeStart = function() {
  if (!this.isStarted && typeof this.localStream != 'undefined'
   && this.isJoined) {
    this.createPeerConnection();
    this.pc.addStream(this.localStream);
    this.isStarted = true;
    console.log('isHost', this.isHost);
  }
}

function handleIceCandidate(event) {
  console.log('handleIceCandidate event: ', event);
  if (event.candidate) {
    /*kr.rtc.sendMessage('{"type":"candidate","label":"' 
     + event.candidate.sdpMLineIndex + '","id":"' + event.candidate.sdpMid 
     + ',"candidate":"' + event.candidate.candidate + '"})');*/
  } else {
    console.log('End of candidates.');
  }
}

function handleRemoteStreamAdded(event) {
  console.log('Remote stream added.');
  kr.rtc.remoteStreams.push(event.stream);
  var streamId = 'remoteVideo' + kr.rtc.remoteStreams.length;

  $('#remoteVideoContainer').append('<video autoplay id="' + streamId + '"></video>');

  var vidElement = document.querySelector('#' + streamId);

  attachMediaStream(vidElement, event.stream);
}

function handleCreateOfferError(event){
  console.log('createOffer() error: ', event);
}

function handleCreateAnswerError(event){
  console.log('createAnswer() error: ', event);
}

Rtc.prototype.doCall = function() {
  console.log('Sending offer to peer');
}

Rtc.prototype.doAnswer = function() {
  console.log('Sending answer to peer.');
  this.pc.createAnswer(setLocalAndSendMessage, handleCreateAnswerError, this.sdpConstraints);
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
  this.maybeStart();
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

  if (crate.names != null) {
    for (var i = 0; i < crate.names.length; i++) {
      if (crate.names[i] != this.name) {
        var found = 0;
        for (var j = 0; j < this.peers.length; j++) {
          if (this.peers[j].name == crate.names[i]) {
            found = 1;
            break;
          }
        }
        if (found == 0 && crate.names[i] != "") {
          console.log('creating until now unseen peer', crate.names[i])
          this.peers.push(new Peer(crate.names[i]));
        }
      }
    }
  }

  if (crate.ctrl == 'call') {
    console.log('Client received call', crate);
    for (var i = 0; i < this.peers.length; i++) {
      if (this.peers[i].name == crate.user) {
        this.peers[i].connection.setRemoteDescription(new RTCSessionDescription({"sdp":crate.sdp, "type":"offer"}));
        this.answerPeer(crate.user);
        break;
      }
    }
  } else if (crate.ctrl == 'answer') {
    for (var i = 0; i < this.peers.length; i++) {
      if (this.peers[i].name == crate.user) {
        this.peers[i].connection.setRemoteDescription(new RTCSessionDescription({"sdp":crate.sdp, "type":"answer"}));
        break;
      }
    }
  }
    /*} else if (crate.message.type === 'candidate' && this.isStarted) {
      var candidate = new RTCIceCandidate({
        sdpMLineIndex: crate.message.label,
        candidate: crate.message.candidate
      });
      this.pc.addIceCandidate(candidate);
    } else if (crate.message === 'bye') {
      this.handleRemoteHangup();
    }
  }*/
}

Rtc.prototype.shared = function(key, shared_object) {
}






