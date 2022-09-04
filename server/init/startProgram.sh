#!/bin/sh
#
# Starts BrokeBerry
#

DAEMON_NAME=BrokeBerry

DAEMON=/brokeberry/bin/brokeberry
#-s Start, -N alters the priority
DAEMON_OPTS="-s -N -15"

start()
{
	echo "Starting $DAEMON_NAME ..."

	start-stop-daemon --start --exec $DAEMON -- $DAEMON_OPTS -t
	RETVAL=$?
	if [ $RETVAL -eq 0 ]; then
		echo "OK - $DAEMON_NAME has started!"
	else
		echo "FAILED - $DAEMON_NAME did not start!" 
	fi
	
	return $RETVAL
}

stop()
{
	echo "Stopping $DAEMON_NAME ..."

	start-stop-daemon --stop --exec $DAEMON
	RETVAL=$?
	if [ $RETVAL -eq 0 ]; then
		echo "OK - $DAEMON_NAME has stopped!"
	else
		echo "FAILED - $DAEMON did not stop!" 
	fi
	
	return $RETVAL
}

status()
{
	echo "$DAEMON_NAME status: "
	
	start-stop-daemon -K -t -q -x $DAEMON 2>/dev/null
	RETVAL=$?
	if [ $RETVAL -eq 0 ]; then
		echo "Running"
	else
		echo "Stopped"
	fi
	
	return $RETVAL
}

case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	status)
		status
		;;
	*)
		echo "CMD options: {start | stop}"
		exit 1
esac

exit $RETVAL
