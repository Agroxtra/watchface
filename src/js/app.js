var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
var style = 0;

function locationSuccess(pos) {
  // We will request the weather here
  var temperature = 99.9;
  var conditions;
  var dictionary;
  var myAPIKey = 'b9f2a2f7b0be28012d1ec9f295d9f69e';
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  xhrRequest(url, 'GET',
    function(responseText) {
      // responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      // Temperature in Kelvin requires adjustment
      temperature = Math.round(json.main.temp - 273.15);
      //console.log('Temperature is ' + temperature);

      // Conditions
      conditions = json.weather[0].main;
      //console.log('Conditions are ' + conditions);

      dictionary = {
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
        'KEY_SECONDS_STYLE': -1
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
      function(e) {
        //console.log('Weather info sent to Pebble successfully!');
      },
      function(e) {
        //console.log('Error sending weather info to Pebble!');
      }
      );
    }
  );



}

function getStyle(){
  var style = 0;
  // Send to the watchapp via AppMessage

  //dictionary = {
  return configData.KEY_SECONDS_STYLE;
  //return style;
}



function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    //console.log('PebbleKit JS ready!');
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log('AppMessage received!');

    getWeather();
  }
);

Pebble.addEventListener('showConfiguration',
  function(e) {
    Pebble.openURL('http://agroxtra.spieleckecker.com/pebble/watchface.html');
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));

    //Send to Pebble, persist there
    var dictionary = {
      'KEY_TEMPERATURE': -274,
      'KEY_CONDITIONS': -1,
      'KEY_SECONDS_STYLE': configuration.KEY_SECONDS_STYLE
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
    function(e) {
      //console.log('Weather info sent to Pebble successfully!');
    },
    function(e) {
      //console.log('Error sending weather info to Pebble!');
    }
    );
  }
);
