yarp read ... /annabell/output | while read l; do pico2wave -w a.wav "$l"; aplay a.wav; rm -f a.wav; done
