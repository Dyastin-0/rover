const char waitPage[] PROGMEM = R"=====(
  <!DOCTYPE html>
   <html>
   <head>
     <title>Waiting page</title>
     <meta name="viewport" content="width=device-width, initial-scale=1.0">
     <style>
      :root {
        --bg: rgb(50, 50, 50);
        --bg2: rgb(35, 35, 35);
        --text: rgb(220, 220, 220);
      }
       * {
         padding: 0;
         margin: 0;
       }
       html, body {
        width: 100%;
        height: 100%;
        display: flex;
        justify-content: center;
        flex-direction: column;
        align-items: center;
        background-color: rgb(25, 25, 25);
       }
       h1, h2 {
        color: var(--text);
       }
      </style>
   </head>
   <body>
      <h1> Someone else is controlling :( </h1>
      <h2> You can wait until they disconnect. </h2>
      <h2 id="queue-number"> </h2>
   </body>
   <script>
      const socket = new WebSocket(`ws://${location.host}:81`);
      socket.onmessage = (event) => {
        if (event.data == "redirect") {
          location.href = location.href;
        }
      } 
   </script>
   </html>
  )=====";