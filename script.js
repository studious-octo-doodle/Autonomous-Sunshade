angular.module('tabApp', [])
  .controller('TabController', ['$scope', function($scope) {
    $scope.tab = 1;

    $scope.setTab = function(newTab){
      $scope.tab = newTab;
    };

    $scope.isSet = function(tabNum){
      return $scope.tab === tabNum;
    };
}]);

$(document).ready(function() { 
  function getIcons(icon) {
    switch(icon) {
      case "01d": return "day-sunny";
      case "02d": return "day-cloudy";
      case "03d": return "cloud";
      case "04d": return "cloudy";
      case "09d": return "showers";
      case "10d": return "day-rain";
      case "11d": return "thunderstorm";
      case "13d": return "snow";
      case "50d": return "fog";
      case "01n": return "night-clear";
      case "02n": return "night-alt-cloudy";
      case "03n": return "cloud";
      case "04n": return "cloudy";
      case "09n": return "showers";
      case "10n": return "night-alt-rain";
      case "11n": return "thunderstorm";
      case "13n": return "snow";
      case "50n": return "fog";
      default: return "alien";
      }
    }
  function getLocation() {
    if(navigator.geolocation) {
      navigator.geolocation.getCurrentPosition(showPosition);
    } else {
      console.log("Geolocation is not supported on this browser.");
    }
  }
  function showPosition(position) {
    if(position.coords.latitude !== undefined && position.coords.longitude !== undefined) {
      getWeather(position.coords.latitude, position.coords.longitude);
    } else {
      alert("weather not loading...");
    }
  }
  function getWeather(lat, lon) {
    var url = 'https://api.openweathermap.org/data/2.5/weather?lat=' + lat + '&lon=' + lon + '&appid=a798de8e1df640529fc3cdd0b5cab5fd';
    $.getJSON(url, function(data) {
      renderPage(data);
    });
  }
  function renderPage(data) {
    var tempCel = Math.round(data.main.temp - 272.15);
    $('#location').html(data.name);
    $('#temp').attr("data-temp", data.main.temp).attr("temp-type", "cel");
    $('#temp').html(tempCel + "&#8451;");
    renderBackground(tempCel);
    var iconString = "";
    for(var i=0;i<data.weather.length;i++) {
      iconString += "<i class='wi wi-" + getIcons(data.weather[i].icon) + "'></i>";
    }
    $('#weather-icons').html(iconString);
    console.log(data); 
  }
  function renderBackground(temp) {
    //cutoffs at 15 and 32 degrees celsius
    var imageUrl;
    if(temp >= 32) {
      //hot background
      tempName = 'hot';
    } else if(temp >15) {
      //warm background
      tempName = 'warm';
    } else if(temp <=15) {
      //cold background
      tempName = 'cold';
    }
  }
  getLocation(); 
  $("#change-temp").click(function() {
    var temp = $('#temp').attr("data-temp");
    var tempCel = Math.round(temp - 272.15);
    var tempFar = Math.round(tempCel*(9/5)+32);
    var currentType = $('#temp').attr("temp-type");
    if(currentType === "cel") {
      $('#temp').html(tempFar + "&#8457;").attr('temp-type', 'far');
      $(this).html('Switch to &#8451;');
    } else if(currentType === "far") {
      $('#temp').html(tempCel + '&#8451;').attr('temp-type', 'cel');
      $(this).html('Switch to &#8457;');
    }
  });
  $('#getWeather').click(function() {
    var zip = $('#zip').val();
    var url = "http://api.openweathermap.org/data/2.5/weather?zip=" + zip + ",us&appid=a798de8e1df640529fc3cdd0b5cab5fd";
    $.getJSON(url, function(data) {
      renderPage(data);
    });
  });
  $('#zip').bind('keypress', function(e) {
	  if(e.keyCode==13){
      var zip = $('#zip').val();
      var url = "http://api.openweathermap.org/data/2.5/weather?zip=" + zip + ",us";
      $.getJSON(url, function(data) {
        renderPage(data);
      });
    }
  });
});