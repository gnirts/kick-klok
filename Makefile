monitor: upload
	arduino-cli monitor -p /dev/ttyACM0

upload: compile
	arduino-cli upload

compile:
	arduino-cli compile
