from pyfbsdk import *

lSystem = FBSystem()

myCamera = FBCamera('charCamera')
myCamera.Parent = FBFindModelByName('Bip01')
myCamera.Show = True
myCamera.Selected = True

FBApplication().SwitchViewerCamera(myCamera)

myCamera.FrameSizeMode = FBCameraFrameSizeMode.kFBFrameSizeFixedResolution

myCamera.ResolutionHeight = 1080
myCamera.ResolutionWidth = 1920
myCamera.PixelAspectRatio = 1
myCamera.UseFrameColor = True