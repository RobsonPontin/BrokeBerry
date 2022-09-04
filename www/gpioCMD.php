<?php
// Error report = for debugging
ini_set('display_errors',1);
error_reporting(E_ALL);

function startProgram()
{
  echo "Start Program...";
  
  $output = exec("sudo /projects/c_program/gpioProject -b 500");
  echo $output;
}

if(isset($_POST['submit']))
{
   startProgram();
} 
?>