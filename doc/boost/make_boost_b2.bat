cd %~dp0\boost
.\b2 --build-dir="build64" --toolset=msvc-10.0 link=shared runtime-link=static --build-type=complete architecture=x86 address-model=64 --stagedir="lib/win64/dynamic_sruntime" stage > .\dynamic_sruntime_x86_64.log.txt

cd %~dp0\
