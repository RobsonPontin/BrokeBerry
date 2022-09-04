<?php
// Error report = for debugging
ini_set('display_errors', 1);
error_reporting(E_ALL);

$start_program = "/brokeberry/scripts/startProgram.sh";

if(isset($_POST['startMQTT']))
{
	echo "Iniciando BrokeBerry...\n";

	//ref: https://stackoverflow.com/questions/8387077/starting-a-daemon-from-php
	// /dev/null - redirect STDOUT to /dev/null (blackhole it, in other words)
    // 2>&1 - redirect STDERR to STDOUT (blackhole it as well)
    // & detach process and run in the background

	exec("sudo $start_program start > /dev/null 2>&1 &");

	sleep(1);

	$retval = exec("sudo $start_program status");
	echo '

	<hr /> Current status: ' . $retval;

}
else if(isset($_POST['stopMQTT']))
{
	echo "Parando BrokeBerry...\n";

	exec("sudo $start_program stop");

	sleep(1);
	$retval = exec("sudo $start_program status");
	echo '

	<hr /> Current status: ' . $retval;
}
?>