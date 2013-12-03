$(document).ready(function() {
  rack_units.push({'constructor': Rtc, 'aspect': [32,20], 'page': 1});
});

function LocalStream(name, streamId, stream, containerDiv, divName, canvasId) {

  this.name = name;
  this.divName = divName;
  this.canvasId = canvasId;
  this.streamId = streamId;
  this.groupId = streamId + '_group';

  containerDiv.append('<div class="local_stream_div ' + this.groupId
   + '" id="' + this.divName + '"></div>');

  var local_stream_div = $('#' + this.divName);
  var local_stream_id = "local_stream_" + this.streamId;
  local_stream_div.append('<p id="' + local_stream_id + '" '
     + 'class="local_stream_label">' + this.name
     + '</p><canvas class="local_stream_spectrum_canvas" id="'
   + canvasId + '"></canvas>');

  var vidName = this.divName + '_video';
  local_stream_div.append('<video class="' + this.groupId 
   + ' local_stream_video"'
   + ' id="' + vidName + '" autoplay muted></video>');

  $('.' + this.groupId).css('visibility', 'hidden');

  this.videoDiv = document.querySelector('#' + vidName);
  $('#' + vidName).bind('play', {'groupId': this.groupId}, function(e) {
    kr.rtc.handleLocalVideoStream(e.data.groupId, vidName, local_stream_div);
  });

  this.vidName = vidName;

  this.spectrumCanvasId = canvasId;
  this.spectrumCanvas = document.querySelector('#' + this.spectrumCanvasId);

  this.stream = stream;
  var audio_tracks = stream.getAudioTracks();
  var video_tracks = stream.getVideoTracks();
  console.log(stream);
  console.log('audio tracks: ' + audio_tracks.length);
  console.log('vid tracks: ' + video_tracks.length);
  attachMediaStream(this.videoDiv, stream);
}

LocalStream.prototype.removeStream = function() {
}

Rtc.prototype.handleLocalVideoStream = function(groupId, vidName, local_stream_div) {
  if ($('#' + vidName).height() != 150) {
      var w = $('#' + vidName).width();
      var h = $('#' + vidName).height();
      var r = w / h;
      var new_w = 300;
      $('#' + vidName).css('width', new_w);
      $('#' + vidName).css('height', new_w / r);
      local_stream_div.css('width', $('#' + vidName).width());
      local_stream_div.css('height', $('#' + vidName).height());
      $('.' + groupId).css('visibility', 'visible');
      $('.' + groupId).hide();
      $('.' + groupId).show("highlight");
  } else {
    setTimeout(function() {
      kr.rtc.handleLocalVideoStream(groupId, vidName, local_stream_div);
    }, 100);
  }
}

function Peer(name) {
  this.connection = new RTCPeerConnection(null);
  this.connection.onicecandidate = handleIceCandidate.bind(this);
  this.connection.onaddstream = handleRemoteStreamAdded.bind(this);
  this.connection.onremovestream = handleRemoteStreamRemoved;
  this.name = name;
  this.streams = [];
  this.readyToProcessCandidates = false;
  this.queuedCandidates = [];
  console.log('Created RTCPeerConnnection');
}

Peer.prototype.handleIceCandidate = function(candidate) {
  this.queuedCandidates.push(candidate);
  this.processQueuedCandidates();
}

