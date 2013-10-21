$(document).ready(function() {
  rack_units.push({'constructor': Pager, 'aspect': [32,1], 'page': 'all'});
});

function Pager(info_object) {
  this.title = "Pager";
  this.description = "Multiple rack pages, sir";
  this.aspect_ratio = [32,1];
  this.x = 0;
  this.y = 0;
  this.width = 0;
  this.height = 0;
  this.address_masks = [];
  this.div_text = "<div id='pager' class='orangey RCU pager'></div>";
  this.sel = "#pager";
  info_object['parent_div'].append(this.div_text);

  $(this.sel).append("<span class='page' id='page1'>Page 1</span><span class='page' id='page2'>Page 2</span>");

  $('.page').bind("click", function(e) {
    e.preventDefault();
    $('.page').removeClass('selected');
    $(this).addClass('selected');
    var id = $(this).attr('id');
    var num = id.match(/\d+/);
    num--;
    kr.ux.display_page(num, 500);
  });

  $(window).resize(function() {
    $('.page').css('font-size', 0.8*$('#pager').height()+'px');
  });
}

Rtc.prototype.update = function(crate) {
}

Rtc.prototype.shared = function(key, shared_object) {
}


