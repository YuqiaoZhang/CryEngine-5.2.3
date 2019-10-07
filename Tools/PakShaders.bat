echo on

set zip=7za.exe
set cp=xcopy /S /V /I /F /Y

set rootdir=%CD%

set user=%rootdir%\USER
set user1=%rootdir%\USER1
set user2=%rootdir%\USER2
set shadersdir=Shaders\Cache\%1
set shadercachedir=Shadercache\%1
set usershadersdir=%user%\%shadersdir%

pushd %user%
%rootdir%\Tools\%zip% a -tzip -mx0 -r ..\ShaderCache.pak Shaders\Cache\%1\*.*
popd

set outdir=%user1%\%shadersdir%
mkdir %outdir%
pushd %usershadersdir%


%cp% Common.cfib %outdir%
%cp% FXConstantDefs.cfib %outdir%
%cp% FXSamplerDefs.cfib %outdir%
%cp% FXSetupEnvVars.cfib %outdir%
%cp% FXStreamDefs.cfib %outdir%
%cp% fallback.cfxb %outdir%
%cp% FixedPipelineEmu.cfxb %outdir%
%cp% Scaleform.cfxb %outdir%
%cp% Stereo.cfxb %outdir%
%cp% lookupdata.bin %outdir%
popd

set outdir=%user1%\%shadercachedir%
mkdir %outdir%
pushd %usershadersdir%

%cp% CGPShaders\Scaleform@* %outdir%\CGPShaders\
%cp% CGVShaders\Scaleform@* %outdir%\CGVShaders\
%cp% CGPShaders\Scaleform\* %outdir%\CGPShaders\
%cp% CGVShaders\Scaleform\* %outdir%\CGVShaders\
%cp% CGPShaders\FixedPipelineEmu@* %outdir%\CGPShaders\
%cp% CGVShaders\FixedPipelineEmu@* %outdir%\CGVShaders\
%cp% CGPShaders\FixedPipelineEmu\* %outdir%\CGPShaders\
%cp% CGVShaders\FixedPipelineEmu\* %outdir%\CGVShaders\
%cp% CGPShaders\Stereo@* %outdir%\CGPShaders\
%cp% CGVShaders\Stereo@* %outdir%\CGVShaders\
%cp% CGPShaders\Stereo\* %outdir%\CGPShaders\
%cp% CGVShaders\Stereo\* %outdir%\CGVShaders\
%cp% lookupdata.bin* %outdir%
popd

set outdir=%user2%\%shadersdir%
mkdir %outdir%
pushd %usershadersdir%
%cp% *.cfib %outdir%\
%cp% *.cfxb %outdir%\
popd

pushd %user1%
%rootdir%\Tools\%zip% a -tzip -mx0 -r ..\ShaderCacheStartup.pak Shaders\Cache\%1\*
%rootdir%\Tools\%zip% a -tzip -mx0 -r ..\ShaderCacheStartup.pak Shadercache\%1\*
popd

pushd %user2%
%rootdir%\Tools\%zip% a -tzip -mx0  -r ..\ShadersBin.pak Shaders\Cache\%1\*.cfib
%rootdir%\Tools\%zip% a -tzip -mx0  -r ..\ShadersBin.pak Shaders\Cache\%1\*.cfxb
popd
