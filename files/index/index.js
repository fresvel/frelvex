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



function ws_callSection(section,owner){ //The element already exists

  let sc_files=[]
  let i=0
  section.files.forEach(item => {
    sc_files[i]=`${section.path}/${item}`
    i++
  })

  let ws_info={
    "array": sc_files,
    "owner": owner
  }

  let ws_req={
    "ws-method": "ws-section",
    "ws-info": ws_info,
    "ws-token": {}
  }

  let ws_str=JSON.stringify(ws_req)
  websocket.send(ws_str)

}

function ws_renderSection(ren_obj, ws_msg){

let path = ws_msg.src.file.split("/")
let file =path[path.length-1]
file=file.split(".")[0]
let ext=file.split(".")[1]
path=path[0]



ren_obj[path]="" //Crear función para validar si es necesario o permitido borrar o modificar lo existente
  for(let i=0;i<=ws_obj.load.tally;i++){
    if(ren_obj[i]===undefined){
      alert("File is not completely loaded")
      break
    }else{
      ren_obj[ws_obj.src.object]+=ren_obj[i]
      delete ren_obj[i]
    }
  }
 
  let html_el=document.getElementById(`${ws_obj.src.file}-${ws_obj.src.object}`)
  console.log(ws_obj.src.object)
  html_el.innerHTML=ren_obj[ws_obj.src.object]
  console.log(doc_render)


  //RENDERIZAR EN FUNCIÓN DEL VALOR DE  FILE
  //CAMBIAR VARIABLES FILE Y OBJETC POR PATH Y FILE
  //INTEGRAR ESTA FUNCIÓN EN VEZ DE RENDER TOPIC
  
}
/////////////////////////////////////////

function ws_callRender(param,owner=""){ ////The element does not exist
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
          let pos_div
          if(owner =="")
            pos_div=document.getElementById("ws-body")
          else
            pos_div=document.getElementById(owner)
          pos_div.innerHTML=""
          let new_html=document.createElement("div")
          new_html.id=id
          pos_div.appendChild(new_html)
          alert("HTML ADDED")
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
  if(owner=="")
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
    let id=`ws-${file}-${ext}`
    switch(module){
      case "ws-section":
        if(ext=="json"){
          alert("Execute code for JSON param")
          return
        }
        doc_render[module][path][file].owner=ws_obj.src.owner
        break
      case "ws-topic":
      break
      default:
    }

    let html_el=document.getElementById(id)
    console.log(id)
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
  let ws_req={
    "path":"login",
    "files":["login.html", "login.js"],
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


