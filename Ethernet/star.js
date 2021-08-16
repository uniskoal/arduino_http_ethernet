/**
 * quietflow.js
 * Paul Krishnamurthy 2016
 *
 * https://paulkr.com
 * paul@paulkr.com
 */



function randCol (r, g, b, a) {
  return "rgba(" + Math.floor(Math.random() * r).toString() + "," +
                   Math.floor(Math.random() * g).toString() + "," +
                   Math.floor(Math.random() * b).toString() + "," + a + ")";
}

$.fn.quietflow = function (attributes) {

  // Cache node
  var $element = $(this);
  var $limitX  = $element.width();
  var $limitY  = $element.height() * 5;



  var centerX  = $limitX/2;
  var centerY  = $limitY/2;

  // Remove quietflow
  $("#Quietflow").remove();

  var theme   = "starfield";
  var z_index = -1000;

  var effectNames = [
    "squareFlash",
    "vortex",
    "bouncingBalls",
    "shootingLines",
    "simpleGradient",
    "starfield",
    "layeredTriangles",
    "cornerSpikes",
    "floatingBoxes"
  ];

  // Theme defaults
  var defaults = {
    starfield : {
      starColor : "#FFF",
      starSize  : 4,
      speed     : 1
    }
  };

  // Create canvas and set attributes
  var canvas            = document.createElement("canvas");
  var ctx               = canvas.getContext("2d");
  canvas.id             = "Quietflow";
  canvas.width          = $limitX;
  canvas.height         = $limitY;
  canvas.style.zIndex   = z_index;
  canvas.style.position = "absolute";
  canvas.style.top      = 0;

  // Attach canvas to element
  var $checkValidID = $element.attr("id");

  if (!($checkValidID == undefined)){
    var appendObject = document.getElementById($checkValidID);
    appendObject.appendChild(canvas);
  } else {
    document.body.appendChild(canvas);
  }

  // Set theme
  if ($.inArray(attributes.theme, effectNames) > -1) {
    theme = attributes.theme;
  }

  var effectAttrs = {};
  effectAttrs     = $.extend(defaults[theme], attributes);

  // Update canvas on resize without clearing
  $(window).resize(function () {

    $limitX = $element.width();
    $limitY = $element.height() * 5;

    var oldWidth  =  $("#Quietflow").css("width").replace("px", "");
    var oldHeight = $("#Quietflow").css("height").replace("px", "");

    $("#Quietflow").css({
      "width" : $limitX,
      "height": $limitY
    });
    
    var ratio1 = oldWidth / $limitX;
    var ratio2 = oldHeight / $limitY;

    ctx.scale(ratio1, ratio2);
  });



  // Render based on interval or automatically
  function render (callback) {

    if (effectAttrs.speed !== undefined) {
      setTimeout(function () {
        id = requestAnimationFrame(callback);
      }, effectAttrs.speed);
    } else {
      id = requestAnimationFrame(callback);
    }
  }

  // Effect animations




  function starfieldEffect () {

    var gradient = ctx.createLinearGradient(0, 0, $limitX / 2, $limitY);
    gradient.addColorStop(0, "#000");
    gradient.addColorStop(1, "#000");

    ctx.fillStyle = gradient;
    ctx.fillRect(0, 0, $limitX, $limitY);
    
    for (var i = 0; i < starData.length; i++) {

      var currentStar = starData[i],
          X           = 0,
          Y           = 1,
          RADIUS      = 2,
          SPEED       = 3;

      currentStar[Y] += currentStar[SPEED];
      currentStar[X] += currentStar[SPEED];

      ctx.beginPath();
      ctx.fillStyle = effectAttrs.starColor;
      ctx.arc(currentStar[X], currentStar[Y], currentStar[RADIUS], 0, Math.PI * 2, true);
      ctx.shadowColor = "#FFF";
      ctx.shadowBlur = 20;
      ctx.closePath();
      ctx.fill();

      // Create new star
      if (currentStar[Y] > $limitY) {
        starData.splice(i, 1);
        starData.unshift([Math.random() * $limitX,
                          Math.random() * $limitY/4 - $limitY/4,
                          Math.random() * effectAttrs.starSize,
                          Math.ceil(Math.random() * 3)]);
      }

      if (currentStar[X] > $limitX) {
        starData.splice(i, 1);
        starData.unshift([Math.random() * $limitX/4 - $limitX/4,
                          Math.random() * $limitY,
                          Math.random() * effectAttrs.starSize,
                          Math.ceil(Math.random() * 3)]);
      }

    }

    render(starfieldEffect);
  }





  // Effects
  switch (theme) {
    case "starfield":

      var starData = [];

      for (var i = 0; i < 1500; i++){
        starData.push([Math.random() * $limitX * 2 - $limitX,
                       Math.random() * $limitY,
                       Math.random() * effectAttrs.starSize,
                       Math.ceil(Math.random() * 5)]);
      }

      starfieldEffect();
      break;
    };

}
