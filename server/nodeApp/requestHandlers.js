var exec = require("child_process").exec;
var fs = require('fs');

function start(response) {
  console.log("Request handler 'start' was called.");
  var content = "empty";

  exec("dir", function (error, stdout, stderr) {
    response.writeHead(200, {"Content-Type": "text/plain"});
    response.write(stdout);
    response.end();
  });

  return content;
}

function openSesame(response) {
	console.log("Request handler 'openSesame' was called.");
	response.writeHead(200, {"Content-Type": "text/plain"});
  
 	fs.readFile('doors.js', function(err, data){
		if(err) {
			console.error("Could not open file: %s", err);
			process.exit(1);
		}
		var doors = JSON.parse(data).doors;
		console.log(findById(doors, 'front').status);
		response.writeHead(200, {"Content-Type": "text/plain"});
		response.write(findById(doors, 'front').status);
		response.end();
	});
	return;
}

function findById(source, id) {
    return source.filter(function( obj ) {
        return obj.id === id;
    })[ 0 ];
}

function setById(source, id) {
    return source.filter(function( obj ) {
        return obj.id === id;
    })[ 0 ];
}

exports.start = start;
exports.openSesame = openSesame;