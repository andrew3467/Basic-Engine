
for %%i in (*.vert *.frag) do "C:\VulkanSDK\1.3.216.0\Bin\glslc.exe" "%%~i" -o "%%~i.spv"
