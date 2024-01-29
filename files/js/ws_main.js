ws_header={}    
ws_body={}      
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
    websocket.onmessage = onMessage
    websocket.onerror = onError
}

function ws_join_file(el){
  let ws_el=JSON.parse(el)
  if(ws_el.render===undefined){
    console.log("Function not allowed")
  }else {
    if(doc_state[ws_el.render]===undefined||doc_state[ws_el.render]===0){
      doc_render["ws-section"][ws_el.render]={}
      doc_state[ws_el.render]=1
    }
    if(doc_state[ws_el.render]==1){// Determines if save in doc_render or in ws-section mode
      doc_render["ws-section"][ws_el.render][ws_el.part]=ws_el.data
      if(ws_el.state==100){
        renderWebSocket(doc_render["ws-section"][ws_el.render], ws_el.part,ws_el.render)//It is send part cause of that variable stores the real size
        doc_state[ws_el.render]=0
      }
    }else{
      doc_render[ws_el.render][ws_el.part]=ws_el.data
      if(ws_el.state==100){
        renderWebSocket(doc_render[ws_el.render],ws_el.part, ws_el.render)
      }
    }
  }
}

function renderWebSocket(json_el, size, id_el){
  json_el.content="";//Adds a function to check doc state 
  for(let i=0;i<=size;i++){
    if(json_el[i]===undefined){
      console.log("Error de comunicación")
      break
    }else{
      json_el.content+=json_el[i]
      delete json_el[i]
    }
  }
  console.log("File received")
  console.log(doc_render);
  let html_el=document.getElementById(id_el)
  html_el.innerHTML=json_el.content
}

function onOpen(event) {
    const json_app = JSON.stringify(ws_app)
    websocket.send(json_app)
    console.log("Sending request:", `${json_app}`)
    let ws_led=document.getElementById('ws-led');
    console.log(ws_led)
    if (ws_led!=null) {
      console.log(ws_led)
      ws_led.classList.remove('led-red');
      ws_led.classList.add('led-green');
      blinkLED(ws_led);
    }
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let state;
    
    ws_join_file(event.data)

    console.log(`On message: ${event.data}\n`);
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

