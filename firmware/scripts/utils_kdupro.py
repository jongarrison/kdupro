#See extra_scripts docs: https://docs.platformio.org/en/latest/projectconf/advanced_scripting.html
# env: dict
# Import: function

Import("env")
import time
from platformio import exception, fs, util
import serial
import os

#print(env.Dump())

serialPortSubstring = "CP2104" #This is info that will be found in the port description
#On a mac, the port name will look like: /dev/cu.usbserial-02861555
baudrate = 9600

def get_upload_port_name():
    uploadPort = env.get("UPLOAD_PORT")
    
    if uploadPort is not None:
        return uploadPort
    
    serialPorts = util.get_serial_ports()
    for portInfo in serialPorts:
        if serialPortSubstring in portInfo["description"]:
            uploadPort = portInfo["port"]
            return uploadPort

    return serialPorts[-1] #This is based on platformio's own logic of choosing the last port when no upload_port is specified

def get_rtc_style_time():
    return time.strftime("%Y%m%d%H%M%S")


def setclock_callback(*args, **kwargs):
    uploadPortName = get_upload_port_name()
    currentTimeStr = get_rtc_style_time()
    print("Setting clock on the device")
    print(f"{uploadPortName=}")
    print(f"{currentTimeStr=}")

    ser = serial.Serial(uploadPortName, baudrate)   
    # Send character 'S' to start the program

    command = f"T {currentTimeStr}\n"
    print(f"{command=}")
    ser.write(bytearray(command,'ascii'))
    time.sleep(0.5)
    while (ser.in_waiting > 0):
        result = ser.readline()
        print(f"{result=}")

    print(f"Checking device time several times:")
    for i in range(5):
        ser.write(b"GT\n")
        result = ser.readline()
        print(f"check {i}: {result=}")
        time.sleep(1)


def checkclock_callback(*args, **kwargs):
    uploadPortName = get_upload_port_name()
    print("Checking clock on the device")
    print(f"{uploadPortName=}")

    ser = serial.Serial(uploadPortName, baudrate)   

    ser.write(b"GT\n")
    time.sleep(0.5)
    result = ser.readline()
    print(f"{result=}")

def sendCommand(ser, command):
    print(f"SENDING: '{command}'")
    ser.write(bytearray(f"{command}\n",'UTF-8'))
    return getSerialDataUntilEOF(ser)

def pauseCollection(ser):
    sendCommand(ser, "PAUSE")

def getSerialDataUntilEOF(ser):
    print("Reading serial data...")
    result = ""
    while True:
        line = ser.readline().decode("utf-8").strip()
        print(f"RCVD: {line=}")
        if "EOF" in line:
            return result.strip()
        result += f"{line}\n"

def downloadfiles_callback(*args, **kwargs):
    uploadPortName = get_upload_port_name()
    print("Downloading files from the device")
    print(f"{uploadPortName=}")

    projectDir = env.get("PROJECT_DIR")
    print(f"{projectDir=}")

    ser = serial.Serial(uploadPortName, baudrate)   

    pauseCollection(ser)

    lsResult = sendCommand(ser, "LS")

    for filename in lsResult.split("\n"):
        print(f"{filename=}")

        if (filename.startswith(".") or filename == "settings.txt"):
            print(f"Skipping {filename}")
            continue

        filepath = os.path.join(projectDir, "data_download", filename);
        print(f"Saving to local file: '{filepath=}'")
        fileData = sendCommand(ser, "CAT " + filename)
        with open(filepath, "wb") as f:
            f.write(bytearray(fileData, "UTF-8"))

        #determine local file size:
        statinfo = os.stat(filepath)
        print(f"Local file size: {statinfo.st_size}")
        #Get remote file size:
        remoteSize = int(sendCommand(ser, f"SZ {filename}"))
        print(f"Remote file size: {remoteSize}")
        
        print(f"Downloaded {filename}")




#These scripts can be run from within the platformio environment by using:
# pio run -t setclock
env.AddCustomTarget("setclock", None, setclock_callback)
env.AddCustomTarget("checkclock", None, checkclock_callback)
env.AddCustomTarget("download", None, downloadfiles_callback)
