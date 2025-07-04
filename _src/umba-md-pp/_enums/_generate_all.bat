@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@rem same but more detailed
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set FMTDEC=--enum-flags=fmt-dec
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set SERIALIZE_CAMEL=--serialize-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle
@set SERIALIZE_HYPHEN=--serialize-style=HyphenStyle


@set SNIPPETOPTIONS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL% --enum-serialize-style=All



umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% %SNIPPETOPTIONS_GEN_FLAGS% ^
    %UINT32% %HEX4% -E=SnippetOptions                -F=@SnippetOptions.txt               ^
    %UINT32% %HEX4% -E=SnippetOptionsParsingResult   -F=@SnippetOptionsParsingResult.txt  ^
    %UINT32% %HEX4% -E=ConditionOperators            -F=@ConditionOperators.txt           ^
    %UINT32% %HEX4% -E=ListingNestedTagsMode         -F=@ListingNestedTagsMode.txt        ^
    %UINT32% %HEX4% -E=ProcessingOptions             -F=@ProcessingOptions.txt            ^
    %UINT32% %HEX4% -E=TargetRenderer                -F=@TargetRenderer.txt               ^
    %UINT32% %HEX4% -E=LineHandlerEvent              -F=@LineHandlerEvent.txt             ^
    %UINT32% %HEX4% -E=PreprocessorParsingState      -F=@PreprocessorParsingState.txt     ^
    %UINT32% %HEX4% -E=MetaTagType                   -F=@MetaTagType.txt                  ^
    %UINT32% %HEX4% -E=TargetFormat                  -F=@TargetFormat.txt                 ^
    %UINT32% %HEX4% -E=BacktickProcessingState       -F=@BacktickProcessingState.txt      ^
    %UINT32% %HEX4% -E=PreprocessorDirective         -F=@PreprocessorDirective.txt        ^
    %UINT32% %HEX4% -E=SnippetTagType                -F=@SnippetTagType.txt               ^
    %UINT32% %HEX4% -E=CodeProcessingHandlerType     -F=@CodeProcessingHandlerType.txt    ^
    %SERIALIZE_HYPHEN%                                                                    ^
    %UINT32% %HEX4% -E=MdPpTag                       -F=@MdPpTag.txt                      ^
    %UINT32% %HEX4% -E=GraphVizTargetFormat          -F=@GraphVizTargetFormat.txt         ^
    %UINT32% %HEX4% -E=GraphType                     -F=@GraphType.txt                    ^
    %UINT32% %HEX4% -E=PlantUmlTargetFormat          -F=@PlantUmlTargetFormat.txt         ^
    %UINT32% %HEX4% -E=PlantUmlDiagramType           -F=@PlantUmlDiagramType.txt          ^
    %UINT32% %HEX4% -E=ArgListType                   -F=@ArgListType.txt                  ^
    %UINT32% %HEX4% -E=ArgListValueStyle             -F=@ArgListValueStyle.txt            ^
    %UINT32% %HEX4% -E=TableCellAlignment            -F=@TableCellAlignment.txt           ^
    %UINT32% %HEX4% -E=CsvQuot                       -F=@CsvQuot.txt                      ^
    %UINT32% %HEX4% -E=CsvSeparator                  -F=@CsvSeparator.txt                 ^
    %UINT32% %HEX4% -E=CsvTitle                      -F=@CsvTitle.txt                     ^
..\enums.h

@rem Что-то пошло не так и файл генерится в текущий каталог
if exist %~dp0enums.h copy /Y %~dp0enums.h %~dp0..\enums.h
if exist %~dp0enums.h del %~dp0enums.h
