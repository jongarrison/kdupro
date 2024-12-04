#This script is run from within the platformio environment by using:
# pio run -t setclock

#See extra_scripts docs: https://docs.platformio.org/en/latest/projectconf/advanced_scripting.html
# env: dict
# Import: function

Import("env")
from platformio import exception, fs, util
import time

# host = env.GetProjectOption("custom_ping_host")

serialPortSubstring = "CP2104"

def get_upload_port():
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
    uploadPort = get_upload_port()
    currentTimeStr = get_rtc_style_time()
    print(f"{uploadPort=}")
    print(f"{currentTimeStr=}")

env.AddCustomTarget("setclock", None, setclock_callback)