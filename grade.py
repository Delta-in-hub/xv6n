#! /usr/bin/env python3

# OSLAB
# 用于学生测试和提交作业的脚本


#! NOT MODIFY ANYTHING IN THIS FILE

import os
import signal
import subprocess
import sys
from subprocess import Popen
import tarfile

# test server address and port
SERVER_ADDR = "127.0.0.1"
SERVER_PORT = 8000


TOKEN = ""
LAB = ""


def setLabToken():
    with open("./conf/lab.mk", "r") as f:
        global TOKEN
        global LAB

        try:
            for line in f.readlines():
                (k, v) = line.strip().split("=")
                k = k.strip()
                v = v.strip()
                if k == "LAB":
                    LAB = v
                elif k == "TOKEN":
                    TOKEN = v
        except Exception as e:
            pass
        if (TOKEN == "" or LAB == ""):
            print("Error: conf/lab.mk not found or invalid format")
            print("conf/lab.mk Example:")
            print("LAB = lab1-1")
            print("TOKEN = 2019xxxx-a123bf4ff789999...")
            print()


def safeRemove(path: str):
    if (os.path.exists(path)):
        try:
            os.remove(path)
        except Exception as e:
            pass


def my_handler(signum, frame):
    if signum in [signal.SIGINT, signal.SIGTERM, signal.SIGQUIT, signal.SIGABRT, signal.SIGTSTP, signal.SIGSTOP, signal.SIGKILL]:
        # rm gradelib.py
        # rm {LAB}.py
        safeRemove("gradelib.py")
        safeRemove(f"{LAB}.py")
        # {LAB}-{TOKEN}.tar.gz
        safeRemove(f"{LAB}-{TOKEN}.tar.gz")
        exit(0)


def setSignal():
    signal.signal(signal.SIGINT, my_handler)
    signal.signal(signal.SIGTERM, my_handler)
    signal.signal(signal.SIGQUIT, my_handler)
    signal.signal(signal.SIGABRT, my_handler)


def myExit():
    my_handler(signal.SIGINT, None)


def genCurlGetCmd(path: str):
    path = path.strip()
    if (path.startswith("/")):
        path = path[1:]
    return ("curl", "-s", "-L", "-X", "GET", f"http://{SERVER_ADDR}:{SERVER_PORT}/{path}")


"""
curl -X 'POST' \
  'http://127.0.0.1:8000/handin' \
  -H 'accept: application/json' \
  -H 'Content-Type: application/json' \
  -d '{
  "token": "20194755-9584ae5c8e17423f9ba7919c23799a53",
  "lab": "lab1-1",
  "content": "string"
}'
"""


def genCurlPostHandinCmd(toekn: str, lab: str, content: str):

    content = content.encode('utf-8').hex()

    # print(bytes.fromhex(content).decode('utf-8'))

    jc = f"{{ \"token\": \"{toekn}\", \"lab\": \"{lab}\", \"content\": \"{content}\" }}"

    return ("curl", "-s", "-L", "-X", "POST", f"http://{SERVER_ADDR}:{SERVER_PORT}/handin", "-H", "accept: application/json", "-H", "Content-Type: application/json", "-d", jc)

# curl -X 'POST' \
#   'http://127.0.0.1:8000/uploadcode/20194755/lab2-1-hello' \
#   -H 'accept: application/json' \
#   -H 'Content-Type: multipart/form-data' \
#   -F 'code=@test.tar.gz;type=application/gzip'


def genCurlPostCodeCmd(token: str, lab: str, path: str):
    return ('curl', '-s', '-L', '-X', 'POST', f'http://{SERVER_ADDR}:{SERVER_PORT}/uploadcode/{token}/{lab}', '-H', 'accept: application/json', '-H', 'Content-Type: multipart/form-data', '-F', f'code=@{path};type=application/gzip')


