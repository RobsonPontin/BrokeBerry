<!DOCTYPE html>
<html>
<head>

<title>Control</title>

<link rel='stylesheet' type='text/css' href='style.css' />

<style>

</style>
</head>
<body>

<div id="mySidenav" class="sidenav">
  <a href="index.php" id="status">Home</a>
  <a href="control.php" id="control">Control</a>
  <a href="gpio.php" id="GPIO">GPIO</a>
  <a href="fileManager.php" id='fileManager'>File Manager</a>
  <a href="setup.php" id="setup">Setup</a>
</div>

<div id="main" style="margin-left:200px;">
<h1>Raspberry Control</h1>
<br><br>

<form action="mqtt.php" method="post">
  <input type="submit" name="startMQTT" value="Start MQTT">
  <br>
  <input type="submit" name="stopMQTT" value="Stop MQTT">
  <br>
</form>

</div>

</body>
</html>