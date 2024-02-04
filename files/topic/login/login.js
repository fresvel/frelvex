alert("file login.js added to client");
function login_Btn(){

    let ws_req={
        "path":"ota",
        "files":["ota.html", "ota.js"],
      }

    ws_callRender(ws_req,"ws-form")
  
}