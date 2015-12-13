Set WshShell = WScript.CreateObject("WScript.Shell")

'WshShell.AppActivate "Google Chrome"
WshShell.Run "%windir%\notepad.exe"
WshShell.AppActivate "Notepad"

Function write(str)
	For i=1 To Len(str)
		WScript.Sleep(15)
		WshShell.SendKeys Mid(str, i, 1)
	Next
	
	WshShell.SendKeys "{ENTER}"
End Function

WScript.Sleep(100)

write "      ***********                  ***********"
write "   *****************            *****************"
write " *********************        *********************"
write "***********************      ***********************"
write "************************    ************************"
write "*************************  *************************"
write " **************************************************"
write "  ************************************************"
write "    ********************************************"
write "      ****************************************"
write "         **********************************"
write "           ******************************"
write "              ************************"
write "                ********************"
write "                   **************"
write "                     **********"
write "                       ******"
write "                         **"
write ""
write ""
write "I love you!"