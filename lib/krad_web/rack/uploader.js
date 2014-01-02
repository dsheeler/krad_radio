$(document).ready(function() {
  rack_units.push({'constructor': Uploader, 'aspect': [16,10], 'page': 2});
});

function Uploader(info_object) {
  this.title = "Uploader";
  this.description = "For file uploading.";
  this.aspect_ratio = [32,20];
  this.x = 0;
  this.y = 1;
  this.width = 0;
  this.height = 0;
  this.address_masks = [];
  this.id = 'uploader'
  this.div_text = "<div class='RCU uploader ultrablue' id='" + this.id + "'></div>";
  this.sel = "#" + this.id;
  info_object['parent_div'].append(this.div_text);

  $(this.sel).append('\
   <div class="row">\
   <label for="fileToUpload">Select a File to Upload</label><br />\
   <div class="button_wrap"><div class="krad_button" id="proxyButton">\
   BROWSE</div></div>\
   <input type="file" name="fileToUpload"\
    id="fileToUpload" style="display:none" multiple="multiple"\
    onchange="fileSelected();"/>\
   <div class="row">\
   <div id="fileName"></div>\
   <div id="fileSize"></div>\
   <div id="fileType"></div></div> <div class="row">\
   <div class="button_wrap"><div class="krad_button"\
    onclick="uploadFile()">UPLOAD</div></div>\
   <div id="progressNumber"></div>');


   $('#proxyButton').bind("click", function(e) {
   fileElem = document.getElementById("fileToUpload");
   fileElem.click();

   e.preventDefault();
   return false;
  });

}

Uploader.prototype.update = function(crate) {
}

Uploader.prototype.shared = function(key, shared_object) {
}

function fileSelected() {
  var file = document.getElementById('fileToUpload').files[0];
  if (file) {
    var fileSize = 0;
    if (file.size > 1024 * 1024)
      fileSize = (Math.round(file.size * 100 / (1024 * 1024)) / 100).toString() + 'MB';
    else
      fileSize = (Math.round(file.size * 100 / 1024) / 100).toString() + 'KB';

    document.getElementById('fileName').innerHTML = 'Name: ' + file.name;
    document.getElementById('fileSize').innerHTML = 'Size: ' + fileSize;
    document.getElementById('fileType').innerHTML = 'Type: ' + file.type;
  }
}

    function uploadFile() {
        var fd = new FormData();
        fd.append("fileToUpload", document.getElementById('fileToUpload').files[0]);
        var xhr = new XMLHttpRequest();
        xhr.upload.addEventListener("progress", uploadProgress, false);
        xhr.addEventListener("load", uploadComplete, false);
        xhr.addEventListener("error", uploadFailed, false);
        xhr.addEventListener("abort", uploadCanceled, false);
        xhr.open("POST", "UploadMinimal.aspx");
        xhr.send(fd);
      }

      function uploadProgress(evt) {
        if (evt.lengthComputable) {
          var percentComplete = Math.round(evt.loaded * 100 / evt.total);
          document.getElementById('progressNumber').innerHTML = percentComplete.toString() + '%';
        }
        else {
          document.getElementById('progressNumber').innerHTML = 'unable to compute';
        }
      }

      function uploadComplete(evt) {
        /* This event is raised when the server send back a response */
        alert(evt.target.responseText);
      }

      function uploadFailed(evt) {
        alert("There was an error attempting to upload the file.");
      }

      function uploadCanceled(evt) {
        alert("The upload has been canceled by the user or the browser dropped the connection.");
      }
