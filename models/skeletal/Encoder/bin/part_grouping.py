#!/usr/bin/env python2.7
# ###############################################################
#  oteng kwaku  <otengkwaku@gmail.com>
# This is to automate the process of
# generating a part_info and a grouping file for use with the
# open-3d-viewer api
#
# ###############################################################


import re
import os

#get file name or directory for the obj file
objFile = str(raw_input('Enter .obj file name or directory\n'))
outputFile = 'parts_info.txt'
groupfile = 'groupings.txt'
layerList = []

# change the print info during a loop
def seter(counter,defultText,nextText):
    if counter == 0:
        return defultText
    else:
        return nextText


    
#charking to see if the files exsit or not
def FileChark(FILE):
    if os.path.exists(FILE):
        f = file(FILE,'w')
    else:
        f = file(FILE,'a')
    return f

#part creation function 
def Partgene(INPUTFILE,OUTPUTFILE):
    print('BUILDING PARTS_INFO FILE\n')
    f = file(INPUTFILE,'r')
    r = f.read()
    f.close()
    global Garr
    Garr = re.findall(r'[^\S]g\s(.+)',r) #creating an array of all the group and object names
    Garr += re.findall(r'[^\S]o\s(.+)',r)
    c_Garr = Garr[:]
    Garr = [i.lower() for i in c_Garr]
    index = 5000
    f = FileChark(OUTPUTFILE)
    f.write('# Parts\n\n')
    TestDisplayName = int(raw_input('Type\"1\" if you want a diplay name for your parts, if not type \"0\": '))
    for w in Garr:
        index += 1
        f.write(w+'\n'+ '\tid: '+ str(index)+ '\n'+'\ttype: part\n')
        if TestDisplayName == 1:
            print(w)
            displayName = str(raw_input('Type a name for the part above: '))
            f.write('\tdisplay_name_en_us: '+ displayName+ '\n\n')
        elif TestDisplayName == 0:
            f.write('\n')
    return f

# layer creation
def layerGen(FILEOBJECT):
    f = FILEOBJECT
    f.write('# Layers\n\n')
    layerId = 10
    test = 0
    global numLayers
    numLayers = int(raw_input('How many layers do you have: '))
    a = 0 # variable to make the seter run it defult once
    while test < numLayers:
        b = 'Type the name of your layer and press enter NOTE: one layer at a time: '
        c = 'Name of next layer: '
        layer = str(raw_input(seter(a,b,c)))# to print the requed text
        layDisName = str(raw_input('Enter the display name for the layer above: '))
        f.write(layer+'\n'+'\tid: '+str(layerId)+'\n'+'\ttype: group\n\tdisplay_name_en_us: '+layDisName+'\n')
        f.write('\tlayer: yes\n\thidden: yes\n\n')
        layerId +=1
        test +=1
        a +=1
        layerList.append(layer)
    f.write('# Groups\n\n')
    global groupName
    groupName = str(raw_input('Enter the name for your model: '))
    while 1:
        if groupName in Garr:
            print 'Error ',groupName , ' in Model'
        else:
            break
        groupName = str(raw_input('Enter the name for your model: '))
        
    f.write(groupName+ '\n\tid: 1\n\ttype: group\n\thidden : yes')
    f.close()


# grouping list file generation
# creating grouping for the model and the layers
def groupFile (FILE):
    print('\nBUILDING GROUPING FILE')
    f = FileChark(FILE)
    f.write(groupName)
    print('Below is an arry of your layers starting with an index of 1 to '+str(len(layerList)))
    print(layerList)
    i = 0
    a = 0
    while i < numLayers:
          b = 'Type the index of your first layer in the order you will like it to appear and press enter: '
          c = 'Type the index of the next layer: '
          s = int(raw_input(seter(a,b,c)))
          s -= 1
          f.write('\n\t'+ layerList[s])
          i += 1
          a +=1
    return f

# grouping for layers and parts
def groupParts(FILEOBJECT):
    f = FILEOBJECT
    print('\nYOU ARE GOING TO ARRANGE THE PARTS UNDER THEIR LAYERS USING THEIR INDEX\n')
    print('All arrays have an index starting with 1\n')
    print('\nBelow is the array of LAYERS:')
    print(layerList)
    print('\nBelow is the array of PARTS: ')
    print(Garr)
    test = 0
    pnum = len(Garr)
    num = len(layerList)
    while test < len(layerList):
        b = 'Type the index of your first layer: '
        c = 'Type the index of the next layer: '
        print('Number of LAYERS left: '+str(num))
        f.write('\n\n'+layerList[(int(raw_input(seter(test,b,c)))-1)])
        print('Number of PARTS left: '+str(pnum))
        s = int(raw_input('How many parts do you have under this layer: '))
        a = 0
        for e in range(s):
            d = 'Type the index of the part under this layer NOTE: ONE AT A TIME: '
            e = 'Type the index for the next part: '
            f.write('\n\t'+Garr[(int(raw_input(seter(a,d,e)))-1)])
            a += 1
        test += 1
        pnum -= s
        num -=1
    
layerGen(Partgene(objFile,outputFile))
groupParts(groupFile(groupfile))
print('PART_INFO.TXT AND GROUPING.TXT FILES GENERATED')

