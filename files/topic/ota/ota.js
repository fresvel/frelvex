let state=1;
function updateFilename() {
    var input = document.getElementById('ota-file');
    var ota_fnameSpan = document.getElementById('ota-fname');
    if (input.files.length > 0) {
        ota_fnameSpan.textContent = input.files[0].name;
    } else {
        ota_fnameSpan.textContent = 'Select';
    }
}

function ws_sendFile(){
    ws_test()
    let input=document.getElementById("ota-file");
    let file=input.files[0];
    let bar=document.getElementById("ota-pbar")

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
    //ws_concat_data(event.data)
}
websocket.onmessage = onMessage