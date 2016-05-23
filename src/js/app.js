var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
var style = 0;

/**
 * Created by eliaslipp on 03.05.16.
 */
/*window.onload = function () {
    var list = init("i13064", "hallolipp", "htlwrn");
    var time = getTime();
    var subject;
    for (var i = 0; i < list.length; i++) {
        if (list[i].endTime >= time) {
            subject = {name: list[i].name};
            subject.endHours = convertToTime(list[i].endTime, "h");
            subject.endMinutes = convertToTime(list[i].endTime, "m");
            subject.startHours = convertToTime(list[i].startTime, "h");
            subject.startMinutes = convertToTime(list[i].startTime, "m");
            //var diffTime = differenceOfTime(list[i].endTime, time);
            //subject.free = diffTime > 50;
            break;
        }
    }
    document.getElementById("main").innerHTML += "<br>Subject: " + subject.name + "<br>StartTime: " + subject.startHours + ":" + subject.startMinutes + "<br>EndTime:  " + subject.endHours + ":" + subject.endMinutes;
};*/

function init(user, password, school) {
    var sessionId;
    var date = getDate();

    sessionId = sendRequest("authenticate", {
        "user": user,
        "password": password,
        "client": "CLIENT"
    }, false).sessionId;

    var result = sendRequest("getTimetable", {
        "options": {
            "startDate": date,
            "endDate": date,
            "element": {
                "id": 291,
                "type": 1
            },
            "showStudentgroup": true,
            "showLsText": true,
            "showLsNumber": true,
            "showInfo": true,
            "klasseFields": ["id", "name", "longname", "externalkey"],
            "teacherFields": ["id", "name"]
        }
    }, true).sort(function (a, b) {
        if (a.startTime > b.startTime)
            return 1;
        else if (a.startTime < b.startTime)
            return -1;
        else
            return 0;
    });

    var subjects = sendRequest("getSubjects", {}, true);

    var subs = sendRequest("getSubstitutions", {
        "startDate": date,
        "endDate": date,
        "departmentId": 0
    }, true); //19 = Informatik

    var substitions = [];

    for (var i = 0; i < subs.length; i++) {
        if (subs[i].lstype != "oh" && subs[i].kl[0].name == "3BHIF") {
            substitions.push(subs[i]);
        }
    }

    sendRequest("logout", {}, true);

    result = changeToSubs(substitions, result);

    var array = [];

    for (var i = 0; i < result.length; i++) {
        for (var j = 0; j < subjects.length; j++) {
            if (subjects[j].id == result[i].su[0].id && usedSubject(subjects[j])) {
                array.push({
                    name: subjects[j].name + result[i].eliasText,
                    startTime: result[i].startTime,
                    endTime: result[i].endTime
                });
            }
        }
    }

    return array;

    function sendRequest(method, params, sendId) {
        var options = {
            "id": "ID",
            "method": method,
            "params": params,
            "jsonrpc": 2.0
        };

        var xhttp = new XMLHttpRequest();
        if (!sendId)
            xhttp.open("POST", "https://nete.webuntis.com/WebUntis/jsonrpc.do?school=" + school, false);
        else
            xhttp.open("POST", "https://nete.webuntis.com/WebUntis/jsonrpc.do" + ";jsessionid=" + sessionId + "?school=" + school, false);
        xhttp.setRequestHeader("Content-Type", "application/json-rpc;charset=UTF-8");
        xhttp.send(JSON.stringify(options));
        return JSON.parse(xhttp.responseText).result;
    }

    function getDate() {
        var date = (new Date()).getFullYear() + '';
        if ((new Date()).getMonth() + 1 < 10)
            date += '0' + ((new Date()).getMonth() + 1);
        else
            date += ((new Date()).getMonth() + 1);
        date += (new Date()).getDate();
        return date;
    }

    function changeToSubs(subs, subjects) {
        var k = [];
        var bool = false;
        for (var i = 0; i < subjects.length; i++) {
            for (var j = 0; j < subs.length; j++) {
                if (subs[j].startTime == subjects[i].startTime) {
                    switch (subs[j].type) {
                        case "cancel":
                            bool = true;
                            break;
                        case "add":
                            if (!contains(subs[j], k)) { //funkt vl nicht
                                subs[j].eliasText = "";
                                k.push(subs[j]);
                                bool = true;
                            }
                            break;
                        case "subst":
                            if (!contains(subjects[i], k)) { //funkt vl nicht
                                subjects[i].eliasText = " (sub)";
                                k.push(subjects[i]);
                                bool = true;
                            }
                            break;
                    }
                }
            }
            if (!bool && (subjects[i].sg == undefined || groups(subjects[i]))) {
                subjects[i].eliasText = "";
                k.push(subjects[i]);
            }
            bool = false;
        }
        return k;
    }

    function contains(elem, arr) {
        for (var i = 0; i < arr.length; i++) {
            if (elem.startTime == arr[i].startTime) {
                return true;
            }
        }
        return false;
    }

    function usedSubject(sub) {
        var unused = ["EUN", "ROB"];
        for (var i = 0; i < unused.length; i++) {
            if (unused[i] == sub.name) return false;
        }
        return true;
    }

    function groups(sub) {
        var groupList = ["NVS1_3BHIF_2", "NVS1_3BHIF", "GGP_3BHIF", "RK_3BHIF", "POS1_3BHIF", "DBI1_3BHIF", "DBI1_3BHIF_2", "POS1_3BHIF_2"];
        for (var i = 0; i < groupList.length; i++) {
            if (groupList[i] == sub.sg) return true;
        }
        return false;
    }
};

