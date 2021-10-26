# 2dGame
2d game created from "C++ Game Development by Example" book

Important instruction

In order to ensure that everything is working correctly, need to ensure that the include directories and library directories are setup, and the linker is setup. 
I likely didn't need to do all of these, but I did it to ensure that everything worked correctly.  To set them up, make sure to:

1)Open the project settings
2)Under Configuration Properties -> VC++ Directories, edit the Include Directories to be $(ProjectDir)\SFML-2.5.1\include
3)Under same section, make sure to update the Library Directories to be $(ProjectDir)\SFML-2.5.1\lib
4)Under Configuration Properties -> C/C++ -> General, update the Additional Include Directories to (FileLocationOrder)\SFML-2.5.1\include.  Example C:\Users\(Name)\source\repos\SFMLProject\SFML-2.5.1\include or where ever it is located.
5)Under Configuration Properties -> Linker -> General, update the Additional Library Directories to (FileLocationOrder)\SFML-2.5.1\lib
6)Under Configuration Properties -> Linker -> Input, add sfml-graphics-d.lib;sfml-window-d.lib;sfml-system-d.lib;sfml-network-d.lib;sfml-audio-d.lib;kernel32.lib;user32.lib;gdi32.lib to the
      ADDITIONAL DEPENDENCIES section.
7)Make sure that Platform is set correctly as well.  Either Win32 or the x64 setting.
