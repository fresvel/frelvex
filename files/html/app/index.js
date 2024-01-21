const express = require('express');
const path = require('path');
const fs = require('fs').promises;
const directorioArchivos=path.join(__dirname, '../')

const app = express();
const puerto = 3000; // Puedes cambiar el número de puerto según tus necesidades

// Establecer el directorio de archivos estáticos
app.use(express.static('/home/fresvel/esp/Proyectos/filesystem/files'));

// Ruta de ejemplo para manejar solicitudes a la raíz
app.get('/', async(req, res) => {
    try {
        // Lee el contenido de cada archivo de manera asíncrona
        const headContent = await fs.readFile(path.join(directorioArchivos, 'header/head.html'), 'utf-8');
        const headerContent = await fs.readFile(path.join(directorioArchivos, 'header/header.html'), 'utf-8');
        const bodyContent = await fs.readFile(path.join(directorioArchivos, 'content/panel.html'), 'utf-8');//////////////////////////
        const footerContent=await fs.readFile(path.join(directorioArchivos, 'footer/footer.html'), 'utf-8');
        // Combina el contenido de los archivos y envíalo al cliente
        const combinedContent = headContent + headerContent+bodyContent+footerContent;
        res.send(combinedContent);
      } catch (error) {
        console.error('Error al leer o enviar archivos:', error);
        res.status(500).send('Error interno del servidor');
      }
});

// Iniciar el servidor
app.listen(puerto, () => {
  console.log(`Servidor estático en http://localhost:${puerto}`);
});
