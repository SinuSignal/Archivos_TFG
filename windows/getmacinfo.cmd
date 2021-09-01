@echo off
echo ----------- >>%1
echo %date% %time% >>%1
pause
netsh wlan show interfaces | findstr /i /c:"Dirección" /c:"SSID" >> %1
pause
netsh interface ip show config name="Wi-fi" | findstr "Dirección IP" >>%1


