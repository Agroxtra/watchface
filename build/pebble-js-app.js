var __loader = (function() {

var loader = {};

loader.packages = {};

loader.packagesLinenoOrder = [{ filename: 'loader.js', lineno: 0 }];

loader.fileExts = ['?', '?.js', '?.json'];
loader.folderExts = ['?/index.js', '?/index.json'];
loader.builtins = ['safe'];

loader.basepath = function(path) {
  return path.replace(/[^\/]*$/, '');
};

loader.joinpath = function() {
  var result = arguments[0];
  for (var i = 1; i < arguments.length; ++i) {
    if (arguments[i][0] === '/') {
      result = arguments[i];
    } else {
      result += '/' + arguments[i];
    }
  }

  if (result[0] === '/') {
    result = result.substr(1);
  }
  return result;
};

var replace = function(a, regexp, b) {
  var z;
  do {
    z = a;
  } while (z !== (a = a.replace(regexp, b)));
  return z;
};

loader.normalize = function(path) {
  path = replace(path, /(?:(^|\/)\.?\/)+/g, '$1');
  path = replace(path, /[^\/]*\/\.\.\//, '');
  path = path.replace(/\/\/+/g, '/');
  path = path.replace(/^\//, '');
  return path;
};

function _require(module) {
  if (module.exports) {
    return module.exports;
  }

  var require = function(path) { return loader.require(path, module); };

  module.exports = {};
  module.loader(module.exports, module, require);
  module.loaded = true;

  return module.exports;
}

loader.require = function(path, requirer) {
  var module = loader.getPackage(path, requirer);
  if (!module) {
    throw new Error("Cannot find module '" + path + "'");
  }

  return _require(module);
};

var compareLineno = function(a, b) { return a.lineno - b.lineno; };

loader.define = function(path, lineno, loadfun) {
  var module = {
    filename: path,
    lineno: lineno,
    loader: loadfun,
  };

  loader.packages[path] = module;
  loader.packagesLinenoOrder.push(module);
  loader.packagesLinenoOrder.sort(compareLineno);
};

loader.getPackageForPath = function(path) {
  return loader.getPackageForFile(path) || loader.getPackageForDirectory(path);
};

loader.getPackage = function(path, requirer) {
  var module;
  var fullPath;
  if (requirer && requirer.filename) {
    fullPath = loader.joinpath(loader.basepath(requirer.filename), path);
  } else {
    fullPath = path;
  }

  if (loader.builtins.indexOf(path) !== -1) {
    return loader.packages[path];
  }

  // Try loading the module from a path, if it is trying to load from a path.
  if (path.substr(0, 2) === './' || path.substr(0, 1) === '/' || path.substr(0, 3) === '../') {
    module = loader.getPackageForPath(fullPath);
  }

  if (!module) {
    module = loader.getPackageFromBuildOutput(path);
  }

  if (!module) {
    module = loader.getPackageForNodeModule(path);
  }

  return module;
};

loader.getPackageForFile = function(path) {
  path = loader.normalize(path);

  var module;
  var fileExts = loader.fileExts;
  for (var i = 0, ii = fileExts.length; !module && i < ii; ++i) {
    var filepath = fileExts[i].replace('?', path);
    module = loader.packages[filepath];
  }

  return module;
};

loader.getPackageForDirectory = function(path) {
  path = loader.normalize(path);

  var module;
  var packagePackage = loader.packages[loader.joinpath(path, 'package.json')];
  if (packagePackage) {
    var info = _require(packagePackage);
    if (info.main) {
      module = loader.getPackageForFile(loader.joinpath(path, info.main));
    }
  }

  if (!module) {
    module = loader.getPackageForFile(loader.joinpath(path, 'index'));
  }

  return module;
};

loader.getPackageFromBuildOutput = function(path) {
  var moduleBuildPath = loader.normalize(loader.joinpath('build', 'js', path));

  return loader.getPackageForPath(moduleBuildPath);
};

// Nested node_modules are banned, so we can do a simple search here.
loader.getPackageForNodeModule = function(path) {
  var modulePath = loader.normalize(loader.joinpath('node_modules', path));

  return loader.getPackageForPath(modulePath);
};

loader.getPackageByLineno = function(lineno) {
  var packages = loader.packagesLinenoOrder;
  var module;
  for (var i = 0, ii = packages.length; i < ii; ++i) {
    var next = packages[i];
    if (next.lineno > lineno) {
      break;
    }
    module = next;
  }
  return module;
};

return loader;

})();

__loader.define('safe', 181, function(exports, module, require) {
/* safe.js - Building a safer world for Pebble.JS Developers
 *
 * This library provides wrapper around all the asynchronous handlers that developers
 * have access to so that error messages are caught and displayed nicely in the pebble tool
 * console.
 */

/* global __loader */

var safe = {};

/* The name of the concatenated file to translate */
safe.translateName = 'pebble-js-app.js';

safe.indent = '    ';

/* Translates a source line position to the originating file */
safe.translatePos = function(name, lineno, colno) {
  if (name === safe.translateName) {
    var pkg = __loader.getPackageByLineno(lineno);
    if (pkg) {
      name = pkg.filename;
      lineno -= pkg.lineno;
    }
  }
  return name + ':' + lineno + ':' + colno;
};

var makeTranslateStack = function(stackLineRegExp, translateLine) {
  return function(stack, level) {
    var lines = stack.split('\n');
    var firstStackLine = -1;
    for (var i = lines.length - 1; i >= 0; --i) {
      var m = lines[i].match(stackLineRegExp);
      if (!m) {
        continue;
      }
      var line = lines[i] = translateLine.apply(this, m);
      if (line) {
        firstStackLine = i;
        if (line.indexOf(module.filename) !== -1) {
          lines.splice(i, 1);
        }
      } else {
        lines.splice(i, lines.length - i);
      }
    }
    if (firstStackLine > -1) {
      lines.splice(firstStackLine, level);
    }
    return lines;
  };
};

/* Translates a node style stack trace line */
var translateLineV8 = function(line, msg, scope, name, lineno, colno) {
  var pos = safe.translatePos(name, lineno, colno);
  return msg + (scope ? ' ' + scope + ' (' + pos + ')' : pos);
};

/* Matches <msg> (<scope> '(')? <name> ':' <lineno> ':' <colno> ')'? */
var stackLineRegExpV8 = /(.+?)(?:\s+([^\s]+)\s+\()?([^\s@:]+):(\d+):(\d+)\)?/;

safe.translateStackV8 = makeTranslateStack(stackLineRegExpV8, translateLineV8);

/* Translates an iOS stack trace line to node style */
var translateLineIOS = function(line, scope, name, lineno, colno) {
  var pos = safe.translatePos(name, lineno, colno);
  return safe.indent + 'at ' + (scope ? scope  + ' (' + pos + ')' : pos);
};

/* Matches (<scope> '@' )? <name> ':' <lineno> ':' <colno> */
var stackLineRegExpIOS = /(?:([^\s@]+)@)?([^\s@:]+):(\d+):(\d+)/;

safe.translateStackIOS = makeTranslateStack(stackLineRegExpIOS, translateLineIOS);

/* Translates an Android stack trace line to node style */
var translateLineAndroid = function(line, msg, scope, name, lineno, colno) {
  if (name !== 'jskit_startup.js') {
    return translateLineV8(line, msg, scope, name, lineno, colno);
  }
};

/* Matches <msg> <scope> '('? filepath <name> ':' <lineno> ':' <colno> ')'? */
var stackLineRegExpAndroid = /^(.*?)(?:\s+([^\s]+)\s+\()?[^\s\(]*?([^\/]*?):(\d+):(\d+)\)?/;

safe.translateStackAndroid = makeTranslateStack(stackLineRegExpAndroid, translateLineAndroid);

/* Translates a stack trace to the originating files */
safe.translateStack = function(stack, level) {
  level = level || 0;
  if (Pebble.platform === 'pypkjs') {
    return safe.translateStackV8(stack, level);
  } else if (stack.match('com.getpebble.android')) {
    return safe.translateStackAndroid(stack, level);
  } else {
    return safe.translateStackIOS(stack, level);
  }
};

var normalizeIndent = function(lines, pos) {
  pos = pos || 0;
  var label = lines[pos].match(/^[^\s]* /);
  if (label) {
    var indent = label[0].replace(/./g, ' ');
    for (var i = pos + 1, ii = lines.length; i < ii; i++) {
      lines[i] = lines[i].replace(/^\t/, indent);
    }
  }
  return lines;
};

safe.translateError = function(err, intro, level) {
  var name = err.name;
  var message = err.message || err.toString();
  var stack = err.stack;
  var result = [intro || 'JavaScript Error:'];
  if (message && (!stack || stack.indexOf(message) === -1)) {
    if (name && message.indexOf(name + ':') === -1) {
      message = name + ': ' + message;
    }
    result.push(message);
  }
  if (stack) {
    Array.prototype.push.apply(result, safe.translateStack(stack, level));
  }
  return normalizeIndent(result, 1).join('\n');
};

/* Dumps error messages to the console. */
safe.dumpError = function(err, intro, level) {
  if (typeof err === 'object') {
    console.log(safe.translateError(err, intro, level));
  } else {
    console.log('Error: dumpError argument is not an object');
  }
};

/* Logs runtime warnings to the console. */
safe.warn = function(message, level, name) {
  var err = new Error(message);
  err.name = name || 'Warning';
  safe.dumpError(err, 'Warning:', 1);
};

/* Takes a function and return a new function with a call to it wrapped in a try/catch statement */
safe.protect = function(fn) {
  return fn ? function() {
    try {
      fn.apply(this, arguments);
    } catch (err) {
      safe.dumpError(err);
    }
  } : undefined;
};

/* Wrap event handlers added by Pebble.addEventListener */
var pblAddEventListener = Pebble.addEventListener;
Pebble.addEventListener = function(eventName, eventCallback) {
  pblAddEventListener.call(this, eventName, safe.protect(eventCallback));
};

var pblSendMessage = Pebble.sendAppMessage;
Pebble.sendAppMessage = function(message, success, failure) {
  return pblSendMessage.call(this, message, safe.protect(success), safe.protect(failure));
};

/* Wrap setTimeout and setInterval */
var originalSetTimeout = setTimeout;
window.setTimeout = function(callback, delay) {
  if (safe.warnSetTimeoutNotFunction !== false && typeof callback !== 'function') {
    safe.warn('setTimeout was called with a `' + (typeof callback) + '` type. ' +
              'Did you mean to pass a function?');
    safe.warnSetTimeoutNotFunction = false;
  }
  return originalSetTimeout(safe.protect(callback), delay);
};

var originalSetInterval = setInterval;
window.setInterval = function(callback, delay) {
  if (safe.warnSetIntervalNotFunction !== false && typeof callback !== 'function') {
    safe.warn('setInterval was called with a `' + (typeof callback) + '` type. ' +
              'Did you mean to pass a function?');
    safe.warnSetIntervalNotFunction = false;
  }
  return originalSetInterval(safe.protect(callback), delay);
};

/* Wrap the geolocation API Callbacks */
var watchPosition = navigator.geolocation.watchPosition;
navigator.geolocation.watchPosition = function(success, error, options) {
  return watchPosition.call(this, safe.protect(success), safe.protect(error), options);
};

var getCurrentPosition = navigator.geolocation.getCurrentPosition;
navigator.geolocation.getCurrentPosition = function(success, error, options) {
  return getCurrentPosition.call(this, safe.protect(success), safe.protect(error), options);
};

var ajax;

/* Try to load the ajax library if available and silently fail if it is not found. */
try {
  ajax = require('ajax');
} catch (err) {}

/* Wrap the success and failure callback of the ajax library */
if (ajax) {
  ajax.onHandler = function(eventName, callback) {
    return safe.protect(callback);
  };
}

module.exports = safe;
});
__loader.define('src/js/app.js', 397, function(exports, module, require) {
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
var style = 0;
var user;
var pwd;
var school;

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

        if ((new Date()).getDate() < 10)
            date += '0' + (new Date()).getDate();
        else
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
                        case "shift":
                            if (!contains(subjects[i], k)){
                              subs[j].eliasText = "";
                              k.push(subs[j]);
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
  var list = init(localStorage.KEY_UNTIS_USER, localStorage.KEY_UNTIS_PWD, localStorage.KEY_UNTIS_SCHOOL);
  var time = getTime();
  var subject;
  for (var i = 0; i < list.length; i++) {
      if (list[i].endTime >= time) {
          subject = {name: list[i].name};
          subject.endHours = convertToTime(list[i].endTime, "h");
          subject.endMinutes = convertToTime(list[i].endTime, "m");
          subject.startHours = convertToTime(list[i].startTime, "h");
          subject.startMinutes = convertToTime(list[i].startTime, "m");
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
        var starttime = (subject.startHours*100) + subject.startMinutes
        console.log(time + ' ' + starttime);
        if (time < starttime){
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
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
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

    user = configuration.KEY_UNTIS_USER;
    pwd = configuration.KEY_UNTIS_PWD;
    school = configuration.KEY_UNTIS_SCHOOL;
    localStorage.KEY_UNTIS_USER = user;
    localStorage.KEY_UNTIS_PWD = pwd;
    localStorage.KEY_UNTIS_SCHOOL = school;


    //console.log(localStorage.KEY_UNTIS_USER + ' ' + localStorage.KEY_UNTIS_PWD);

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
});
__loader.define('build/js/message_keys.json', 796, function(exports, module, require) {
module.exports = {
    "KEY_CONDITIONS": 1,
    "KEY_HOURS_TO": 2,
    "KEY_MINUTES_TO": 3,
    "KEY_SECONDS_STYLE": 4,
    "KEY_TEMPERATURE": 0
};
});
(function() {
  var safe = __loader.require('safe');
  safe.protect(function() {
    __loader.require('/src/js/app');
  })();
})();