from pyfbsdk import *

def UnselAll():
        selModels = FBModelList()
        FBGetSelectedModels (selModels, None, True)
        for model in selModels:
                model.Selected = False;
        del(selModels)
                

UnselAll()

Models = FBModelList()
Skeleton = list()
FBGetSelectedModels (Models, None, False)

for model in Models:
    if model.ClassName() == 'FBModelSkeleton':
        Skeleton.append(model)

Deter = FBFindModelByName("Bip01").Children[0].Name.split(":")        

for i in range(len(Skeleton)):  
        if Deter[1][5] == " ":
                SplName = Skeleton[i].Name.split(":")
                SplName[1] = SplName[1].replace(" ","_")
                Skeleton[i].Name = SplName[0] + ":" + SplName[1]
        else:
                SplName = Skeleton[i].Name.split(":")
                if SplName[1][0] == "_":
                        SplName[1] = SplName[1].replace("_"," ")
                        SplName[1] = SplName[1].replace(" ","_",1)
                        Skeleton[i].Name = SplName[0] + ":" + SplName[1]
                else:
                        SplName[1] = SplName[1].replace("_"," ")
                        Skeleton[i].Name = SplName[0] + ":" + SplName[1]
