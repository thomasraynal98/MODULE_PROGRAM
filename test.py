import serial
import time

ser = serial.Serial("/dev/ttyS0",9600)
while True:
	oui = ser.readline()
	ser.flushInput()
	print(oui)

