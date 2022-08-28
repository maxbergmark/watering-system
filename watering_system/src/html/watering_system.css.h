#pragma once

const char *css = R"====(

body {
	background-color: #232322;
}

.title {
	color: white;
	text-align: center;
	padding: 10px;
}

.motorDiv {
	display: flex;
	justify-content: center;
}

.motorBox {
    background-color: #323634;
    margin-top: 10px;
    margin-bottom: 10px;
    margin-left: 5px;
    margin-right: 5px;
    padding: 20px;
    /* height: 80%; */
    width: calc(30% - 10px);
    border: 2px solid black;
    display: flex;
    flex-flow: column;
}

.motorImageBox {
    width: 100%;
    margin: auto;
    min-height: 300px;
    height: auto;
    padding-bottom: 5px;
    padding-top: 5px;
    display: flex;
    justify-content: center;
    flex: 1;
}

.motorImage {
	width: 60%;
	max-height: 100%;
	margin: auto;
	display: block;
	object-fit: scale-down;
}

.motorSoilGraph {
    border: none;
    width: 100%;
    height: 80%;
    margin: auto;
    padding-top: 5px;
    padding-bottom: 5px;
    display: block;
    flex: 1;
}

.motorTextBox {
	border: 2px solid black;
	color: white;
	width: auto;
	padding: 5px;
	margin-top: 5px;
	margin-bottom: 10px;
	height: auto;
	vertical-align: middle;
}

.motorBoxPadding {
	flex: auto;
}

.motorInputDiv {
	width: 100%;
	min-height: 25px;
	margin: auto;
}

.motorInputBox {
	width: calc(70% - 10px);
	height: 20px;
	margin-right: 10px;
	vertical-align: middle;
}

.motorButton {
	height: 20px;
	width: 30%;
	vertical-align: middle;
}

.bigMotorButton {
	width: 100%;
	height: 40px;
}

.logDiv {
    background-color: #323634;
	margin: auto;
	height: 400px;
	width: 99%;
	display: flex;
	justify-content: center;
	border: 2px solid black;
}

.logFrame {
	width: 45%;
	height: 80%;
	background: white;
	margin: auto;
}

)====";