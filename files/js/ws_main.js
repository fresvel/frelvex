const doc_render={
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
const doc_state={ 
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
  for(let i=0;i<=ws_obj.load.tally;i++){
    if(ren_obj[i]===undefined){
      console.log("Error de comunicación")
      break
    }else{
      ren_obj.content+=ren_obj[i]
      delete ren_obj[i]
    }
  }


  if(ws_obj.src.object===""){
    ws_obj.src.object=ws_obj.src.module  
  }else{
    console.log("VALOR TRUE")
    console.log(ws_obj.src.object)
  }
  
  let html_el=document.getElementById(ws_obj.src.object)
  console.log(ws_obj.src.object)
  html_el.innerHTML=ren_obj.content
  console.log(doc_render)
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
    if(ws_obj.load.state==100)
      ws_renderPage(doc_render[ws_obj.src.module][ws_obj.src.object],ws_obj)
  }
}

function onOpen(event) {
    let ws_app={
      "ws-type":"ws-body", 
      "ws-info":"login/login.html",
      "ws-token":{}
    }

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
    let ws_msg=JSON.parse(event.data);
    if (ws_msg===null) {
      console.log("NOT IMPLEMENTED");
    }else if(ws_msg.method ==="render"){
      ws_renderFile(ws_msg)
    }else if(ws_msg.method ==="system"){
      console.log("SYSTEM");
    }else if(ws_msg.method ==="header"){
      ws_renderHeader(ws_msg.data)
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

  ws_configHeader()
  //here request de ccs elements
  let ws_req={
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

  let ws_script={
    "ws-sjs":"ota/test.js",
    "ws-djs":"ota/ota.js"
  }
  let ws_req={
    "ws-type":"ws-js",
    "ws-info":ws_script,
    "ws-token":{}
  }
  const json_req = JSON.stringify(ws_req)
  console.log("JSON script request sent");
  websocket.send(json_req)
  let ws_led=document.getElementById('ws-led');
  blinkLED(ws_led) 
}



/**
 * Manage the header in a diferent method in wich the user can decide 
 * wich elements of header is active and using a plain or o type of object
 * configures the elements of the header like names of the menu items and navbar
 */


function ws_configHeader(){

  let ws_header={
    "ws-ota":"ota/ota.json"
  }
  
  let ws_req={
    "ws-type":"ws-header",
    "ws-info":ws_header
  }

  let req_str=JSON.stringify(ws_req)
  websocket.send(req_str)
  console.log("Solicitud enviada")
  console.log(req_str)
}


function ws_renderHeader(hd_str){
  
  hd_obj=JSON.parse(hd_str)
  let title=document.getElementById("ws-title");
  title.innerHTML=hd_obj.title
  let menu = document.getElementById("hd-menu"); // Asumo que el menú tiene un ID "ws-menu"
  menu.innerHTML=""
  hd_obj.menu.forEach(el => {
    let item = document.createElement("a");
    item.className = "navbar-item is-size-5";
    item.id = `menu-${el}`;
    item.textContent = el;
    menu.appendChild(item);
  });
  let hd_brand=document.getElementById("hd-brand")
  hd_brand.className=""
  let brand_sty="button m-3 "+hd_obj.class
  hd_brand.className=brand_sty

  let hd_nav=document.getElementById("hd-nav")
  hd_nav.className=""
  hd_obj.class="navbar "+hd_obj.class
  hd_nav.classList=hd_obj.class

  let hd_logo=document.getElementById("hd-logo")
  let hd_led=document.getElementById("ws-led")
  hd_logo.hidden=hd_obj.logo
  hd_led.hidden=hd_obj.led
  hd_led.classList.add('led-green');
  blinkLED(hd_led);
}