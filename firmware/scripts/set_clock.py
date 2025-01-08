#This script is run from within the platformio environment by using:
# pio run -t setclock

#See extra_scripts docs: https://docs.platformio.org/en/latest/projectconf/advanced_scripting.html
# env: dict
# Import: function

Import("env")
from platformio import exception, fs, util
import time

# host = env.GetProjectOption("custom_ping_host")

serialPortSubstring = "CP2104" #This is info that will be found in the port description
#On a mac, the port name will look like: /dev/cu.usbserial-02861555

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

    import serial
    ser = serial.Serial(uploadPortName, 9600)   
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

env.AddCustomTarget("setclock", None, setclock_callback)