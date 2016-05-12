
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
        'KEY_CONDITIONS': conditions
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
  dictionary = {
    'KEY_SECONDS_STYLE': style,
  };
  Pebble.sendAppMessage(dictionary,
    function(e){
      console.log('style sent successfully');
    },
    function(e){
      console.log('Error sending style!');
    }
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    //console.log('PebbleKit JS ready!');
    getStyle();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    //console.log('AppMessage received!');
    getStyle();
  }
);
Pebble.addEventListener('showConfiguration',
  function(e) {
    Pebble.openURL('http://agroxtra.spieleckecker.com/pebble/watchface.html');
  }
);
