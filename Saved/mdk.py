import sys
import json
import shutil
import os

# log functions
def error(str):
    print("[ERROR] " + str)
    exit(1)

# load config
try:
    f = open("config.json","r+")
    config = json.load(f)
    f.close()
except:
    config = {
        "gameRoot": "C:\\Program Files\\Epic Games\\Satisfactory",
        "projectRoot": "C:\\Users\\User\\Documents\\UnrealProjects\\LightItUp"
    }
    f = open("config.json", "w+")
    json.dump(config, f)
    f.close()

cmd = None
param = None
for arg in sys.argv[1:]:
    if param:
        if param == "g":
            config["gameRoot"] = arg
            param = None
            continue
        elif param == "p":
            config["projectRoot"] = arg
            param = None
            continue
        else:
            error("Unknown parameter '" + param + "'")
    if arg.startswith("-"):
        param = arg[1:]
    elif cmd:
        error("only one command can be executed at once")
    else:
        cmd = arg

def cook():
    shutil.rmtree(config["projectRoot"] + "\\Saved\\Pak\\FactoryGame")
    try:
        f = open("cook-files.txt","r+")
    except:
        error("no cook-files.txt found! can't cook files")
    lines = f.readlines()
    path = []
    i = 0
    while i < len(lines):
        line = lines[i]
        while line.endswith("\n"):
            line = line[:len(line)-1]
        i = i + 1
        space = 0
        while line.startswith("\t") or line.startswith("    "):
            if line[0] == "\t":
                line = line[1:]
            else:
                line = line[4:]
            space = space + 1
        if len(path) < space:
            error("cook-files.txt parsing error")
        path = path[:space]
        if line.endswith(":"):
            path.append(line[:len(line)-1])
        else:
            s = ""
            for p in path:
                s = s + p + "\\"
            s = s + line
            print("Copy '" + s + "'...")
            try:
                os.makedirs(config["projectRoot"] + "\\Saved\\Pak\\FactoryGame\\Content\\" + s[:s.rfind("\\")])
            except:
                print("Meep")
            shutil.copyfile(config["projectRoot"] + "\\Saved\\Cooked\\WindowsNoEditor\\FactoryGame\\Content\\" + s + ".uasset", config["projectRoot"] + "\\Saved\\Pak\\FactoryGame\\Content\\" + s + ".uasset")
            shutil.copyfile(config["projectRoot"] + "\\Saved\\Cooked\\WindowsNoEditor\\FactoryGame\\Content\\" + s + ".uexp", config["projectRoot"] + "\\Saved\\Pak\\FactoryGame\\Content\\" + s + ".uexp")

def pack():
    os.chdir(config["projectRoot"] + "\\Saved\\Pak")
    os.system("u4pak.py pack LightItUp.pak FactoryGame");

def test():
    shutil.copyfile(config["projectRoot"] + "\\Saved\\Pak\\LightItUp.pak", config["gameRoot"]+ "\\FactoryGame\\Content\\Paks\\LightItUp.pak")

if cmd == "cook":
    cook()
elif cmd == "pack":
    cook()
    pack()
elif cmd == "test":
    cook()
    pack()
    test()
else:
    error("command not found")