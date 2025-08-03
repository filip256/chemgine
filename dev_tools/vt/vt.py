from utils.av_check import *
from utils.build import *

files = []
if build_chem_release64() == True:
    files.append("../bin/x64/Release/GUIApp/GUIApp.exe")

with open("./keys/vt_api_key", "r") as file:
    vt_api_key = file.readline()
    asyncio.run(vt_check_files(files, vt_api_key))
    