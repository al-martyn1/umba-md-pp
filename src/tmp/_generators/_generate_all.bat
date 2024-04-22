@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@same but more detailed
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set SERIALIZE_CAMEL=--serialize-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle

@set DNDOCGENCMD_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set DNDOCGENCMD_DEF=invalid,unknown=-1;translation,tr=0;markdown,md;listing,lst;document,doc

@set LSTNESTEDTAGSMODE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set LSTNESTEDTAGSMODE_DEF=invalid,unknown=-1;keep;remove;empty,emptyLine

@set LSTUPDATETRMODE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set LSTUPDATETRMODE_DEF=invalid,unknown=-1;none;update;updateOnly

@set MDEXTRAPROCESSINGTAGS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set MDEXTRAPROCESSINGTAGS_DEF=invalid,unknown=-1;none=0;code,c;typeref,r


umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% ^
%DNDOCGENCMD_GEN_FLAGS%                %UINT32% -E=DocGenCommand                    -F=%DNDOCGENCMD_DEF%           ^
%LSTNESTEDTAGSMODE_GEN_FLAGS%          %UINT32% -E=ListingNestedTagsMode            -F=%LSTNESTEDTAGSMODE_DEF%     ^
%LSTUPDATETRMODE_GEN_FLAGS%            %UINT32% -E=ListingUpdateTrMode              -F=%LSTUPDATETRMODE_DEF%       ^
%MDEXTRAPROCESSINGTAGS_GEN_FLAGS%      %UINT32% -E=MdExtraProcessingTags            -F=%MDEXTRAPROCESSINGTAGS_DEF% ^
..\enums.h

