const char controlPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>Rover Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    * {
      padding: 0;
      margin: 0;
    }
    html, body {
      background-color: #222;
      font-family: Arial, sans-serif;
      width: calc(100% - 72px);
      height: calc(100% - 72px);
      margin: 36px;
      display: flex;
      align-items: center;
      flex-direction: column;
      justify-content: center;
    }
    .container {
      display: flex;
      flex-direction: row;
      flex-flow: flex-end;
      align-items: center;
      justify-content: center;
      gap: 18px;
      height: fit-content;
    }
    .controller {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      grid-template-rows: 1fr 1fr 1fr;
    }
    .controller button {
      width: 50px;
      height: 50px;
      background-color: #66c3ff;
      border: none;
      border-radius: 50%;
      color: #fff;
      outline: none;
    }
    .controller button:hover {
      background-color: #aad4ff;
    }
    #forwardBtn {
      grid-column: 2;
      grid-row: 1;
    }
    #backwardBtn {
      grid-column: 2;
      grid-row: 3;
    }
    #leftBtn {
      grid-column: 1;
      grid-row: 2;
    }
    #rightBtn {
      grid-column: 3;
      grid-row: 2;
    }
    .control-slider-container {
      width: 80%;
      margin-top: 20px;
    }
    .control-slider {
      -webkit-appearance: none;
      appearance: none;
      width: 100%;
      height: 9px;
      background-color: #66c3ff;
      border-radius: 10px;
      outline: none;
      opacity: 0.7;
      transition: opacity 0.3s;
      margin-top: 20px;
    }
    .control-slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      background: #fff;
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
      background-color: #66c3ff;
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
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }
    input:checked + .slider {
      background-color: #66c3ff;
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
      color: #fff;
    }
    h1, h4, h3 {
      text-align: center;
      color: #fff;
    }
    .row {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 9px;
    }
    .input {
      border: none;
      outline: none;
      border-radius: 9px;
      font-size: 14px;
      padding: 4px;
      width: 70px;
    }
    .button {
      padding: 9px;
      border: none;
      outline: none;
      border-radius: 9px;
      background-color: #66c3ff;
      color: #fff;
    }
  </style>
</head>
<body>
  <h1>Controller</h1>
  <div class="container">
    <div class="controller">
      <button id="forwardBtn" ontouchstart="sendControlSignal('Forward', 1)" ontouchend="sendControlSignal('Forward', 0)" onmousedown="sendControlSignal('Forward', 1)" onmouseup="sendControlSignal('Forward', 0)">&#9650;</button>
      <button id="leftBtn" ontouchstart="sendControlSignal('Left', 1)" ontouchend="sendControlSignal('Left', 0)" onmousedown="sendControlSignal('Left', 1)" onmouseup="sendControlSignal('Left', 0)">&#9664;</button>
      <button id="rightBtn" ontouchstart="sendControlSignal('Right', 1)" ontouchend="sendControlSignal('Right', 0)" onmousedown="sendControlSignal('Right', 1)" onmouseup="sendControlSignal('Right', 0)">&#9654;</button>
      <button id="backwardBtn" ontouchstart="sendControlSignal('Backward', 1)" ontouchend="sendControlSignal('Backward', 0)" onmousedown="sendControlSignal('Backward', 1)" onmouseup="sendControlSignal('Backward', 0)">&#9660;</button>
    </div>
    <div class="row">
      <h4 id="modeText">Control mode</h4>
      <div class="mode-toggle">
        <label class="toggle-switch">
          <input type="checkbox" id="modeToggle" onchange="toggleMode()">
          <span class="slider round"></span>
        </label>
      </div>
    </div>
    <div class="control-slider-container">
      <h4 class="label">Speed</h4>
      <input type="range" min="1" max="10" value="5" class="control-slider" id="speedSlider" oninput="sendSpeedSignal(this.value)">
    </div>
    <div class="control-slider-container">
      <h4 class="label">Shift</h4>
      <input type="range" min="0" max="1" value="0" class="control-slider" id="shiftSlider" oninput="sendShiftSignal(this.value)">
    </div>
  </div>
  <div class="container">
    <div class="row">
      <h4>Set distance</h4>
      <input id="distance-input" class="input" type="text">
      <button class="button" onclick="setDistance()">Set</button>
    </div>
    <div class="row">
      <h4>Set delay</h4>
      <input id="delay-input" class="input" type="text">
      <button class="button" onclick="setDelay()">Set</button>
    </div>
  </div>
  <script>
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
    function sendShiftSignal(shift) {
      fetch('/controlShift', {
        method: 'POST',
        headers: {
          'Content-Type': 'text/plain',
        },
        body: shift.toString()
      });
    }
    function toggleMode() {
      var modeToggle = document.getElementById('modeToggle');
      var modeText = document.getElementById('modeText');
      if (modeToggle.checked) {
        modeText.innerText = 'Automatic mode';
      } else {
        modeText.innerText = 'Control mode';
      }
      fetch('/controlMode', {
        method: 'POST',
        headers: {
          'Content-Type': 'text/plain',
        },
        body: modeToggle.checked ? '1' : '0'
      });
    }
    const distanceText = document.getElementById('distance-input');
    const delayText = document.getElementById('delay-input');
    function setDistance() {
      fetch('/setDistance' , {
        method: 'POST',
        headers: {
          'content-type': 'text/plain',
        },
        body: distanceText.value.toString()
      });
    }
    function setDelay() {
      console.log(delayText.value);
      fetch('/setDelay' , {
        method: 'POST',
        headers: {
          'content-type': 'text/plain',
        },
        body: delayText.value.toString()
      });
    }
    const fetchData = async () => {
      fetch('/get/distance')
      .then(response => response.text())
      .then((result) => {
        distanceText.value = result;
      });
      fetch('/get/delay')
      .then(response => response.text())
      .then((result) => {
        delayText.value = result;
      });
    }
    fetchData();
  </script>
</body>
</html>
)=====";