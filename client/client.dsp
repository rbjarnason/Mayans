# Microsoft Developer Studio Project File - Name="client" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=client - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "client.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "client.mak" CFG="client - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "client - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "client - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "client - Win32 ReleaseDebug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "client - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 dinput8.lib libxml2.lib freetype.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"G:\code\Mayan Pyramid\client\mayans.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "client - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "__STL_DEBUG" /FR /YX /FD /GZ /Zm200 /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 freetype.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/snowballs.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "client - Win32 ReleaseDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "client___Win32_ReleaseDebug"
# PROP BASE Intermediate_Dir "client___Win32_ReleaseDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseDebug"
# PROP Intermediate_Dir "ReleaseDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"msvcrtd.lib"
# ADD LINK32 libxml2.lib freetype.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /out:"G:\code\Mayan Pyramid\client\mayans_rd.exe"
# SUBTRACT LINK32 /profile /pdb:none

!ENDIF 

# Begin Target

# Name "client - Win32 Release"
# Name "client - Win32 Debug"
# Name "client - Win32 ReleaseDebug"
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\avatar_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\avatar_interface.h
# End Source File
# Begin Source File

SOURCE=.\src\cluster_display.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cluster_display.h
# End Source File
# Begin Source File

SOURCE=.\src\combat_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\combat_interface.h
# End Source File
# Begin Source File

SOURCE=.\src\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\src\commands.h
# End Source File
# Begin Source File

SOURCE=.\src\exploring_interface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\exploring_interface.h
# End Source File
# Begin Source File

SOURCE=.\src\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\src\graph.h
# End Source File
# Begin Source File

SOURCE=.\src\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\interface.h
# End Source File
# Begin Source File

SOURCE=.\src\inventory_display.cpp
# End Source File
# Begin Source File

SOURCE=.\src\inventory_display.h
# End Source File
# Begin Source File

SOURCE=.\src\radar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\radar.h
# End Source File
# Begin Source File

SOURCE=.\src\skills_display.cpp
# End Source File
# Begin Source File

SOURCE=.\src\skills_display.h
# End Source File
# Begin Source File

SOURCE=.\src\stats_display.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stats_display.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\18.textskills
# End Source File
# Begin Source File

SOURCE=.\18.textstats
# End Source File
# Begin Source File

SOURCE=.\src\animation.cpp
# End Source File
# Begin Source File

SOURCE=.\src\animation.h
# End Source File
# Begin Source File

SOURCE=.\src\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\src\camera.h
# End Source File
# Begin Source File

SOURCE=.\client.cfg
# End Source File
# Begin Source File

SOURCE=.\src\client.cpp
# End Source File
# Begin Source File

SOURCE=.\src\client.h
# End Source File
# Begin Source File

SOURCE=.\src\client_world_objects.cpp
# End Source File
# Begin Source File

SOURCE=.\src\client_world_objects.h
# End Source File
# Begin Source File

SOURCE=.\src\compass.cpp
# End Source File
# Begin Source File

SOURCE=.\src\compass.h
# End Source File
# Begin Source File

SOURCE=.\src\entities.cpp
# End Source File
# Begin Source File

SOURCE=.\src\entities.h
# End Source File
# Begin Source File

SOURCE=.\src\envfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\envfx.h
# End Source File
# Begin Source File

SOURCE=.\src\inventory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\inventory.h
# End Source File
# Begin Source File

SOURCE=.\src\landscape.cpp
# End Source File
# Begin Source File

SOURCE=.\src\landscape.h
# End Source File
# Begin Source File

SOURCE=.\src\lens_flare.cpp
# End Source File
# Begin Source File

SOURCE=.\src\lens_flare.h
# End Source File
# Begin Source File

SOURCE=.\src\mouse_listener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\mouse_listener.h
# End Source File
# Begin Source File

SOURCE=.\src\network.cpp
# End Source File
# Begin Source File

SOURCE=.\src\network.h
# End Source File
# Begin Source File

SOURCE=.\src\pacs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pacs.h
# End Source File
# Begin Source File

SOURCE=.\src\physics.cpp
# End Source File
# Begin Source File

SOURCE=.\src\physics.h
# End Source File
# Begin Source File

SOURCE=.\src\skills.cpp
# End Source File
# Begin Source File

SOURCE=.\src\skills.h
# End Source File
# Begin Source File

SOURCE=.\src\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sound.h
# End Source File
# Begin Source File

SOURCE=.\src\spells.cpp
# End Source File
# Begin Source File

SOURCE=.\src\spells.h
# End Source File
# Begin Source File

SOURCE=.\src\stats.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stats.h
# End Source File
# Begin Source File

SOURCE=.\src\tracers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tracers.h
# End Source File
# Begin Source File

SOURCE=.\src\world_objects.cpp
# End Source File
# Begin Source File

SOURCE=.\src\world_objects.h
# End Source File
# Begin Source File

SOURCE=.\src\world_types.cpp
# End Source File
# Begin Source File

SOURCE=.\src\world_types.h
# End Source File
# End Target
# End Project
