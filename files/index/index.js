const doc_render={
  "ws-section":{
    "file":{
      "s1":{
        "html":"",//contenido del div
        "spot":"",//id del div y nombre del file
        "json":{} //parámetros del div
      }
    }
  },
  "ws-system":{},
  "ws-topic":{}
  }
const gateway = `wss://${window.location.host}/ws`;
let websocket;

window.addEventListener('load', initWebSocket);       
function initWebSocket() {
    console.log('Oppening a new WebSocket connection...');
    websocket = new WebSocket(gateway)
    websocket.onopen = onOpen
    websocket.onclose = onClose
    websocket.onmessage = mainMessage
    websocket.onerror = onError
}

function ws_callRender(param,owner="ws-body"){ ////The element does not exist
  let i=0

  let arr_files=[]
  param.files.forEach(item => {
    arr_files[i]=`${param.path}/${item}`
    let ext=item.split('.')[1]
    let id=`ws-${item.split('.')[0]}-${ext}`
    if(document.getElementById(id)===null){
      switch(ext){
        case "css":
          let new_css=document.createElement("style")
          new_css.id=id
          document.head.appendChild(new_css)
          alert("CSS ADDED")
          break;
        case "js":
          let new_js=document.createElement("script")
          new_js.id=id
          document.body.appendChild(new_js)
          alert("JS ADDED")
          break;
        case "html":
          
          break;
        default:
      }
    }else {
      alert("The element already exists")
    }
    i++
  })

  let ws_info={
    "array": arr_files,
    "owner": owner
  }

  let kind
  if(owner=="ws-body")
    kind="ws-topic"
  else
    kind="ws-section"

  let ws_req={
    "ws-method": kind,
    "ws-info": ws_info,
    "ws-token": {}
  }

  let ws_str=JSON.stringify(ws_req)
  websocket.send(ws_str)
}

function ws_joinFile(ws_obj){//module file object
  console.log("JOINING file")
  console.log(ws_obj)
  
  let path = ws_obj.src.file.split("/")
  let file =path[path.length-1].split(".")[0]
  let ext=path[path.length-1].split(".")[1]
  path=path[0]
  let module =ws_obj.src.module
  let part=ws_obj.load.tally

  if(doc_render[module][path]===undefined)
    doc_render[module][path]={}
  if(doc_render[module][path][file]===undefined)
    doc_render[module][path][file]={}
  doc_render[module][path][file][part]=ws_obj.data
 
  if(ws_obj.load.state==100){
    doc_render[module][path][file][ext]=""
    for(let i=0;i<=part;i++){
      if(doc_render[module][path][file][i]===undefined){
        alert("File is not completely loaded")
        break
      }else{
        doc_render[module][path][file][ext]+=doc_render[module][path][file][i]
        delete doc_render[module][path][file][i]
      }
    }
    doc_render[module][path][file].owner=ws_obj.src.owner
    
    let id=`ws-${file}-${ext}`
    if(ext=="html"){
      let new_el=document.createElement("div")
      new_el.id=id
      console.log("ID OWNER: " + ws_obj.src.owner)
      let html_ow=document.getElementById(ws_obj.src.owner)
      html_ow.innerHTML=""
      html_ow.appendChild(new_el)
    }if(ext=="json"){
      console.log(ws_obj)
      alert("JSON DETECTED!! ")
      let fn = `${path}_${file}_${ext}`;

      if (typeof window[fn] === 'function') {
        window[fn](doc_render[module][path][file][ext]); // Esto ejecutará la función path_json()
      } else {
        console.error("La variable no contiene el nombre de una función válida");
      }
      return

    }


    let html_el=document.getElementById(id)
    console.log(html_el)
    console.log(id)
    console.log(ws_obj.src.owner)
    html_el.innerHTML=doc_render[module][path][file][ext]
    console.log(doc_render)
  }


  console.log(`Element received, sending PING`)

}

 /*REVISAR LA FORMA DE SOBREESCRIBIR*/ 

/** Above functions are statics and
 * below functions are configured in 
 * dependencies of the actual module
 * Here is necessary to implement
 * a control of variables, checking
 * if they already exist
*/

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
  let hd_brand=document.getElementById("bd-menu")
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

function mainMessage(event) {
  let ws_msg=null;
  try{
    ws_msg=JSON.parse(event.data);
  }catch(e){
    console.error()
  }
  
  if (ws_msg===null) {
    console.log("OBJECT RECIVED IS NOT A JSON");
  }else if(ws_msg.method ==="render"){
    console.log("OBJECT RECIVED IS RENDER TYPE");
    ws_joinFile(ws_msg)
  }else if(ws_msg.method ==="system"){
    console.log("SYSTEM");
  }else if(ws_msg.method ==="header"){
    ws_renderHeader(ws_msg.data)
  }else{
    console.log("NOT IMPLEMENTED");
    console.log(ws_msg)
  }

  onMessage(event)

}

function onOpen(event) {
    
  let ws_led=document.getElementById('ws-led');
  //console.log(ws_led)
  if (ws_led!=null) {
    //console.log(ws_led)
    ws_led.classList.remove('led-red');
    ws_led.classList.add('led-green');
    blinkLED(ws_led);
  }
  console.log("Conectado")
  
  /**
  let ws_req={
    "path":"cebox",
    "files":["cebox.html"]
  }
  ws_callRender(ws_req)

  alert("WS SENDED!");

  let sc_req={
    "path":"login",
    "files":["login.html","login.js"]
  }
  ws_callRender(sc_req,"ws-cb-form")   
   */

  let ws_req={
    "path":"board",
    "files":["board.html","panel.js","board.json"]
  }
  ws_callRender(ws_req)


}

function onError(event) {
  let ws_led=document.getElementById('ws-led');
  if (ws_led!=null) {
  ws_led.classList.remove('led-green');
  ws_led.classList.add('led-red');
  blinkLED(ws_led);
  }
  console.log('Connection error');
}

function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
}

function blinkLED(button) {
  button.classList.add('blink');
  setTimeout(() => {
  button.classList.remove('blink');
  }, 10000);
}

function onMessage(ws_msg){

}


