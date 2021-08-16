function gasWarning() {
  var text = $("#gas").text()
  text.trim();

  var warning = parseInt(text);

  if(warning >= 200) {
    $(".window").css('visibility',"visible")
    $("#inputdata").text('GAS ISSUE!');
  }
  else {
    $(".window").css('visibility',"hidden")
    $("#inputdata").text("");
  }
}

setInterval(function() {
  gasWarning();
  $('.content').load(document.URL +  ' .content .border');
} , 5000);



$(document).ready(function() {

  /* 새로고침시 스크롤 영역이 유지됨에 따라 발생하는 문제를 제거하기 위함입니다 */
  
  $(document).on('mouseover' , '.relay_button1' , function() {
    $(".relay_button1").css('background-color' , '#fff');
    $(".relay_button1").css('color' , '#000');    
  });

  $(document).on('mouseout' , '.relay_button1' , function() {
    $(".relay_button1").css('background-color' , '');
    $(".relay_button1").css('color' , '#fff');    
  });

  $(document).on('mouseover' , '.relay_button2' , function() {
    $(".relay_button2").css('background-color' , '#fff');
    $(".relay_button2").css('color' , '#000');    
  });

  $(document).on('mouseout' , '.relay_button2' , function() {
    $(".relay_button2").css('background-color' , '');
    $(".relay_button2").css('color' , '#fff');    
  });

  $(document).on('mouseover' , '.relay_button3' , function() {
    $(".relay_button3").css('background-color' , '#fff');
    $(".relay_button3").css('color' , '#000');    
  });

  $(document).on('mouseout' , '.relay_button3' , function() {
    $(".relay_button3").css('background-color' , '');
    $(".relay_button3").css('color' , '#fff');    
  });

  $(document).on('mouseover' , '.relay_button4' , function() {
    $(".relay_button4").css('background-color' , '#fff');
    $(".relay_button4").css('color' , '#000');    
  });

  $(document).on('mouseout' , '.relay_button4' , function() {
    $(".relay_button4").css('background-color' , '');
    $(".relay_button4").css('color' , '#fff');    
  });

  $(document).on('click' , '#fan_on' , function() {
    $.ajax({
      type : "GET",
      url : "?fan=on"
      }).done(function(data) {
      });  
  });

  $(document).on('click' , '#fan_off' , function() {
    $.ajax({
      type : "GET",
      url : "?fan=off"
      }).done(function(data) {
      });  
  });

  $(document).on('click' , '#bulb_on' , function() {
    $.ajax({
      type : "GET",
      url : "?bulb=on"
      }).done(function(data) {
      });  
  });

  $(document).on('click' , '#bulb_off' , function() {
    $.ajax({
      type : "GET",
      url : "?bulb=off"
      }).done(function(data) {
      });  
  });
  $('#fan_on').click(function () {
    
    
  });
});




