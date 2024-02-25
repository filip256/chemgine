from utils.av_check import *
from utils.build import *

files = []
if build_chem_debug64() == True:
    files.append("../Chemgine/x64/Debug/Chemgine.exe")
if build_chem_release64() == True:
    files.append("../Chemgine/x64/Release/Chemgine.exe")

asyncio.run(vt_check_files(files))