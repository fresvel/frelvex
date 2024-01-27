
The loaded content is the indicated in show variable, the path
is the directory on websoket server to get the file for the
content which has two or tree elements: html, js and css. The
name of these three elements are mirrored in each directory 
only changes their extensions. When websoket receives a request it 
returns the three files if that exists.

The websoket server and client communicates via json object containing the event data which 
has the neext structure.

ws_app={
    "ws-type": "",
    "ws-info": {}
}

In this object the type element could be:
- ws-body.- To change the content of the ws-body div element
- ws-section.- To change the content of a specific section
- ws-linker.- To get information in a directly mode.
- ws-header.- To change the content of the header
- ws-footer.- To change the content of the footer
- ws-system To manage functions like autentications, erros ota.

1. ws-body
In the ws-content type the data variable returns a json object 
in which receives the three files html, js and css with the next
structure:

info={
    "html":"html data",
    "js":"js data",
    "css":"css data
}

At the end of this request the next data will be overwritten
- The innerhtml of the ws-body id
- The innerhtml of the ws-js id
- The innerhtml of the ws-css



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