@echo off
setlocal enableextensions enabledelayedexpansion

for /f "usebackq tokens=2 delims=:" %%i in (`chcp`) do set SAVED_CODE_PAGE=%%~i
echo "%SAVED_CODE_PAGE%"
chcp 65001

git add "README.md"
git add "Алгоритм балансировки (Гайдашёв).md"
git add "Алгоритм балансировки (Мартынов).md"
git add "Алгоритмы.md"
git add "Архитектура ПО батарейного балансера на базе Framework.md"
git add "Дополнение к техническому заданию.md"
git add "Дополнительные ресурсы по проекту.md"
git add "Описание периферии устройства.md"
git add "План-график.md"
git add "Протокол обмена информацией между платами.md"
git add "Техническое задание.md"
git add "C:\work\projects\firmware\firmware_ES_4007_BMCL\wiki\index.md"
git add "C:\work\projects\firmware\firmware_ES_4007_BMCL\wiki\img\balancing_graphic.png"

chcp %SAVED_CODE_PAGE%