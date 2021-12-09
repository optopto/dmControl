file = 'dmdControl.cpp';
includePath = '-IC:\Program Files\IVI Foundation\VISA\Win64\Include';
libPath = '-LC:\Program Files\IVI Foundation\VISA\Win64\Lib_x64\msc';
includePath32 = '-IC:\Program Files (x86)\IVI Foundation\VISA\WinNT\Include';
libPath32 = '-LC:\Program Files (x86)\IVI Foundation\VISA\WinNT\Lib\msc';
mex('-v',includePath,libPath, '-lTLDFM_64' , '-lTLDFMX_64',file)
clearvars;
fprintf("Compilado con éxito \n")