alert("file login.js added to client");
function login_Btn(){

alert("Llamar a OTA o función login")
let topic={
    "path":"ota",
    "files":["ota.js"]
}

ws_callTopic(topic)

let section={
    "path":"ota",
    "files":["ota.html"],
  }

  ws_callSection(section,"login")
  
}