function convertToTime(endTime, type) {
    switch (type) {
        case "h":
            return Math.floor(endTime / 100);
        case "m":
            return Math.floor(endTime % 100);
    }
}

function getTime() {
    var h = (new Date()).getHours();
    var m = (new Date()).getMinutes();
    if (h < 10) h = '0' + h;
    if (m < 10) m = '0' + m;
    h += '';
    m += '';
    return h + m;
}

function differenceOfTime(time1, time2) {
    var difference = (Math.floor(time1 / 100) - Math.floor(time2 / 100) - 1) * 100;
    if (Math.floor(time1 % 100) >= Math.floor(time2 % 100)) {
        difference += Math.floor(time1 % 100) - Math.floor(time2 % 100);
    } else {
        difference += 60 - Math.floor(time2 % 100);
        difference += Math.floor(time1 % 100);
    }
    return difference;
}

function getUntil(){
  var list = init("i13064", "hallolipp", "htlwrn");
  var time = getTime();
  var subject;
  for (var i = 0; i < list.length; i++) {
      if (list[i].endTime >= time) {
          subject = {name: list[i].name};
          subject.endHours = convertToTime(list[i].endTime, "h");
          subject.endMinutes = convertToTime(list[i].endTime, "m");
          subject.startHours = convertToTime(list[i].startTime, "h");
          subject.startMinutes = convertToTime(list[i].startTime, "m");
          //var diffTime = differenceOfTime(list[i].endTime, time);
          //subject.free = diffTime > 50;
          break;
      }
  }
  return subject;
}

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
      var subject = getUntil();
      var hour;
      var minute;
      if (subject != null){
        var time = getTime();
        if (time < subject.startTime){
          hour = subject.startHours;
          minute = subject.startMinutes;
        }
        else {
          hour = subject.endHours;
          minute = subject.endMinutes;
        }
      }
      else {
        hour = -1;
        minute = -1;
      }
      dictionary = {
        'KEY_TEMPERATURE': temperature,
        'KEY_CONDITIONS': conditions,
        'KEY_HOURS_TO': hour,
        'KEY_MINUTES_TO': minute,
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
      'KEY_HOURS_TO': -1,
      'KEY_MINUTES_TO': -1,
      'KEY_SECONDS_STYLE': parseInt(configuration.KEY_SECONDS_STYLE)
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
