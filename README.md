# AmibrokerPython
Amibroker Plugin DLL that embeds the Python 3.4 interpreter for use within Amibroker.
Based on the original Sample project included in the Amibroker SDK.

Usage requires Python 3.4 to be installed on your machine.  Tested with Anaconda 3.4
installer.  Anaconda was installed to C:\Python3 on a Windows 7 machine.  Project
defaults to that location.

Consider this a work in progress.  It works, and allows one to execute wxternal Python
scripts from AFL, but I haven't implemented any sharing of data arrays between AFL 
and Python yet.

