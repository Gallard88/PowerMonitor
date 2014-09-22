#!/bin/bash
# Provides: PowerMonitor
# Required-Start: $syslog
# Required-Stop: $syslog
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: Example PowerMonitor
# Description: Start/Stops  PowerMonitor
start() {
	# Start daemon
	echo "PowerMonitor: Start"
	/usr/sbin/PowerMonitor /dev/ttyUSB0
}

stop() {
	# Stop daemon
	echo "PowerMonitor: Stop"
	pkill PowerMonitor
}
case "$1" in
	start)
		start
		;;
	stop)
		stop
		;;
	retart)
		stop
		start
		;;
*)
echo "Usage: $0 {start|stop|restart}"
esac
exit 0
