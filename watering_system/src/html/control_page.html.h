#pragma once

const char *controlPageHtml = R"====(
<html>
	<head>
		<title>Watering System</title>
		<link rel="icon" type="image/png" href="/favicon.png">
		<link rel="stylesheet" href="/watering_system.css">
	</head>
	<body>
		<div id="title" class="title">
			<h1>Watering System Control Panel</h1>
		</div>
		<div id="motors" class="motorDiv">
		</div>
		<div id="logs" class="logDiv">
		</div>

		<script>

			const activateMotor = (motor_idx, seconds) => {
				var url = "/motor?index=" + motor_idx + "&seconds=" + seconds;
				console.log("trying to activate motor " + motor_idx);
				fetch(url)
					.then(response => response.json())
					.then(data => console.log(data));
			};

			async function getMoisture(div, index) {
				var url = "/sensor?index=" + index + "&sensor_type=soil_moisture";
				var data = await fetch(url)
					.then(response => response.json());
				return "<br>Soil moisture: " + (100*data.value).toFixed(1)  + "%";
			};

			const createButtons = (motor_idx, input, smallDiv, inputDiv, textDiv, controller) => {
				var button = document.createElement("button");
				button.innerHTML = "Activate";
				button.className = "motorButton";
				button.onclick = function() {
					var seconds = input.value;
					input.value = "";
					activateMotor(motor_idx, seconds);
					populateTextDiv(textDiv, motor_idx, controller);
				};
				smallDiv.appendChild(button);

				var bigButton = document.createElement("button");
				bigButton.innerHTML = "Activate (10s)";
				bigButton.className = "bigMotorButton";
				bigButton.onclick = () => {
					activateMotor(motor_idx, 10)
					populateTextDiv(textDiv, motor_idx, controller);
				};
				inputDiv.appendChild(bigButton);

			};

			const createInputDiv = (textDiv, motor_idx, controller) => {
				var motor = getMotor(motor_idx);
				var inputDiv = document.createElement("div");
				inputDiv.className = "motorInputDiv";

				var smallDiv = document.createElement("div");
				smallDiv.className = "motorInputDiv";

				var input = document.createElement("input");
				input.type = "text";
				input.placeholder = "Activation time (seconds)";
				input.className = "motorInputBox";
				smallDiv.appendChild(input);
				inputDiv.appendChild(smallDiv);
				createButtons(motor_idx, input, smallDiv, inputDiv, textDiv, controller);

				return inputDiv;
			};

			async function populateTextDiv(textDiv, motor_idx, controller) {
				await updateConfiguration();
				var motor = getMotor(motor_idx);
				var s = "This is the text for motor " + motor.index
				s += "<br>It has been activated " + motor.activations + " times";
				if (controller) {
					s += await getMoisture(textDiv, controller.sensor_index);
				}
				textDiv.innerHTML = s;
			};

			const createTextDiv = (motor_idx, controller) => {
				var textDiv = document.createElement("div");
				textDiv.className = "motorTextBox";
				populateTextDiv(textDiv, motor_idx, controller);
				setInterval(() => {populateTextDiv(textDiv, motor_idx, controller);}, 10000);
				return textDiv;
			};

			function createGraph(controller) {
				var motorImageDiv = document.createElement("div");
				motorImageDiv.className = "motorImageBox";

				if (controller) {
					var iframe = document.createElement("iframe");
					iframe.src = "http://192.168.10.200:32000/d-solo/cRJOIaY7k/watering-system?orgId=1&panelId=3&refresh=10s&var-sensor_index=" + controller.sensor_index;
					iframe.className = "motorSoilGraph";
					motorImageDiv.appendChild(iframe);
				} else {
					var img = document.createElement("img");
					img.src = "https://st2.depositphotos.com/3904951/9521/v/950/depositphotos_95216484-stock-illustration-electric-motor-icon.jpg";
					img.className = "motorImage";
					motorImageDiv.appendChild(img);
				}
				return motorImageDiv;
			}

			const getMotorDiv = (motor_idx, controllers) => {
				var div = document.createElement("div");
				div.id = "container" + motor_idx;
				div.className = "motorBox";

				var controller = controllers.find(e => e.motor_index == motor_idx);
				div.appendChild(createGraph(controller));
				var textDiv = createTextDiv(motor_idx, controller);
				div.appendChild(textDiv);

				var fillDiv = document.createElement("div");
				fillDiv.className = "motorBoxPadding";
				div.appendChild(fillDiv);

				div.appendChild(createInputDiv(textDiv, motor_idx, controller));
				return div;
			};

			async function getConfiguration() {
				return fetch("/configuration")
					.then(response => response.json());
			};

			function getMotor(motor_idx) {
				return configuration.motors[motor_idx];
			}

			async function updateConfiguration() {
				configuration = await getConfiguration();				
			}

			var configuration;
			async function buildMotorDivs() {
				await updateConfiguration();
				var motorDiv = document.querySelector("#motors");
				configuration.motors.forEach((motor, idx) => {
					var div = getMotorDiv(idx, configuration.watering_controllers);
					motorDiv.appendChild(div);
				});
			}

			async function buildLogDivs() {
				// await updateConfiguration();
				var logDiv = document.querySelector("#logs");

				var logIframe = document.createElement("iframe");
				logIframe.src = "http://192.168.10.40/logs";
				logIframe.className = "logFrame";
				logDiv.appendChild(logIframe);

				var crashlogIframe = document.createElement("iframe");
				crashlogIframe.src = "http://192.168.10.40/crashlog";
				crashlogIframe.className = "logFrame";
				logDiv.appendChild(crashlogIframe);

			}


			document.cookie = "SameSite=Secure";
			buildMotorDivs();
			buildLogDivs();
			
		</script>
	</body>
</html>
)====";