var exec = require("child_process").exec;

function start(query, response) {
  console.log("Request handler 'start' was called.");
  var content = "empty";

  exec("dir", function (error, stdout, stderr) {
    response.writeHead(200, {"Content-Type": "text/plain"});
    response.write(stdout);
    response.end();
  });

  return content;
}

function openSesame(query, response) {
  console.log("Request handler 'openSesame' was called.");
  allowed = ["q=123"
    ,"q=909"
    ,"q=100"
    ,"q=4B00DA30B4"
  ];
  if(allowed.indexOf(query) > -1){
    console.log("Request was for " + query + " granted.");
    response.writeHead(200, {"Content-Type": "text/plain"});
    response.write('{"access" : "granted"}');
  } else {
    console.log("Request was for " + query + " denied.");
    response.writeHead(401, {"Content-Type": "text/plain"});
    response.write('{"access" : "denied"}');
  }
  response.end();
}

exports.start = start;
exports.openSesame = openSesame;
