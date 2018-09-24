import sys
import os
import serial
import base64
from subprocess import call
from serial import SerialException

serialport = sys.argv[1]
command = sys.argv[2]
if command == 'upload':
    fileName = sys.argv[3]
    fileNameNoExt = os.path.splitext(fileName)[0]
elif command == 'delete':
    fileName = sys.argv[3]
    fileNameNoExt = os.path.splitext(fileName)[0]
elif command == 'play':
    fileName = sys.argv[3]
    fileNameNoExt = os.path.splitext(fileName)[0]
else:
    command = sys.argv[1]

def writeLineCR(ser,str):
    sys.stdout.write(str.replace('\r', '\n'))
    sys.stdout.write('\n')
    ser.write(str)
    ser.write('\r')

def readResponse(ser):
    response = ''
    while True:
        ch = ser.read()
        sys.stdout.write(ch)
        sys.stdout.flush()
        if ch == '\n' or len(ch) == 0:
            break
        if ch != '\r':
            response += ch
    return response

def chunkify(l, n):
    n = max(1, n)
    return (l[i:i+n] for i in xrange(0, len(l), n))

def checksum(st):
    return reduce(lambda x,y:x+y, map(ord, st))

# Download protocol:
# Host: OD<FILENAME> 7\n character max no extension
# Holo: ACK\n
# Host: LEN<length>\n
# Loop until <length> has been transmitted
#   Host: ACK<checksum>\n
#   Host: [Base64 encoded packet up to 4096 bytes in length]\
#   Holo: ACK|NAK
def handleHoloOLED(ser):
    while True:
        global command
        global fileName
        global fileNameNoExt
        value = 0
        response = readResponse(ser)
        sys.stdout.write('[HOLO] \'')
        sys.stdout.write(response)
        sys.stdout.write('\'\n')
        sys.stdout.flush()
        if response == 'READY':
            if command == 'upload':
                success = False
                ser.write("OD"+fileNameNoExt+"\n")
                response = readResponse(ser)
                if response == 'ACK':
                    with open(fileName, "rb") as image_file:
                       encoded_string = base64.b64encode(image_file.read())
                    ser.write('LEN')
                    ser.write(str(len(encoded_string)))
                    ser.write('\n')
                    sys.stdout.write('Sending file size: ')
                    sys.stdout.write(str(len(encoded_string)))
                    sys.stdout.write('\n')
                    for ci in xrange(0, len(encoded_string), 4096):
                       chunk = encoded_string[ci:ci+4096]
                       sum = checksum(chunk)
                       ser.write('ACK')
                       ser.write(str(sum))
                       ser.write('\n')
                       ser.write(chunk)
                       ser.write('\n')
                       percent = int(float(ci) / len(encoded_string) * 100)
                       sys.stdout.write(str(percent))
                       sys.stdout.write('% complete\n')
                       response = readResponse(ser)
                       if response != 'ACK':
                         sys.stdout.write('Fail1: ')
                         sys.stdout.write(response)
                         sys.stdout.write('\n')
                         success = False
                         break
                       success = True
                else:
                    sys.stdout.write('Fail2: ')
                    sys.stdout.write(response)
                    sys.stdout.write('\n')
                if success:
                    sys.stdout.write('Success\n')
                else:
                    sys.stdout.write('Failed\n')
            elif command == 'delete':
                ser.write("OX"+fileNameNoExt+"\n")
            elif command == 'play':
                ser.write("OP"+fileNameNoExt+"\n")
            elif command != '':
                ser.write(command+"\n")
            command = ''

print 'Waiting for HoloOLED to connect'
connected = False
while not connected:
    try:
        ser_stealth = serial.Serial(
            port=serialport,
            baudrate=19200,
            rtscts=False)
        connected = True
        print
        print 'Connected'
        handleHoloOLED(ser_stealth)
    except SerialException:
        if connected:
            print 'Disconnected'
            connected = False
