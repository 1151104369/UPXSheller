@echo off
echo 测试UPX版本选择功能
echo.

REM 检查是否存在UPX版本目录
if exist "upx-5.0.1-win64\upx.exe" (
    echo 找到 upx-5.0.1-win64\upx.exe
) else (
    echo 未找到 upx-5.0.1-win64\upx.exe
)

if exist "upx-5.0.2-win32\upx.exe" (
    echo 找到 upx-5.0.2-win32\upx.exe
) else (
    echo 未找到 upx-5.0.2-win32\upx.exe
)

if exist "upx-5.0.2-win64\upx.exe" (
    echo 找到 upx-5.0.2-win64\upx.exe
) else (
    echo 未找到 upx-5.0.2-win64\upx.exe
)

if exist "upx\upx-5.0.1-win64\upx.exe" (
    echo 找到 upx\upx-5.0.1-win64\upx.exe
) else (
    echo 未找到 upx\upx-5.0.1-win64\upx.exe
)

echo.
echo 请将UPX版本文件夹放在程序目录下，格式如：
echo - upx-5.0.1-win64\upx.exe
echo - upx-5.0.2-win32\upx.exe
echo - upx-5.0.2-win64\upx.exe
echo 或者放在 upx\ 子目录下
echo.
pause