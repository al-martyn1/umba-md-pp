@if exist %~dp0\.vs  @rd /S /Q %~dp0\.vs
@start "" umba-md-pp-msvc2019.sln
