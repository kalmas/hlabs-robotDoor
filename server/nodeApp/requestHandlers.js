var exec = require("child_process").exec;

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
  response.write("I'll think about it.");
  response.end();
}

exports.start = start;
exports.openSesame = openSesame;