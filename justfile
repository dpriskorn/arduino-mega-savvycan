default: compile

compile:
	arduino-cli compile --fqbn arduino:avr:mega MegaRET/

upload:
	arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:mega MegaRET/

monitor:
	arduino-cli monitor -p /dev/ttyACM0 --fqbn arduino:avr:mega
