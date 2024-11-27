import subprocess
import os

msbuild_path = "C:/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe"

def build(sln_file :str, args :list = []) -> bool:
    defaults = ["/maxcpucount:8"]
    try:
        result = subprocess.run([msbuild_path] + [sln_file] + defaults + args, shell=True)
        return result.returncode != 1

    except Exception as e:
        print(e)
        return False


def build_chem_debug86() -> bool:
    success = build("../Chemgine.sln", 
    [
        "/p:Configuration=Debug",
        "/p:Platform=x86"
    ])

    if(success):
        print("Tools > build > debug > x86: SUCCESS")
    else:
        print("Tools > build > debug > x86: FAIL")
        print("\n----------------------------------------------------------------------------\n\n")
    return success


def build_chem_debug64() -> bool:
    success = build("../Chemgine.sln", 
    [
        "/p:Configuration=Debug",
        "/p:Platform=x64"
    ])

    if(success):
        print("Tools > build > debug > x64: SUCCESS")
    else:
        print("Tools > build > debug > x64: FAIL")
        print("\n----------------------------------------------------------------------------\n\n")
    return success


def build_chem_release86() -> bool:
    success = build("../Chemgine.sln", 
    [
        "/p:Configuration=Release",
        "/p:Platform=x86"
    ])

    if(success):
        print("Tools > build > release > x86: SUCCESS")
    else:
        print("Tools > build > release > x86: FAIL")
        print("\n----------------------------------------------------------------------------\n\n")
    return success



def build_chem_release64() -> bool:
    success = build("../Chemgine.sln", 
    [
        "/p:Configuration=Release",
        "/p:Platform=x64"
    ])

    if(success):
        print("Tools > build > release > x64: SUCCESS")
    else:
        print("Tools > build > release > x64: FAIL")
        print("\n----------------------------------------------------------------------------\n\n")
    return success
