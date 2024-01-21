//Se puede manipular como blob desde fylesystem

ws_header=["ws-navbar","ws-led"] // incluir los elementos que se necesitan

/*
The loaded content is the indicated in show variable, the path
is the directory on websoket server to get the file for the
content which has two or tree elements: html, js and css. The
name of these three elements are mirrored in each directory 
only changes their extensions. When websoket receives a request it 
returns the three files if that exists.

The websoket server and client communicates via json object containing the event data which 
has the neext structure.

ws_app={
    "type": "",
    "data": {}
}

In this object the type element could be:
- ws-body.- To change the content of the ws-body div element
- ws-section.- To change the content of a specific section
- ws-data.- To get information in a directly mode.
- ws-header.- To change the content of the header
- ws-footer.- To change the content of the footer
- ws-system To manage functions like autentications, erros ota.

1. ws-body
In the ws-content type the data variable returns a json object 
in which receives the three files html, js and css with the next
structure:

data={
    "html":"html data",
    "js":"js data",
    "css":"css data
}

At the end of this request the next data will be overwritten
- The innerhtml of the ws-body id
- The innerhtml of the ws-js id
- The innerhtml of the ws-css
*/

ws_body="ws-ota"

/*
2. ws-section
This type of event allows you to change the content or the properties of one
section inside the ws-body via the ws-section id. Alternatively
you could change the content of ws-js section adding, removing or
overwriting the content of the function.

For this goal the structure of the data object is as follows:

data={
    "html": {"inner":"","properties":{}},
    "js": {functions},
    "css":{styles},
}

The object functions will contain the new functions to add or overwrite,
and the structure is as follows:

functions={
    "name":"function",
    "name2":"function",
}

Finally for the styles object the idea is the same.

The general idea is that each ws-body element knows its functions and sections,
so you could modify the structure of them via ws-section requests. Additionally 
every function and id section and style must be registed by ws_section and ws_function 
objects.
*/

ws_section={ //si enable es true se carga en el arranque actualiza una sección del content
        "cnt-x":{
            "sec_1":{"path":"","loaded":false},
            "sec_2":{"path":"","loaded":false},
            "sec_3":{"path":"","loaded":false},
            "sec_4":{"path":"","loaded":false},
            "sec_5":{"path":"","loaded":false},
        }
}

/*
El objeto ws_functions contiene la estructura básica

ws_functions={ 
    "cnt-x":functions
}

En donde el objeto functions contiene las funciones del elemento class 
ws-body. En el ejemplo cnt-x es el id del elemento ws-body

functions= {
        "name1":fn1,
        "name2":fn2,
}
*/

ws_functions={ 
    "ws-body":["functions"]
}

//Validar ya que es posible que no se carguen los estilos
ws_styles={
    "ws-body":["led-css"],

}

/*Verify if it is posible to get first the websocket connection
and after of that get the ccs libraries and js files.  
*/
// to manage the general libraries of css
ws_lib_css=["ws-bulma"]
    

/*To manage the general libraries of javascript */
ws_lib_js=["ws-chartjs"] 



ws_app={
  "type":"ws-system",
	"data":{
    "ws-method":"ws-onload",
    "ws-request":{
        "ws-header":ws_header,
        "ws-body":ws_body,
        "ws-lib-js":ws_lib_js,
        "ws-lib-css":ws_lib_css
    }
  }	
};

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

let jsws={
  "type": "websocket",
  "data": {"id":"ws://${window.location.host"}
}

function onOpen(event) {
    let ws_js=document.getElementById('ws-js')
    

    const ws_app_json = JSON.stringify(ws_app)
    websocket.send(`${ws_app_json}`)
    //websocket.send(`{"type":"tipo de ws desde ll","data":"ggggdatos de proceso"}`)
    //websocket.send(ws_app_json)
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
        
        console.log("Received new message: "+event.data);
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

/*
function newMessage(event) {
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
      
      console.log("Received: "+event.data);
}*/

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

/*
7. ws-system
The data od ws-system has the next structure:

data={
  "method":"name",
  request":request
}

Where name is a function on the websocket server and
request let process the function to return the request data

*/