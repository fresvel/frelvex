ws_header={} //Only object    
ws_body={}   //{}[]""   
ws_lib_css={}  
ws_lib_js={}   
ws_footer={}    
ws_system={}    

ws_app={
  "ws-type":"ws-body", 
  "ws-info":"login/login.html",
  "ws-token":{}//return an auth method and data for login page(not implemented yet)
}

doc_render={
    "ws-header":{"content":""},
    "ws-body":{"content":""},
    "ws-section":{},
    "ws-footer":{"content":""},
    "ws-system":{"content":""},
    "ws-js":{"content":""},
    "ws-css":{"content":""},
};

/**
 * Values for doc state: 
 *  -1  default init value
 *  0   reset after load file, ready for reload and is saved in the browser
 *  1   loading file
 *  2   static element, this will not be reload and is saved in the browser
 */
doc_state={ 
    "ws-header":-1,
    "ws-body":-1,
    "ws-section":-1,
    "ws-footer":-1,
    "ws-system":-1,
    "ws-js":-1,
    "ws-css":-1
}

const gateway = `wss://${window.location.host}/ws`;
let websocket;

window.addEventListener('load', initWebSocket);       
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway)
    websocket.onopen = onOpen
    websocket.onclose = onClose
    websocket.onmessage = mainMessage
    websocket.onerror = onError
}


function ws_renderPage(ren_obj,ws_obj) {
  ren_obj.content="" //Crear función para validar si es necesario o permitido borrar o modificar lo existente
  console.log(ws_obj)
  for(let i=0;i<=ws_obj.load.tally;i++){
    if(ren_obj[i]===undefined){
      console.log("Error de comunicación")
      break
    }else{
      ren_obj.content+=ren_obj[i]
      delete ren_obj[i]
    }
  }

  console.log("VALOR DE MODULE")
  console.log(ws_obj.src.module)
  if(ws_obj.src.object===""){
    ws_obj.src.object=ws_obj.src.module
    console.log("VALOR TRUE")
    console.log(ws_obj.src.object)  
  }else{
    console.log("VALOR TRUE")
    console.log(ws_obj.src.object)
  }
  

  console.log("File received")
  console.log(doc_render)
  console.log(ws_obj)
  let html_el=document.getElementById(ws_obj.src.object)
  console.log(ws_obj.src.object)
  html_el.innerHTML=ren_obj.content
}

function ws_renderFile(ws_obj){

  if(ws_obj.src.object===""){
    doc_render[ws_obj.src.module][ws_obj.load.tally]=ws_obj.data
    if(ws_obj.load.state==100)
      ws_renderPage(doc_render[ws_obj.src.module],ws_obj)
  }else{
    if(doc_render[ws_obj.src.module][ws_obj.src.object]===undefined)
    doc_render[ws_obj.src.module][ws_obj.src.object]={}
    doc_render[ws_obj.src.module][ws_obj.src.object][ws_obj.load.tally]=ws_obj.data
    ws_renderPage(doc_render[ws_obj.src.module][ws_obj.src.object],ws_obj)
  }
}

function onOpen(event) {
    const json_app = JSON.stringify(ws_app)
    websocket.send(json_app)
    console.log("Sending request:", `${json_app}`)
    let ws_led=document.getElementById('ws-led');
    //console.log(ws_led)
    if (ws_led!=null) {
      //console.log(ws_led)
      ws_led.classList.remove('led-red');
      ws_led.classList.add('led-green');
      blinkLED(ws_led);
    }
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(ws_msg){

}

function mainMessage(event) {
    console.log(`On message: ${event.data}\n`);
    let ws_msg=JSON.parse(event.data);

    if (ws_msg===null) {
      console.log("NOT IMPLEMENTED");
    }else if(ws_msg.method ==="render"){
      ws_renderFile(ws_msg)
    }else if(ws_msg.method ==="system"){
      console.log("SYSTEM");
    }else if(ws_msg.method ==="header"){
      ws_configHeader(ws_msg)
    }else{
      console.log("NOT IMPLEMENTED");
    }

    onMessage(ws_msg)

} /*REVISAR LA FORMA DE SOBREESCRIBIR*/ 

function onError(event) {
    let ws_led=document.getElementById('ws-led');
    if (ws_led!=null) {
    ws_led.classList.remove('led-green');
    ws_led.classList.add('led-red');
    blinkLED(ws_led);
    }
    console.log('Connection error');
}

function blinkLED(button) {
    button.classList.add('blink');
    setTimeout(() => {
    button.classList.remove('blink');
    }, 10000);
}

/** Above functions are statics and
 * below functions are configured in 
 * dependencies of the actual module
 * Here is necessary to implement
 * a control of variables, checking
 * if they already exist
*/

function login_Btn(js_name){
  //here request de ccs elements
  ws_req={
    "ws-type":"ws-body",
    "ws-info":"ota/ota.html",
    "ws-token":{}
  }
  const json_req = JSON.stringify(ws_req)
  websocket.send(json_req)
  console.log("Body requested");
  ws_add_script(js_name)

}

function ws_add_script(js_name){
    
  let new_js=document.createElement("script")
  new_js.id="ws-sjs"
  let js = document.createElement("script")
  document.body.appendChild(new_js)
  js.id="ws-djs"
  document.body.appendChild(js)

  ws_script={
    "ws-sjs":"ota/test.js",
    "ws-djs":"ota/ota.js"
  }
  ws_req={
    "ws-type":"ws-js",
    "ws-info":ws_script,
    "ws-token":{}
  }
  const json_req = JSON.stringify(ws_req)
  console.log("JSON script request sent");
  websocket.send(json_req)
  let ws_led=document.getElementById('ws-led');
  blinkLED(ws_led) 
  console.log(json_req)
}



/**
 * Manage the header in a diferent method in wich the user can decide 
 * wich elements of header is active and using a plain or o type of object
 * configures the elements of the header like names of the menu items and navbar
 */

function ws_configHeader(ws_obj){

  ws_header={
    "ws-ota":"ota/ota.json" 
  }
  
  ws_req={
    "ws-type":"ws-header",
    "ws-info":ws_header
  }

}



function ws_renderHeader(ws_obj){
  //render the header in function of the object received from the server   
}