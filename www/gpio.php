<!DOCTYPE html>
<html>
<head>

<title>GPIO</title>

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
<h1>Raspberry GPIO</h1>
<br><br>

<div id="main" style="margin-left:200px;">

<form method="post" action="gpioCMD.php">
    <input type="submit" value="Blink LED" name="submit">
    <img src="imgs/pi_zero_gpio_layout_model_b.png" alt="Raspberry Pinout">
</form>


</div>

</body>
</html>