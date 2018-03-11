const char CONFIRMATION_page[] PROGMEM = R"=====(
	<!DOCTYPE html>
	<html lang="pt-br">
		<head>
			<title>IOT_CONFIG</title>
			<meta charset="uft=8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
		</head>
		<body>
			<header>
				<h1> Configura&ccedil;&atilde;o do Dispositivo </h1>
			</header>
			<main>
				<p>Configura&ccedil;&atilde;o enviada com sucesso!</p>
				<form method="POST" action="/">
						<input class="sub1" type="submit" value="Voltar"> 
				</form>
			</main>
		</body>
		<style>
			body {
				text-align: center;
			}
			header {
				padding-top: 5px;
				border-bottom: 2px solid #000;
				background-color: #38C2B3;
			}
			main {
				border-bottom: 2px solid #000;
			}
		</style>
	</html>
)=====";


const char CONF_page[] PROGMEM = R"=====(
	<!DOCTYPE html>
	<html lang="pt-br">
		<head>
			<title>Device Config</title>
			<meta charset="uft=8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
		</head>
		<body onload="load()">
			<header>
				<h1> Configura&ccedil;&atilde;o do Dispositivo </h1>
			</header>
			<main>
				<form class="form-conf" method="POST" action="/">
					<div>
						<label>Nome da Rede (SSID)</label>
						<div class="data">
							<input type="text" id="rede" name="rede">
						</div>
		
						<label>Senha da Rede</label>
						<div class="data">
							<input type="password" id="pass" name="pass">
						</div>
		
						<label>IP do Dispositivo</label>
						<div class="data">
							<input id="ip" name="ip" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
						</div>
		
						<label>M&aacute;scara de Subrede</label>
						<div class="data">
							<input id="subn" name="subn" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
						</div>
		
						<label>Gateway</label>
						<div class="data">
							<input id="gatw" name="gatw" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
						</div>

						<label>Nome do Dispositivo</label>
						<div class="data">
							<input type="text" id="devname" name="devname">
						</div>
						<label>Nome do HOST</label>
						<div class="data">
							<input required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$" id="host" name="host">
						</div>
						<label>ID do laboratorio </label>
						<div class="data">
							<input type="text" id="id_lab" name="id_lab">
						</div>


						<div class="sub1">
							<input type="submit" value="Enviar">
						</div>
					</div>
				</form>
			</main>
		</body>
		<script>
			function load() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function()
				{
					if (xhttp.readyState == 4 && xhttp.status == 200)
					{
						var data = xhttp.responseText;
						var data_s = data.split(" ");
						document.getElementById("rede").value = data_s[0];
						document.getElementById("pass").value = data_s[1];
						document.getElementById("ip").value = data_s[2];
						document.getElementById("subn").value = data_s[3];
						document.getElementById("gatw").value = data_s[4];
						document.getElementById("devname").value = data_s[5];
						document.getElementById("host").value = data_s[6];
						document.getElementById("is_lab").value = data_s[7];
					}
				};
				xhttp.open("POST", "/load", true);
				xhttp.send();
			}
		</script>
		<style>
		body {
			text-align: center;
		}
		header {
			padding-top: 2px;
			background-color: #38C2B3;
			border-bottom: 2px solid #666;
		}
		main {
			border-bottom: 2px solid #666;
		}
		.data{
			padding-bottom: 5px;
		}
		.data input{
			font-size: 16px;
		}
		.form-conf .d-sub1{
			padding-top: 20px;
			padding-bottom: 20px;
		}
		.form-conf .d-sub1 .sub1{
			font-size: 25px;
			width: 150px;
		}
		</style>
	</html>
)=====";
