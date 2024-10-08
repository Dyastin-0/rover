const char controlPage[] PROGMEM = R"=====(
  <!DOCTYPE html>
   <html>
   <head>
     <title>Controller</title>
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
         background-color: rgb(25, 25, 25);
         font-family: Arial, sans-serif;
         width: calc(100%);
         height: calc(100%);
         display: flex;
       }
       .container {
         display: flex;
         gap: 18px;
         flex-basis: 33.33%;
       }
       .container.bottom-left {
        flex-direction: column;
        justify-content: space-between;
        align-items: center;
        padding: 18px;
       }
       .container.space-between {
        flex-direction: column;
        justify-content: space-between;
        align-items: center;
        padding: 18px;
       }
       .container.bottom-right {
        flex-direction: column;
        justify-content: space-between;
        align-items: flex-end;
        padding: 18px;
       }
       .controller {
         display: grid;
         grid-template-columns: 1fr 1fr 1fr;
         grid-template-rows: 1fr 1fr 1fr;
         width: fit-content;
         height: fit-content;
         gap: 9px;
       }
       .controller button {
         width: 50px;
         height: 50px;
         background-color: var(--bg);
         border: none;
         border-radius: 50%;
         color: var(--text);
         outline: none;
       }
       .controller button:hover {
         cursor: pointer;
       }
        #forwardBtn {
          grid-column: 3;
          grid-row: 1;
        }
        #backwardBtn {
          grid-column: 3;
          grid-row: 3;
        }
        #leftBtn {
          grid-column: 2;
          grid-row: 2;
        }
        #rightBtn {
          grid-column: 4;
          grid-row: 2;
        }
        #forwardLeftBtn {
          grid-column: 2;
          grid-row: 1;
        }
        #forwardRightBtn {
          grid-column: 4;
          grid-row: 1;
        }
        #backwardLeftBtn {
          grid-column: 2;
          grid-row: 3;
        }
        #backwardRightBtn {
          grid-column: 4;
          grid-row: 3;
        }
        #spinLeftBtn {
          grid-column: 1;
          grid-row: 2;
          font-weight: 900;
          font-size: 18px;
        }
        #spinRightBtn {
          grid-column: 5;
          grid-row: 2;
          font-weight: 900;
          font-size: 18px;
        }
       .control-slider-container {
         width: 200px;
       }
       .control-slider {
         -webkit-appearance: none;
         appearance: none;
         width: 100%;
         height: 9px;
         background-color: var(--bg);
         border-radius: 10px;
         outline: none;
         opacity: 0.7;
         transition: opacity 0.3s;
       }
       .control-slider::-webkit-slider-thumb {
         -webkit-appearance: none;
         appearance: none;
         width: 25px;
         height: 25px;
         background: var(--text);
         border-radius: 50%;
         cursor: pointer;
       }
       .mode-toggle {
         display: flex;
         align-items: center;
         justify-content: center;
         flex-direction: column;
       }
       .mode-toggle label {
         position: relative;
         display: inline-block;
         width: 60px;
         height: 34px;
       }
       .toggle-switch input {
         opacity: 0;
         width: 0;
         height: 0;
       }
       .slider {
         position: absolute;
         cursor: pointer;
         top: 0;
         left: 0;
         right: 0;
         bottom: 0;
         background-color: var(--bg);
         transition: .4s;
         border-radius: 34px;
       }
       .slider:before {
         position: absolute;
         content: "";
         height: 26px;
         width: 26px;
         left: 4px;
         bottom: 4px;
         background-color: var(--bg2);
         transition: .4s;
         border-radius: 50%;
       }
       input:checked + .slider {
         background-color: var(--bg);
       }
       input:checked + .slider:before {
         transform: translateX(26px);
       }
       .slider.round {
         border-radius: 34px;
       }
       .slider.round:before {
         border-radius: 50%;
       }
       span {
         color: var(--text);
       }
       h2, h5, h3 {
         text-align: center;
         color: var(--text);
       }
       h5.left {
        text-align: left;
        font-weight: 500;
        padding: 9px;
        border-radius: 9px;
        background-color: var(--bg2);
       }
       .row {
         display: flex;
         flex-direction: column;
         align-items: center;
         justify-content: center;
         padding-top: 9px;
         gap: 9px;
         width: 100%;
       }
       .row.scroll {
        width: calc(100% - 9px);
        padding-right: 9px;
        overflow-y: auto;
        scroll-behavior: smooth;
       }
       .row.scroll::-webkit-scrollbar {
        width: 9px;
        }
        .row.scroll::-webkit-scrollbar-track {
          border-radius: 9px;
        }
        .row.scroll::-webkit-scrollbar-thumb {
          background-color: var(--bg2);
          border-radius: 9px;
        }
       .row.fit {
        width: fit-content;
       }
       .row.left {
        gap: 5px;
        padding-top: 9px;
        align-items: flex-start;
        overflow-y: auto;
        scroll-behavior: smooth;
       }
       .input {
         border: none;
         outline: none;
         border-radius: 9px;
         font-size: 9px;
         padding: 4px;
         width: 70px;
       }
       .button {
         padding: 9px;
         border: none;
         outline: none;
         border-radius: 9px;
         font-size: 14px;
         background-color: var(--bg);
         color: var(--text);
       }
       .messages {
         display: flex;
         padding: 9px;
         align-items: center;
         flex-direction: column;
         width: 174px;
         border-radius: 9px;
         background-color: var(--bg);
       }
       .column {
        display: flex;
        flex-direction: row;
        gap: 9px;
        justify-content: center;
       }
       @media (max-width: 500px) {
        html, body {
          flex-direction: column;
        }
        .container.bottom-right {
          order: 3;
          align-items: center;
        }
        .container.container.space-between {
          order: 1;
        }
       }
     </style>
   </head>
   <body>
    <div class="container bottom-left">
      <div class="row fit">
        <div class="messages">
          <h5>Sensors</h5>
          <div class="row left" id="sensor-list"></div>
        </div>
      </div>
      <div class="controller">
        <button id="forwardBtn"
          ontouchstart="sendControlSignal('Forward', 1)"
          ontouchend="sendControlSignal('Forward', 0)">
          &#9650;
          </button>
        <button id="leftBtn"
          ontouchstart="sendControlSignal('Left', 1)"
          ontouchend="sendControlSignal('Left', 0)">
          &#9664;
        </button>
        <button id="rightBtn"
          ontouchstart="sendControlSignal('Right', 1)"
          ontouchend="sendControlSignal('Right', 0)">
          &#9654;
        </button>
        <button id="backwardBtn"
          ontouchstart="sendControlSignal('Backward', 1)"
          ontouchend="sendControlSignal('Backward', 0)" >
          &#9660;
        </button>
        <button id="forwardLeftBtn"
          ontouchstart="sendControlSignal('ForwardLeft', 1)"
          ontouchend="sendControlSignal('ForwardLeft', 0)">
          &#9654;
        </button>
        <button id="forwardRightBtn"
          ontouchstart="sendControlSignal('ForwardRight', 1)"
          ontouchend="sendControlSignal('ForwardRight', 0)">
          &#9664;
        </button>
        <button id="backwardLeftBtn"
          ontouchstart="sendControlSignal('BackwardLeft', 1)"
          ontouchend="sendControlSignal('BackwardLeft', 0)">
          &#9654;
        </button>
        <button id="backwardRightBtn"
          ontouchstart="sendControlSignal('BackwardRight', 1)"
          ontouchend="sendControlSignal('BackwardRight', 0)">
          &#9650;
        </button>
      <button id="spinLeftBtn"
        ontouchstart="sendControlSignal('SpinLeft', 1)"
        ontouchend="sendControlSignal('SpinLeft', 0)">
        &#x21BA;
      </button>
      <button id="spinRightBtn"
      ontouchstart="sendControlSignal('SpinRight', 1)"
      ontouchend="sendControlSignal('SpinRight', 0)">
      &#x21BB;
    </button>
      </div>
    </div>
     <div class="container space-between">
      <div class="row">
        <h2>Controller</h2>
      </div> 
      <div class="row">
        <h5 id="modeText">Control mode</h5>
        <div class="mode-toggle">
          <label class="toggle-switch">
            <input type="checkbox" id="modeToggle" onchange="toggleMode()">
            <span class="slider round"></span>
          </label>
        </div>
      </div>
      <div class="column"> 
        <div class="row">
          <h5>Distance</h5>
          <select id="distance-select" class="input" onchange="setDistance()">
            <option value="10">10</option>
            <option value="20">20</option>
            <option value="30">30</option>
            <option value="40">40</option>
            <option value="50">50</option>
            <option value="60">60</option>
            <option value="70">70</option>
            <option value="80">80</option>
            <option value="90">90</option>
            <option value="100">100</option>
          </select>
        </div>
        <div class="row">
          <h5>Delay</h5>
          <select id="delay-select" class="input" onchange="setDelay()">
            <option value="100">100</option>
            <option value="200">200</option>
            <option value="300">300</option>
            <option value="400">400</option>
            <option value="500">500</option>
            <option value="600">600</option>
            <option value="700">700</option>
            <option value="800">800</option>
            <option value="900">900</option>
            <option value="1000">1000</option>
          </select>
        </div>
      </div>
     </div>
    <div class="container bottom-right">
      <div class="row fit">
        <div class="messages">
          <h5>Logs</h5>
          <div class="row left" id="message-list"></div>
         </div>
      </div>
      <div class="row fit">
        <div class="row">
          <h5>Speed</h5>
          <div class="control-slider-container">
            <input type="range" min="1" max="10" value="5" class="control-slider" id="speed-controller" oninput="sendSpeedSignal(this.value)">
          </div>
        </div>
      </div>
    </div>
     <script>
       const socket = new WebSocket(`ws://${location.host}:81`);

       const modeToggle = document.getElementById('modeToggle');
       const modeText = document.getElementById('modeText');

       const distanceText = document.getElementById('distance-select');
       const delayText = document.getElementById('delay-select');
       const speedControl = document.getElementById('speed-controller');

       const messageList = document.getElementById('message-list');
       const sensorDataList = document.getElementById('sensor-list');

       function sendControlSignal(direction, value) {
         fetch('/control' + direction, {
           method: 'POST',
           headers: {
             'Content-Type': 'text/plain',
           },
           body: value.toString()
         });
       }
       function sendSpeedSignal(speed) {
         fetch('/controlSpeed', {
           method: 'POST',
           headers: {
             'Content-Type': 'text/plain',
           },
           body: speed.toString()
         });
       }
       function toggleMode() {
         fetch('/controlMode', {
           method: 'POST',
           headers: {
             'Content-Type': 'text/plain',
           },
           body: modeToggle.checked ? '1' : '0'
         });
       }
       function setDistance() {
         var distanceSelect = document.getElementById('distance-select');
         var selectedDistance = distanceSelect.value;
         fetch('/setDistance' , {
           method: 'POST',
           headers: {
             'Content-Type': 'text/plain',
           },
           body: selectedDistance
         });
       }
       function setDelay() {
         var delaySelect = document.getElementById('delay-select');
         var selectedDelay = delaySelect.value;
         fetch('/setDelay' , {
           method: 'POST',
           headers: {
             'Content-Type': 'text/plain',
           },
           body: selectedDelay
         });
       }
       function listenToChanges() {
          socket.onopen = () => {
            const deviceInfo = getDeviceInfo();
            const clientMessage = `Connected from ${deviceInfo.browser}, ${deviceInfo.os} ${deviceInfo.device}.`;
            socket.send(clientMessage);
            socket.send("broadcast_request");
          }
          socket.onmessage = (event) => {
            const data = JSON.parse(event.data);
            if ("sensors" in data) {
              sensorDataList.innerHTML = '';
              data.sensors.forEach((item, index) => {
                const messageContainer = document.createElement('h5');
                messageContainer.classList.add('left');
                messageContainer.textContent = item;
                sensorDataList.append(messageContainer);
              });
            }
            if ("logs" in data) {
              messageList.innerHTML = '';
              data.logs.forEach((item, index) => {
                const log = document.createElement('h5');
                log.classList.add('left');
                log.textContent = item;
                messageList.append(log);
              });
            }
            if ("variables" in data) {
              const variables = data.variables;
              distanceText.value = variables[0];
              delayText.value = variables[1];
              speedControl.value = parseFloat(variables[2]).toFixed(1) * 10;
              if (variables[3] == "1") {
                modeToggle.checked = true;
                modeText.innerText = "Automatic mode";
              } else {
                modeToggle.checked = false;
                modeText.innerText = "Control mode";
              }
            }
          }
          }
          function getDeviceInfo() {
          const ua = navigator.userAgent;
          let deviceInfo = {
              device: "Unknown",
              os: "Unknown",
              browser: "Unknown"
          };

          // Detecting Device
          if (/mobile/i.test(ua)) {
              deviceInfo.device = "Mobile";
          } else if (/tablet/i.test(ua)) {
              deviceInfo.device = "Tablet";
          } else if (/iPad|iPhone|iPod/.test(ua) && !window.MSStream) {
              deviceInfo.device = "iOS Device";
          } else if (/android/i.test(ua)) {
              deviceInfo.device = "Android Device";
          } else {
              deviceInfo.device = "Desktop";
          }

          // Detecting OS
          if (/Windows NT 10.0/i.test(ua)) {
              deviceInfo.os = "Windows";
          } else if (/Windows NT 6.3/i.test(ua)) {
              deviceInfo.os = "Windows";
          } else if (/Windows NT 6.2/i.test(ua)) {
              deviceInfo.os = "Windows";
          } else if (/Windows NT 6.1/i.test(ua)) {
              deviceInfo.os = "Windows";
          } else if (/Mac OS X 10[._]\d+/i.test(ua)) {
              deviceInfo.os = "Mac";
          } else if (/Android/i.test(ua)) {
              deviceInfo.os = "Android";
          } else if (/iP(hone|od|ad)/i.test(ua)) {
              deviceInfo.os = "iOS";
          } else if (/Linux/i.test(ua)) {
              deviceInfo.os = "Linux";
          }

          // Detecting Browser
          if (/edg/i.test(ua)) {
              deviceInfo.browser = "Microsoft Edge";
          } else if (/chrome|crios|crmo/i.test(ua) && !/edg/i.test(ua)) {
              deviceInfo.browser = "Chrome";
          } else if (/firefox|fxios/i.test(ua)) {
              deviceInfo.browser = "Firefox";
          } else if (/safari/i.test(ua) && !/chrome|crios|crmo/i.test(ua) && !/edg/i.test(ua)) {
              deviceInfo.browser = "Safari";
          } else if (/opr|opera/i.test(ua)) {
              deviceInfo.browser = "Opera";
          } else if (/msie|trident/i.test(ua)) {
              deviceInfo.browser = "Internet Explorer";
          }
          return deviceInfo;
        }
       listenToChanges();
     </script>
   </body>
   </html>
  )=====";