def getGradelibPy():
    try:
        proc = Popen(genCurlGetCmd("/get/gradelib.py"), stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
        proc.wait()
        filecontent = proc.stdout.read().decode('utf-8')
        # print(filecontent)
        if len(filecontent) == 0:
            return None
        if filecontent.count('\n') < 2:
            return None
        return filecontent

    except Exception as e:
        return None


def getLabItem(name: str):
    try:
        proc = Popen(genCurlGetCmd(f"/get/{name}"), stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
        proc.wait()
        filecontent = proc.stdout.read().decode('utf-8')
        # print(filecontent)
        if len(filecontent) == 0:
            return None
        if filecontent.count('\n') < 2:
            return None
        return filecontent
    except Exception as e:
        return None


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def runtest():
    print()
    print(f"{bcolors.OKGREEN}==== Running test for {LAB} ===={bcolors.ENDC}")
    print()
    gradelib = getGradelibPy()
    labitem = getLabItem(f"{LAB}.py")

    if gradelib is None:
        print(f"{bcolors.FAIL}Error: get gradelib.py failed, check your network contenction {bcolors.ENDC}")
        myExit()

    if labitem is None:
        print(
            f"{bcolors.FAIL}Error: get {LAB}.py failed, maybe {LAB} is wrong {bcolors.ENDC}")
        myExit()

    with open("gradelib.py", "w") as f:
        f.write(gradelib)
    with open(f"{LAB}.py", "w") as f:
        f.write(labitem)
    try:
        proc = Popen(["python3", f"{LAB}.py"], stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
        content = ""
        while proc.stdout.readable():
            line = proc.stdout.readline()
            if len(line) == 0:
                break
            print(line.decode('utf-8'), end='')
            content += line.decode('utf-8')
        proc.wait()
    except Exception as e:
        print(e)
        myExit()
    print()
    color = bcolors.OKGREEN

    if "Score" not in content:
        color = bcolors.FAIL
        print(f"{color} {proc.stderr.read().decode('utf-8')} {bcolors.ENDC}")
        print(f"{color}==== Test for {LAB} Failed ===={bcolors.ENDC}")

    if "Score: 0/" in content:
        color = bcolors.FAIL

    print(f"{color}==== Test for {LAB} Finished ===={bcolors.ENDC}")
    return content


def traverse():
    filelist = []
    for root, dirs, files in os.walk("."):
        # print(root)
        path = root.split(os.sep)
        # skip folder
        dirname = path[1] if len(path) > 1 else ''
        if dirname in ['__pycache__', '.cache', '.vscode', 'docs', '.git']:
            continue
        # print((len(path) - 1) * '---', os.path.basename(root))
        for file in files:
            suffix = ['.sym', '.d', '.asm', '.o', '.img', '.tar.gz']
            if file.startswith('.') or file.startswith('_'):
                continue
            flag = False
            for s in suffix:
                if file.endswith(s):
                    flag = True
                    break
            if flag:
                continue
            # print(len(path) * '---', file)
            filelist.append(os.path.join(root, file))
    return filelist


def make_tarfile(output_filename, source_filelist):
    with tarfile.open(output_filename, "w:gz") as tar:
        for file in source_filelist:
            tar.add(file)


def handin():
    testres = runtest()
    if not testres or len(testres) == 0:
        print("Error: test result is empty , run test failed thus can't handin")
        myExit()
    print()

    if "Score: 0/" in testres:
        print(
            f"{bcolors.FAIL}==== Score 0, there is no need to handin ... ===={bcolors.ENDC}")
        return

    print(f"{bcolors.OKBLUE}==== Handin for {LAB} ===={bcolors.ENDC}")
    cmd = genCurlPostHandinCmd(TOKEN, LAB, testres)
    try:
        proc = Popen(cmd, stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
        content = ""
        while proc.stdout.readable():
            line = proc.stdout.readline()
            if len(line) == 0:
                break
            # print(line.decode('utf-8'), end='')
            content += line.decode('utf-8')
        proc.wait()
    except Exception as e:
        print(e)
        myExit()

    try:
        make_tarfile(f"{LAB}-{TOKEN}.tar.gz", traverse())
        cmd = genCurlPostCodeCmd(TOKEN, LAB, f"{LAB}-{TOKEN}.tar.gz")
        proc = Popen(cmd, stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
        uploadcontent = ""
        while proc.stdout.readable():
            line = proc.stdout.readline()
            if len(line) == 0:
                break
            print(line.decode('utf-8'))
            uploadcontent += line.decode('utf-8')
        proc.wait()
    except Exception as e:
        print(e)
        myExit()

    color = bcolors.OKBLUE

    if "score" in content and "course" in content and "username" in content:
        print(f"{bcolors.OKBLUE}==== Handin Success ===={bcolors.ENDC}")
    else:
        print(f"{bcolors.FAIL}==== Handin Failed ===={bcolors.ENDC}")
        color = bcolors.FAIL

    print(f"{bcolors.UNDERLINE} {content} {bcolors.ENDC}")
    print(f"{color}==== Handin for {LAB} Finished ===={bcolors.ENDC}")


if __name__ == "__main__":
    setLabToken()
    setSignal()
    if len(sys.argv) == 1:
        print("Usage: %s [ test | handin ]" % sys.argv[0])
        myExit()
    if sys.argv[1] == "test":
        runtest()
        myExit()
    elif sys.argv[1] == "handin":
        handin()
        myExit()
    else:
        print("Usage: %s [ test | handin ]" % sys.argv[0])
        myExit()
