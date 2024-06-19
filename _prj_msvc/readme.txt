Удаляем из путей инклюдов все ссылки на encoding
Удаляем из путей инклюдов все ссылки на sfmt\inc

Удаляем все сорцы umba, sfmt, encoding

  <ItemGroup>
    <ClCompile Include="..\src\umba-md-pp\umba-md-pp.cpp" />
  </ItemGroup>
  <!-- Сорцы закончились -->

  <!-- Вставляем ссылки на пропсы библиотек -->
  <Import Project="$(UMBA_LOCAL_LIBS_ROOT)\umba\.msvc\sources.props" />
  <Import Project="$(UMBA_LOCAL_LIBS_ROOT)\sfmt\.msvc\sources.props" />
  <Import Project="$(UMBA_LOCAL_LIBS_ROOT)\encoding\.msvc\sources.props" />
