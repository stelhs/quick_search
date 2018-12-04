#!/usr/bin/python
import glob
import sys
import os


class bcolors:
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def checkByte(byte):
    if byte == 13:
        return True
    if byte == 10:
        return True
    if byte == 9:
        return True
    if byte >= 32 and byte <= 126:
        return True
    return False


def readText(filename):
    text = ""
    f = open(filename, "r")
    f.seek(-1,2)
    eof = f.tell()
    f.seek(0,0)
    cnt = 0

    while f.tell() != eof:
        b = f.read(1)
        cnt += 1
        if checkByte(ord(b)):
            text += b
            continue
        f.close()
        return None
    return text


def serachDir(dir, serachText):
    items = glob.glob("%s/*" % dir)
    for item in items:
        item = os.path.basename(item)
        path = "%s/%s" % (dir, item)
        if os.path.isdir(path):
            serachDir(path, serachText)
            continue
        file = path

        text = readText(file)
        if text == None:
            continue

        rows = text.split('\n')
        rowNum = 0
        for row in rows:
            if row.find(serachText) != -1:
                row = row.replace(serachText, bcolors.YELLOW + serachText + bcolors.ENDC)
                print("%s +%d: %s" % (bcolors.GREEN + file + bcolors.ENDC, rowNum, row))

            rowNum += 1



def main():
    serachText = sys.argv[1]
    serachDir(os.getcwd(), serachText)




main()
