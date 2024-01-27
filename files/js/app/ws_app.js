/*Each of the next elements are arrays wich contains the base paths to the requested files */
ws_header=["ws-navbar","ws-led"]    // Type of ws request that manages the header. Points to files/html/header
ws_body=["panel/panel.html"]        // Type of ws request that manages the body. Points to files/html/body
ws_lib_css=["bulma.css"]            // Type of ws request that manages the ccs components. Points to files/ccs
ws_lib_js=["ws-chartjs"]            // Type of ws request that manages the javascript components. Points to files/js
ws_footer=["panel/panel.html"]      // Type of ws request that manages the footer. Points to files/html/footer
ws_section=[]                       // Type of ws request that manages a section of body. Points to files/html/section
ws_system={}                        // This js object is used to create functions 
/** At the first load the browser get the basic elements which includes 
 * header, basic body, footer, ccs styles default and this javascript.
 * For every request you must provide a json object containing
 * a websocket request with the following structure:
 * ws_app={
 *        "ws-type":"name-function",
 *        "ws-info":ws_info_objet 
 * }
 * Only ws_system uses an object for the request, for others it is used 
 * an string array wich contains the name of the files pointing to a path
 * in files directory.     
*/

ws_app={
  "ws-type":"ws-body",
  "ws-info":["ota/ota.html"]
}


/* 
ws_system={
  "ws-type":"ws-system",
	"ws-info":{
    "ws-method":"ws-onload",
    "ws-request":{
        "ws-header":ws_header,
        "ws-body":ws_body,
        "ws-lib-js":ws_lib_js,
        "ws-lib-css":ws_lib_css
    }
  }	
};
*/

doc_header={}
doc_body={"content":""}
doc_section={"name":{}}
doc_footer={}
doc_system={}
doc_js={}
doc_css={}

const gateway = `wss://${window.location.host}/ws`;
let websocket;

window.addEventListener('load', onLoad);       
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway)
    websocket.onopen = onOpen
    websocket.onclose = onClose
    websocket.onmessage = onMessage// <-- add this line
    websocket.onerror = onError
}



function ws_concat_data(el){

  console.log("Contating Document WebSocket...")
  let ws_el=JSON.parse(el)
  console.log(ws_el.function)
  console.log("Before IFF...")
  if(ws_el.function=="ws-body"){
    console.log("Inside Document WebSocket...")
    doc_body[ws_el.part]=ws_el.data


    if(ws_el.state==100){
      for(let i=0;i<=ws_el.part;i++){
        if(doc_body[i]==='undefined'){
          console.log("Error de comunicación")
          break
        }else{
          doc_body.content+=doc_body[i]
          delete doc_body[i]
        }
      }
      console.log(doc_body.content);
      let html_el=document.getElementById(ws_el.function)
      html_el.innerHTML=doc_body.content
    }




    console.log(ws_el.part)
  }else {
      console.log("Unknown")
      console.log(el)
  }
  

}

function renderWebSocket(el){

  console.log("Rendering Document WebSocket...")
  let ht_el=document.getElementById(ws_el.function)
  ht_el.innerHTML=ws_el.data;
  console.log(`Including : ${ws_el.data}`)

}

function onOpen(event) {
    let ws_js=document.getElementById('ws-js')
    const ws_app_json = JSON.stringify(ws_app)
    

    websocket.send(ws_app_json)
    console.log("ws_app sended:", `${ws_app_json}`)
    
    




    ws_js.innerHTML=`let state=1;  
    function actualizarNombreArchivo() {
        var input = document.getElementById('archivoInput');
        var nombreArchivoSpan = document.getElementById('nombreArchivo');
        if (input.files.length > 0) {
            nombreArchivoSpan.textContent = input.files[0].name;
        } else {
            nombreArchivoSpan.textContent = 'Choose a file';
        }
    }

    function sendFile(){
      let input=document.getElementById("archivoInput");
      let file=input.files[0];
      let bar=document.getElementById("prbar")

      
      if (file){
        const chunk_size = 1024;
        let offset=0;
        websocket.send('a');

        function sendChunk(){
          if(state==1){
            const chunk=file.slice(offset,offset + chunk_size)
            websocket.send(chunk)
            offset += chunk_size
            if (offset < file.size){
              setTimeout(sendChunk,3)
              console.log("sending file chunk")
            }else{
              console.log("File send finished")
              websocket.send('b');
            }


            let prog=Math.round(offset/file.size*100);
            prog = Math.min(prog, 100);
            bar.value=prog;
            bar.innerHTML=prog+"%"
            state=0;
          }else if(state==2){
            console.log("Error en la carga del fichero")
          }else{
            console.log("Waiting for response...")
            setTimeout(sendChunk,4)
          }
        }
        sendChunk();
      }else{
        alert("First select a file!");
      }
    }

    function ws_send_pkt(value){
      console.log("Sending packet...");
      websocket.send(value);
    }

    onMessage=function (event) {
      res_ota=document.getElementById('resultado')
      if (event.data=='77')
        state=1;
      else if (event.data=='101'){
        state=2;
        res_ota.innerText = "Upload filed!";
        res_ota.classList.remove('is-success');
        res_ota.classList.add('is-danger');
        res_ota.removeAttribute('hidden');
      } 
      else if (event.data=='100'){
        res_ota.innerText = "File Loaded!";
        res_ota.classList.remove('is-danger');
        res_ota.classList.add('is-success');
        res_ota.removeAttribute('hidden');
      }
        
        //console.log("Received new message: "+event.data);

        ws_concat_data(event.data)
    }`
    //let ws_led=document.getElementById('ws-led');
    //ws_led.classList.remove('led-red');
    //ws_led.classList.add('led-green');
    //blinkLED(ws_led);
    console.log('Connection opened');
    websocket.onmessage=onMessage;
    //let ws_function=document.getElementById('ws-function');
    //ws_function.innerHTML="function function_alert(){alert('Función creada')}";
    //function_alert();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let state;
    
    console.log(`On message: ${event.data}\n`);
} /*REVISAR LA FORMA DE SOBREESCRIBIR*/ 

function onError(event) {
    let ws_led=document.getElementById('ws-led');
    ws_led.classList.remove('led-green');
    ws_led.classList.add('led-red');
    blinkLED(ws_led);
    console.log('Connection error');
    
}

function blinkLED(button) {
    // Agrega la clase 'blink' para iniciar la animación de parpadeo
    button.classList.add('blink');

    // Después de 1 segundo, quita la clase 'blink' para detener la animación
    setTimeout(() => {
    button.classList.remove('blink');
    }, 10000);
}


function onLoad(event) {
    initWebSocket();
}

function sendWebsocket() {
    websocket.send('toggle mundo');
}

