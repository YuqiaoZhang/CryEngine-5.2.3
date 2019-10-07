##############################################################
#             Automatic Animation Retargetting               #
##############################################################
#		Author: Dimitry Kachkovski                   #
#		Created: 07.07.08                            #
#		Company: Crytek GmbH                         #
##############################################################

from pyfbsdk import FBSystem, FBFilePopup, FBFilePopupStyle, FBFolderPopup, FBFbxManager, FBFbxManagerLoadAnimationMethod, FBPlotOptions, FBCharacterPlotWhere,FBFindModelByName, FBModelList, FBGetSelectedModels

# Lets define the actual loading and retargetting function

def LoadAndRetAnim (FileLoc,pOptions):
		manager = FBFbxManager()
		Char = FBSystem().Scene.Characters[0]
		manager.LoadAnimationOnCharacter(FileLoc,Char,0,0,1,1,FBFbxManagerLoadAnimationMethod.kFBFbxManagerLoadConnect,pOptions,0,0,0)
		Char.PlotAnimation(FBCharacterPlotWhere.kFBCharacterPlotOnControlRig,pOptions)
		Char.PlotAnimation(FBCharacterPlotWhere.kFBCharacterPlotOnSkeleton,pOptions)
		del (manager, Char)
		
# Unselect all function
def UnselAll():
		selModels = FBModelList()
		FBGetSelectedModels (selModels, None, True)
		for model in selModels:
				model.Selected = False;
		del(selModels)
		
# For the purpose of being able to clean up the file later on, we need to have
# all of the objects in the scene which need to stay there, be held in memory

UnselAll()

StartList = FBModelList()
StartListNames = list()
FBGetSelectedModels (StartList, None, False)
for model in StartList:
                StartListNames.append(model.Name)

# Now we need to assign animation plot options

pOpt = FBPlotOptions()
pOpt.UseConstantKeyReducer = False
pOpt.PlotAllTakes = True

#Filter needs to be set, otherwise it will crash

FileOpen = FBFilePopup()
FileOpen.Filter = '*.fbx'
FileOpen.Caption = "Select the animation to Retarget"
FileOpen.Style = FBFilePopupStyle.kFBFilePopupOpen
FileOpen.Path = r'J:\temp'

if FileOpen.Execute():
    File = FileOpen.FullFilename
    LoadAndRetAnim (File, pOpt)
else:
    print 'CANCEL'
    
#Get rid of imported trash

for name in StartListNames:
  FBFindModelByName(name).Selected = True

EndList = FBModelList()
FBGetSelectedModels (EndList, None, False)
for model in EndList:
		model.FBDelete()

UnselAll()
#for i in EndListNames:
#		a = FBFindModelByName(i)
#		a.FBDelete()

FBSystem().Scene.Characters[1].Components[0].FBDelete()
FBSystem().Scene.Characters[1].FBDelete()

# Cleanup.

del(StartList, StartListNames, EndList, EndListNames)
del(FBSystem, FBFilePopup, FBFilePopupStyle, FBFolderPopup, FBFbxManager, FBFbxManagerLoadAnimationMethod, FBPlotOptions, FBCharacterPlotWhere,FBFindModelByName, FBModelList, FBGetSelectedModels)