Peer.prototype.processQueuedCandidates = function() {
  if (this.readyToProcessCandidates == true) {
    while(this.queuedCandidates.length > 0) {
      this.connection.addIceCandidate(this.queuedCandidates.shift());
    }
  }
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

  var audio_context_call = window.webkitAudioContext || window.AudioContext;
  this.audio_context = new audio_context_call();
  this.peers = [];
  this.remoteStreams = [];
  this.localStreams = [];

  this.name = null;

  this.audio_context = null;

  this.turnReady;
  this.pc_constraints = {'optional': [{'DtlsSrtpKeyAgreement': true}]};
  /*Set up audio and video regardless of what devices are present.*/
  this.sdpConstraints = {'mandatory': {
  'OfferToReceiveAudio':true,
  'OfferToReceiveVideo':true }};

  info_object['parent_div'].append(this.div_text);

  $(this.sel).append("\
  <div style='width:100%;float:left'>\
    <audio id='six_login_audio'>\
      <source src='http://six600110.com/bong.ogg' type='audio/ogg' />\
    </audio>\
    <audio id='logout_audio'>\
      <source src='http://six600110.com/logout.wav' type='audio/wav' />\
    </audio>\
    <audio id='login_audio'>\
      <source src='http://six600110.com/login.wav' type='audio/wav' />\
    </audio>\
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
        <div class='krad_button' id='start_chat'>Call</div>\
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
  playTone(80, 0.01,  kr.rtc.audio_context);
  playTone(240, 3/120.0, kr.rtc.audio_context);
  playTone(400, 2/200.0, kr.rtc.audio_context);
  playTone(666, 2/333.0, kr.rtc.audio_context);
  playTone(1000, 10/500.0, kr.rtc.audio_context);
  playTone(1600,  8/800.0, kr.rtc.audio_context);
  playTone(2200, 5/1100.0, kr.rtc.audio_context);
  playTone(4000, 20/2000.0, kr.rtc.audio_context);
  playTone(3000, 30/1500.0, kr.rtc.audio_context);
  playTone(5000, 12/2500.0, kr.rtc.audio_context);
}


function playTone(freq, amp, ctx) {
  var ctx;
  var o = ctx.createOscillator();
  var g = ctx.createGain();
  o.frequency.value = freq;
  o.type = "sine";
  o.connect(g);
  g.connect(ctx.destination);
  o.start(ctx.currentTime)
  g.gain.setValueAtTime(0.0,ctx.currentTime);
  g.gain.linearRampToValueAtTime(amp * 0.8, ctx.currentTime+0.01);
  g.gain.linearRampToValueAtTime(amp * 0.6, ctx.currentTime+0.02);
  g.gain.linearRampToValueAtTime(amp * 0.6, ctx.currentTime+0.08);
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
  var audio_name = 'audio';
  $('#user_list').append('<li id="' + name + '">' + name
  + '</li>');

  for (var i = 0; i < kr.rtc.localStreams.length; i++) {
    var button_id = "call_" + name + "_with_"
     + this.localStreams[i].streamId;

    $('#' + name).append("<span class='krad_button' id='" + button_id + "'>Call with '"
     + this.localStreams[i].name + "'</span>");
    $('#' + button_id).bind("click", {'name': name, 'stream':
      kr.rtc.localStreams[i].stream}, function(e) {
      kr.rtc.callPeer(e.data.name, e.data.stream);
    });
  }

  var audio = document.getElementById('login_audio');
  if (name == 'six' || name == 'six600110' || name == 'dan' 
   || name == 'cheech') {
     audio = document.getElementById('six_login_audio');
  }
  audio.play();
  var found = 0;
  for (var j = 0; j < this.peers.length; j++) {
    if (this.peers[j].name == name) {
      found = 1;
      break;
    }
  }

  if (name == this.name) {
    $('#' + name).html(this.name + " (you)");
  } else {
    $('#' + name).droppable( {
      drop: function(event, ui) {
        alert("Calling " + name);
        kr.rtc.callPeer(name, ui.stream);
      }
    });
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
  var audio = document.getElementById('logout_audio');
  audio.play();

  for (var j = 0; j < this.peers.length; j++) {
    if ((this.name == name) || (this.peers[j].name == name)) {
      this.peers[j].destroy();
      this.peers.splice(j, 1);
    }
  }
}

Rtc.prototype.callPeer = function(name, stream) {
  for (var i = 0; i < this.peers.length; i++) {
    if (this.peers[i].name == name) {
      if (typeof(stream) != 'undefined') {
        this.peers[i].connection.addStream(stream);
      } else {
        for (var j = 0; j < this.localStreams.length; j++) {
          try {
            this.peers[i].connection.addStream(this.localStreams[j].stream);
            var stream = this.localStreams[j].stream;
            var audio_tracks = stream.getAudioTracks();
            var video_tracks = stream.getVideoTracks();
            console.log('audio tracks: ' + audio_tracks.length);
            console.log('vid tracks: ' + video_tracks.length);
          } catch (e) {
            if (e.name != "NS_ERROR_UNEXPECTED") {
              throw e;
            } else {
              console.log('error adding local stream to chat', e);
            }
          }
        }
      }
      this.peers[i].connection.createOffer(
       setLocalAndSendOffer.bind(this.peers[i]), handleCreateOfferError);
    }
  }
}

Rtc.prototype.answerPeer = function(name) {
  for (var i = 0; i < this.peers.length; i++) {
    if (this.peers[i].name == name) {
      for (var j = 0; j < this.localStreams.length; j++) {
        try {
          this.peers[i].connection.addStream(this.localStreams[j].stream);
        } catch (e) {
          if (e.name != "NS_ERROR_NOT_AVAILABLE") {
            throw e;
          }
        }
      }
      kr.rtc.peers[i].readyToProcessCandidates = true;
      kr.rtc.peers[i].processQueuedCandidates();
      this.peers[i].connection.createAnswer(
       setLocalAndSendAnswer.bind(this.peers[i]),
       handleCreateAnswerError, this.sdpConstraints);
    }
  }
}

function setLocalAndSendOffer(sessionDescription) {
  // Set Opus as the preferred codec in SDP if Opus is present.
  sessionDescription.sdp = kr.rtc.preferOpus(sessionDescription.sdp);
  var callee_name = this.name;
  var cmd = '{"rtc":"call","name":"' + callee_name + '","sdp":"'
   + sessionDescription.sdp + '"}';
  console.log('setLocalAndSendMessage sending message', cmd);
  for (var i = 0; i < kr.rtc.peers.length; i++) {
    if (kr.rtc.peers[i].name == callee_name) {
      kr.rtc.peers[i].connection.setLocalDescription(sessionDescription);
      kr.rtc.peers[i].readyToProcessCandidates = true;
      kr.rtc.peers[i].processQueuedCandidates();
     }
  }
  kr.rtc.sendMessage(cmd);
}

function setLocalAndSendAnswer(sessionDescription) {
  // Set Opus as the preferred codec in SDP if Opus is present.
  sessionDescription.sdp = kr.rtc.preferOpus(sessionDescription.sdp);
  var callee_name = this.name;
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
  var divName = 'local_stream_div_' + numLocalStreams;
  var canvasId = divName + '_spectrum_canvas';
  var streamId = 'stream_' + (numLocalStreams + 1);
  var streamName = 'Stream ' + (numLocalStreams + 1);
  var localStream = new LocalStream(streamName, streamId, stream,
   kr.rtc.localVideosContainer, divName, canvasId);
  kr.rtc.localStreams.push(localStream);
  var sBox = new SpectrumBox(2048, 64, canvasId, kr.rtc.audio_context,
   SpectrumBox.Types.FREQUENCY);
  var ctx = sBox.getCanvasContext();
  ctx.fillStyle = 'rgb(50,100,200)';
  var microphone = kr.rtc.audio_context.createMediaStreamSource(stream);
  microphone.connect(sBox.getAudioNode());
  sBox.enable();
  $('#' + divName).draggable();
  for (var i = 0; i < kr.rtc.peers.length; i++) {
    if (kr.rtc.peers[i].name != kr.rtc.name) {
      var button_id = "call_" + kr.rtc.peers[i].name + "_with_"
         + streamId;
      $('#' + kr.rtc.peers[i].name).append("<span class='krad_button' id='" + button_id + "'>Call with '"
       + localStream.name + "'</span>");
      $('#' + button_id).bind("click", {'name': kr.rtc.peers[i].name, 'stream':
        localStream.stream}, function(e) {
        kr.rtc.callPeer(e.data.name, e.data.stream);
      });
    }
  }
}

function handleUserMediaError(error){
  console.log('navigator.getUserMedia error: ', error);
}

function handleIceCandidate(event) {
  console.log('handleIceCandidate event: ', event);
  if (event.candidate) {
    var callee_name = this.name;
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
  /*Note: through the use of .bind, 'this' is the Peer who has added
    this remote stream.*/
  console.log('Remote stream added.');
  this.streams.push(event.stream);
  var streamId = 'remote_stream_' + this.name + '_' + this.streams.length;
  var remoteCanvasId = streamId + "_canvas";
  var group_id = streamId + '_group';

  var call_container_id = streamId + '_call_container'; 
  $('#remoteVideoContainer').append('<div id="' + call_container_id
   + '" class="call_container ' + group_id + '"><p class="remote_video_label">' + this.name
   + '</p><video id="' + streamId
   + '" class="remote_video ' + group_id + '" autoplay></video>'
   + '<canvas class="local_video_canvas ' + group_id + '" id="' + remoteCanvasId
   + '"></canvas></div>');

  $('.' + group_id).css('visibility', 'hidden');

  var sBox = new SpectrumBox(2048, 64, remoteCanvasId, kr.rtc.audio_context,
  SpectrumBox.Types.FREQUENCY);
  var ctx = sBox.getCanvasContext();
  ctx.fillStyle = 'rgb(50,100,200)';
  var microphone = kr.rtc.audio_context.createMediaStreamSource(
   event.stream);
  microphone.connect(sBox.getAudioNode());
  sBox.enable();

  $('#' + call_container_id).draggable({stack: ".call_container"});
  $('#' + call_container_id).resizable({ aspectRatio: true});

  for(var i = 0; i < kr.rtc.localStreams.length; i++) {
    var vidId = 'call_local_video_' + this.name + '_' + i;
    var local_video_label_id = 'local_video_label_' + this.name + '_' + i;
    var local_video_div_id = 'local_video_div_' + this.name + '_' + i;
    var canvasId = local_video_div_id + '_spectrum_canvas';

    $('#' + call_container_id).append('<div id="' + local_video_div_id
     + '" class="local_video_div"><p id="' + local_video_label_id + '" '
     + 'class="local_video_label">' + kr.rtc.name
     + '</p><video id="' + vidId + '" class="local_video" '
     + 'muted autoplay></video><canvas class="local_video_canvas" id="'
     + canvasId + '"></canvas></div>');

    var sBox = new SpectrumBox(2048, 64, canvasId, kr.rtc.audio_context,
     SpectrumBox.Types.FREQUENCY);
    var ctx = sBox.getCanvasContext();
    ctx.fillStyle = 'rgb(50,100,200)';
    var microphone = kr.rtc.audio_context.createMediaStreamSource(
     kr.rtc.localStreams[i].stream);
    microphone.connect(sBox.getAudioNode());
    sBox.enable();
    $('#' + vidId).bind('play', function() {
      kr.rtc.handleRemoteVideoPlaying(group_id, streamId,
       call_container_id, local_video_div_id, vidId, canvasId);
    });

    var localVideoElem = document.querySelector('#' + vidId);
    $('#' + local_video_div_id).resizable({
      stack: '.local_video_div',
      aspectRatio: true,
      containment: '#' + call_container_id,
      stop: function(e, ui) {
        var w = $('#' + call_container_id).width();
        var h = $('#' + call_container_id).height();
        $(this).css('width', 100.0*$(this).width()/w + '%');
        $(this).css('height', 100.0*$(this).height()/h + '%');
        $(this).css('top', 100.0*ui.position.top/h + '%');
        $(this).css('left', 100.0*ui.position.left/w + '%');

      }
    });

   attachMediaStream(localVideoElem, kr.rtc.localStreams[i].stream);
    $('#' + local_video_div_id).css('top', (6 + 40*i) + '%');


    $('#' + local_video_div_id).draggable({
      containment: '#' + call_container_id,
      stop: function(e, ui) {
        var w = $('#' + call_container_id).width();
        var h = $('#' + call_container_id).height();
        $(this).css('top', 100.0*ui.position.top/h + '%');
        $(this).css('left', 100.0*ui.position.left/w + '%');
      }
    });
  }

  var vidElement = document.querySelector('#' + streamId);
  attachMediaStream(vidElement, event.stream);
}

Rtc.prototype.handleRemoteVideoPlaying = function(groupId, streamId,
 call_container_id, local_video_div_id, vidId, canvasId) {
  var vid_height = $('#' + streamId).height();
  var vid_width = $('#' + streamId).width();
  var ratio = vid_height != 0 ? vid_width/vid_height : 0;

  if ((ratio != 0) && (ratio != 2)) {
    $('#' + call_container_id).css('height', $('#' + streamId).height());

    var w = $('#' + call_container_id).width();
    var h = $('#' + call_container_id).height();
    $('#' + local_video_div_id).css('height',
     100 * $('#' + vidId).height()/h + '%');
    w = $('#' + local_video_div_id).width();
    $('#' + vidId).css('width', 100 * $('#' + vidId).width()/w + '%');
    $('#' + canvasId).css('width', 100 * $('#' + vidId).width()/w + '%');
    $('.' + groupId).css('visibility', 'visible');
  } else {
    setTimeout(function() {
      kr.rtc.handleRemoteVideoPlaying(groupId, streamId,
       call_container_id, local_video_div_id, vidId, canvasId);
    }, 100);
  }
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
}

Rtc.prototype.stop = function() {
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
    console.log('Received call', crate);
    kr.rtc.ring();
    $('#dialog').html("Answer incoming call from '" + crate.user + "'?");
    $('#dialog').dialog({
      autoOpen: true,
      buttons: {
        Ok: function() {
          for (var i = 0; i < kr.rtc.peers.length; i++) {
            if (kr.rtc.peers[i].name == crate.user) {
              kr.rtc.peers[i].connection.setRemoteDescription(
               new RTCSessionDescription({"sdp":crate.sdp, "type":"offer"}));
              break;
            }
          }
          for (var i = 0; i < kr.rtc.peers.length; i++) {
            if (kr.rtc.peers[i].name == crate.user) {
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
        this.peers[i].handleIceCandidate(candidate);
      }
    }
  } else if (crate.message === 'bye') {
    this.handleRemoteHangup();
  }
}

Rtc.prototype.shared = function(key, shared_object) {
}






