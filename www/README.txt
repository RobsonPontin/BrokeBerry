1 - sudo apt-get update
2 - sudo apt-get install apache2
3 - /etc/apache2/
4 - 
	<Directory /var/www/>
		Options Indexes FollowSymLinks
		AllowOverride None
		Require all granted
	</Directory>



To allow Apache accept shell comands using PHP

Source: https://stackoverflow.com/questions/16740802/how-to-make-a-system-call-remotely/22953339#22953339

The Apache’s user www-data need to be granted privileges to execute certain applications using sudo.

    Run the command sudo visudo. Actually we want to edit the file in etc/sudoers.To do that, by using sudo visudo in terminal ,it duplicate(temp) sudoers file to edit.
    At the end of the file, add the following ex:-if we want to use command for restart smokeping and mount command for another action,

www-data ALL=NOPASSWD: /etc/init.d/smokeping/restart, /bin/mount

(This is assuming that you wish to run restart and mount commands using super user (root) privileges.)

However, if you wish to run every application using super user privileges, then add the following instead of what’s above.You might not want to do that, not for ALL commands, very dangerous.

www-data ALL=NOPASSWD: ALL

3.After edit the sudoers file(by visudo we edit the temp file of sudoers so save and quit temp file(visudo) to write in sudoers file.(wq!)

4.That’s it, now use exec() in the following manner inside your xxx.phpscript.keep remember to use sudo before the command use in the php script.

ex:-

exec ("sudo /etc/init.d/smokeping restart 2>&1");

So in your problem,add the commands that you wish to use in to the step no (2.) as I add and change your php script as what you want.
