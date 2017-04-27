
{*******************************************************}
{                                                       }
{       Borland Delphi Run-time Library                 }
{       Win32 Rich Edit control Interface Unit          }
{                                                       }
{       Copyright (c) 1985-1999, Microsoft Corporation  }
{                                                       }
{       Translator: Inprise Corporation                 }
{                                                       }
{*******************************************************}

unit RichEdit;
{$ifdef fpc}
	{$mode delphi}
{$endif}
interface

uses Messages, Windows;

const
  cchTextLimitDefault     = 32767;


  RICHEDIT_CLASSA       = 'RichEdit20A';     { Richedit2.0 Window Class. }
  RICHEDIT_CLASSW       = 'RichEdit20W';     { Richedit2.0 Window Class. }
  RICHEDIT_CLASS = RICHEDIT_CLASSA;
  RICHEDIT_CLASS10A       = 'RICHEDIT';        { Richedit 1.0 }

{ RichEdit messages }

  WM_CONTEXTMENU                      = $007B;
  WM_PRINTCLIENT                      = $0318;

  EM_GETLIMITTEXT                     = WM_USER + 37;
//  EM_POSFROMCHAR                      = WM_USER + 38;
//  EM_CHARFROMPOS                      = WM_USER + 39;
  EM_SCROLLCARET                      = WM_USER + 49;
  EM_CANPASTE                         = WM_USER + 50;
  EM_DISPLAYBAND                      = WM_USER + 51;
  EM_EXGETSEL                         = WM_USER + 52;
  EM_EXLIMITTEXT                      = WM_USER + 53;
  EM_EXLINEFROMCHAR                   = WM_USER + 54;
  EM_EXSETSEL                         = WM_USER + 55;
  EM_FINDTEXT                         = WM_USER + 56;
  EM_FORMATRANGE                      = WM_USER + 57;
  EM_GETCHARFORMAT                    = WM_USER + 58;
  EM_GETEVENTMASK                     = WM_USER + 59;
  EM_GETOLEINTERFACE                  = WM_USER + 60;
  EM_GETPARAFORMAT                    = WM_USER + 61;
  EM_GETSELTEXT                       = WM_USER + 62;
  EM_HIDESELECTION                    = WM_USER + 63;
  EM_PASTESPECIAL                     = WM_USER + 64;
  EM_REQUESTRESIZE                    = WM_USER + 65;
  EM_SELECTIONTYPE                    = WM_USER + 66;
  EM_SETBKGNDCOLOR                    = WM_USER + 67;
  EM_SETCHARFORMAT                    = WM_USER + 68;
  EM_SETEVENTMASK                     = WM_USER + 69;
  EM_SETOLECALLBACK                   = WM_USER + 70;
  EM_SETPARAFORMAT                    = WM_USER + 71;
  EM_SETTARGETDEVICE                  = WM_USER + 72;
  EM_STREAMIN                         = WM_USER + 73;
  EM_STREAMOUT                        = WM_USER + 74;
  EM_GETTEXTRANGE                     = WM_USER + 75;
  EM_FINDWORDBREAK                    = WM_USER + 76;
  EM_SETOPTIONS                       = WM_USER + 77;
  EM_GETOPTIONS                       = WM_USER + 78;
  EM_FINDTEXTEX                       = WM_USER + 79;
  EM_GETWORDBREAKPROCEX               = WM_USER + 80;
  EM_SETWORDBREAKPROCEX               = WM_USER + 81;

{ Richedit v2.0 messages }

  EM_SETUNDOLIMIT                     = WM_USER + 82;
  EM_REDO                             = WM_USER + 84;
  EM_CANREDO                          = WM_USER + 85;
  EM_GETUNDONAME                      = WM_USER + 86;
  EM_GETREDONAME                      = WM_USER + 87;
  EM_STOPGROUPTYPING                  = WM_USER + 88;
  EM_SETTEXTMODE                      = WM_USER + 89;
  EM_GETTEXTMODE                      = WM_USER + 90;

{ for use with EM_GET/SETTEXTMODE }

  TM_PLAINTEXT                       = 1;
  TM_RICHTEXT                        = 2;     { default behavior }
  TM_SINGLELEVELUNDO                 = 4;
  TM_MULTILEVELUNDO                  = 8;     { default behavior }
  TM_SINGLECODEPAGE                  = 16;
  TM_MULTICODEPAGE                   = 32;    { default behavior }

  EM_AUTOURLDETECT                    = WM_USER + 91;
  EM_GETAUTOURLDETECT                 = WM_USER + 92;
  EM_SETPALETTE                       = WM_USER + 93;
  EM_GETTEXTEX                        = WM_USER + 94;
  EM_GETTEXTLENGTHEX                  = WM_USER + 95;

{ Far East specific messages }

  EM_SETPUNCTUATION                   = WM_USER + 100;
  EM_GETPUNCTUATION                   = WM_USER + 101;
  EM_SETWORDWRAPMODE                  = WM_USER + 102;
  EM_GETWORDWRAPMODE                  = WM_USER + 103;
  EM_SETIMECOLOR                      = WM_USER + 104;
  EM_GETIMECOLOR                      = WM_USER + 105;
  EM_SETIMEOPTIONS                    = WM_USER + 106;
  EM_GETIMEOPTIONS                    = WM_USER + 107;
  EM_CONVPOSITION                     = WM_USER + 108;

  EM_SETLANGOPTIONS                   = WM_USER + 120;
  EM_GETLANGOPTIONS                   = WM_USER + 121;
  EM_GETIMECOMPMODE                   = WM_USER + 122;

{ Options for EM_SETLANGOPTIONS and EM_GETLANGOPTIONS }

  IMF_AUTOKEYBOARD            = $0001;
  IMF_AUTOFONT                = $0002;
  IMF_IMECANCELCOMPLETE       = $0004;  { high completes the comp string when aborting, low cancels. }
  IMF_IMEALWAYSSENDNOTIFY     = $0008;

{ Values for EM_GETIMECOMPMODE }

  ICM_NOTOPEN                         = $0000;
  ICM_LEVEL3                          = $0001;
  ICM_LEVEL2                          = $0002;
  ICM_LEVEL2_5                        = $0003;
  ICM_LEVEL2_SUI                      = $0004;

{ New notifications }

  EN_MSGFILTER                        = $0700;
  EN_REQUESTRESIZE                    = $0701;
  EN_SELCHANGE                        = $0702;
  EN_DROPFILES                        = $0703;
  EN_PROTECTED                        = $0704;
  EN_CORRECTTEXT                      = $0705;                  { PenWin specific }
  EN_STOPNOUNDO                       = $0706;
  EN_IMECHANGE                        = $0707;                  { Far East specific }
  EN_SAVECLIPBOARD                    = $0708;
  EN_OLEOPFAILED                      = $0709;
  EN_OBJECTPOSITIONS                  = $070a;
  EN_LINK                             = $070b;
  EN_DRAGDROPDONE                     = $070c;

{ Event notification masks }

  ENM_NONE                            = $00000000;
  ENM_CHANGE                          = $00000001;
  ENM_UPDATE                          = $00000002;
  ENM_SCROLL                          = $00000004;
  ENM_KEYEVENTS                       = $00010000;
  ENM_MOUSEEVENTS                     = $00020000;
  ENM_REQUESTRESIZE                   = $00040000;
  ENM_SELCHANGE                       = $00080000;
  ENM_DROPFILES                       = $00100000;
  ENM_PROTECTED                       = $00200000;
  ENM_CORRECTTEXT                     = $00400000;              { PenWin specific }
  ENM_SCROLLEVENTS                    = $00000008;
  ENM_DRAGDROPDONE                    = $00000010;

{ Far East specific notification mask }

  ENM_IMECHANGE                       = $00800000;              { unused by RE2.0 }
  ENM_LANGCHANGE                      = $01000000;
  ENM_OBJECTPOSITIONS                 = $02000000;
  ENM_LINK                            = $04000000;

{ New edit control styles }

  ES_SAVESEL                          = $00008000;
  ES_SUNKEN                           = $00004000;
  ES_DISABLENOSCROLL                  = $00002000;
{ same as WS_MAXIMIZE, but that doesn't make sense so we re-use the value }
  ES_SELECTIONBAR                     = $01000000;
{ same as ES_UPPERCASE, but re-used to completely disable OLE drag'n'drop }
  ES_NOOLEDRAGDROP                    = $00000008;

{ New edit control extended style }

  ES_EX_NOCALLOLEINIT                 = $01000000;

{ These flags are used in FE Windows }

  ES_VERTICAL                         = $00400000;
  ES_NOIME                            = $00080000;
  ES_SELFIME                          = $00040000;

{ Edit control options }

  ECO_AUTOWORDSELECTION       = $00000001;
  ECO_AUTOVSCROLL             = $00000040;
  ECO_AUTOHSCROLL             = $00000080;
  ECO_NOHIDESEL               = $00000100;
  ECO_READONLY                = $00000800;
  ECO_WANTRETURN              = $00001000;
  ECO_SAVESEL                 = $00008000;
  ECO_SELECTIONBAR            = $01000000;
  ECO_VERTICAL                = $00400000;              { FE specific }

{ ECO operations }

  ECOOP_SET                                   = $0001;
  ECOOP_OR                                    = $0002;
  ECOOP_AND                                   = $0003;
  ECOOP_XOR                                   = $0004;

{ new word break function actions }

  WB_CLASSIFY                 = 3;
  WB_MOVEWORDLEFT             = 4;
  WB_MOVEWORDRIGHT            = 5;
  WB_LEFTBREAK                = 6;
  WB_RIGHTBREAK               = 7;

{ Far East specific flags }

  WB_MOVEWORDPREV             = 4;
  WB_MOVEWORDNEXT             = 5;
  WB_PREVBREAK                = 6;
  WB_NEXTBREAK                = 7;

  PC_FOLLOWING                = 1;
  PC_LEADING                  = 2;
  PC_OVERFLOW                 = 3;
  PC_DELIMITER                = 4;
  WBF_WORDWRAP                = $010;
  WBF_WORDBREAK               = $020;
  WBF_OVERFLOW                = $040;
  WBF_LEVEL1                  = $080;
  WBF_LEVEL2                  = $100;
  WBF_CUSTOM                  = $200;

{ Far East specific flags }

  IMF_FORCENONE               = $0001;
  IMF_FORCEENABLE             = $0002;
  IMF_FORCEDISABLE            = $0004;
  IMF_CLOSESTATUSWINDOW       = $0008;
  IMF_VERTICAL                = $0020;
  IMF_FORCEACTIVE             = $0040;
  IMF_FORCEINACTIVE           = $0080;
  IMF_FORCEREMEMBER           = $0100;
  IMF_MULTIPLEEDIT            = $0400;

{ Word break flags (used with WB_CLASSIFY) }

  WBF_CLASS                   = $0F;
  WBF_ISWHITE                 = $10;
  WBF_BREAKLINE               = $20;
  WBF_BREAKAFTER              = $40;

{ all character format measurements are in twips }

type
  TCharFormatA = record
    cbSize: UINT;
    dwMask: Longint;
    dwEffects: Longint;
    yHeight: Longint;
    yOffset: Longint;
    crTextColor: TColorRef;
    bCharSet: Byte;
    bPitchAndFamily: Byte;
    szFaceName: array[0..LF_FACESIZE - 1] of AnsiChar;
  end;
  TCharFormatW = record
    cbSize: UINT;
    dwMask: Longint;
    dwEffects: Longint;
    yHeight: Longint;
    yOffset: Longint;
    crTextColor: TColorRef;
    bCharSet: Byte;
    bPitchAndFamily: Byte;
    szFaceName: array[0..LF_FACESIZE - 1] of WideChar;
  end;
  TCharFormat = TCharFormatA;

{ CHARFORMAT masks }

const
  CFM_BOLD            = $00000001;
  CFM_ITALIC          = $00000002;
  CFM_UNDERLINE       = $00000004;
  CFM_STRIKEOUT       = $00000008;
  CFM_PROTECTED       = $00000010;
  CFM_LINK            = $00000020;              { Exchange hyperlink extension }
  CFM_SIZE            = $80000000;
  CFM_COLOR           = $40000000;
  CFM_FACE            = $20000000;
  CFM_OFFSET          = $10000000;
  CFM_CHARSET         = $08000000;

{ CHARFORMAT effects }

  CFE_BOLD            = $0001;
  CFE_ITALIC          = $0002;
  CFE_UNDERLINE       = $0004;
  CFE_STRIKEOUT       = $0008;
  CFE_PROTECTED       = $0010;
  CFE_LINK            = $0020;
  CFE_AUTOCOLOR       = $40000000;  { NOTE: this corresponds to CFM_COLOR, }
                                    { which controls it }
  yHeightCharPtsMost  = 1638;

{ EM_SETCHARFORMAT wParam masks }

  SCF_SELECTION       = $0001;
  SCF_WORD            = $0002;
  SCF_DEFAULT         = $0000;          { set the default charformat or paraformat }
  SCF_ALL             = $0004;          { not valid with SCF_SELECTION or SCF_WORD }
  SCF_USEUIRULES      = $0008;          { modifier for SCF_SELECTION; says that }
                                        { the format came from a toolbar, etc. and }
                                        { therefore UI formatting rules should be }
                                        { used instead of strictly formatting the }
                                        { selection. }

type
  _charrange = record
    cpMin: Longint;
    cpMax: LongInt;
  end;
  TCharRange = _charrange;
  CHARRANGE = _charrange;

  TEXTRANGEA = record
    chrg: TCharRange;
    lpstrText: PAnsiChar;
  end;
  TTextRangeA = TEXTRANGEA;
  TEXTRANGEW = record
    chrg: TCharRange;
    lpstrText: PWideChar;
  end;
  TTextRangeW = TEXTRANGEW;
  TEXTRANGE = TEXTRANGEA;

type
  TEditStreamCallBack = function (dwCookie: Longint; pbBuff: PByte;
    cb: Longint; var pcb: Longint): Longint; stdcall;

  _editstream = record
    dwCookie: Longint;
    dwError: Longint;
    pfnCallback: TEditStreamCallBack;
  end;
  TEditStream = _editstream;
  EDITSTREAM = _editstream;

{ stream formats }

const
  SF_TEXT             = $0001;
  SF_RTF              = $0002;
  SF_RTFNOOBJS        = $0003;          { outbound only }
  SF_TEXTIZED         = $0004;          { outbound only }
  SF_UNICODE          = $0010;          { Unicode file of some kind }

{ Flag telling stream operations to operate on the selection only }
{ EM_STREAMIN will replace the current selection }
{ EM_STREAMOUT will stream out the current selection }

  SFF_SELECTION       = $8000;

{ Flag telling stream operations to operate on the common RTF keyword only }
{ EM_STREAMIN will accept the only common RTF keyword }
{ EM_STREAMOUT will stream out the only common RTF keyword }

  SFF_PLAINRTF        = $4000;

{ EM_FINDTEXT flags (removed in 3.0 SDK - leave in!) }

  FT_MATCHCASE = 4;
  FT_WHOLEWORD = 2;

type
  FINDTEXTA = record
    chrg: TCharRange;
    lpstrText: PAnsiChar;
  end;
  FINDTEXTW = record
    chrg: TCharRange;
    lpstrText: PWideChar;
  end;
  FINDTEXT = FINDTEXTA;
  TFindTextA = FINDTEXTA;
  TFindTextW = FINDTEXTW;
  TFindText = TFindTextA;

  FINDTEXTEXA = record
    chrg: TCharRange;
    lpstrText: PAnsiChar;
    chrgText: TCharRange;
  end;
  FINDTEXTEXW = record
    chrg: TCharRange;
    lpstrText: PWideChar;
    chrgText: TCharRange;
  end;
  FINDTEXTEX = FINDTEXTEXA;
  TFindTextExA = FINDTEXTEXA;
  TFindTextExW = FINDTEXTEXW;
  TFindTextEx = TFindTextExA;

  _formatrange = record
    _hdc: HDC;
    hdcTarget: HDC;
    rc: TRect;
    rcPage: TRect;
    chrg: TCharRange;
  end;
  TFormatRange = _formatrange;
  FORMATRANGE = _formatrange;

{ all paragraph measurements are in twips }

const
  MAX_TAB_STOPS     = 32;
  lDefaultTab     = 720;

type
  _paraformat = record
    cbSize: UINT;
    dwMask: DWORD;
    wNumbering: Word;
    wReserved: Word;
    dxStartIndent: Longint;
    dxRightIndent: Longint;
    dxOffset: Longint;
    wAlignment: Word;
    cTabCount: Smallint;
    rgxTabs: array [0..MAX_TAB_STOPS - 1] of Longint;
  end;
  TParaFormat = _paraformat;
  PARAFORMAT = _paraformat;

{ PARAFORMAT mask values }

const
  PFM_STARTINDENT                     = $00000001;
  PFM_RIGHTINDENT                     = $00000002;
  PFM_OFFSET                          = $00000004;
  PFM_ALIGNMENT                       = $00000008;
  PFM_TABSTOPS                        = $00000010;
  PFM_NUMBERING                       = $00000020;
  PFM_OFFSETINDENT                    = $80000000;

{ PARAFORMAT numbering options }

  PFN_BULLET                  = $0001;

{ PARAFORMAT alignment options }

  PFA_LEFT            = $0001;
  PFA_RIGHT           = $0002;
  PFA_CENTER          = $0003;

type
  CHARFORMAT2A = record
    cbSize: UINT;
    dwMask: DWORD;
    dwEffects: DWORD;
    yHeight: Longint;
    yOffset: Longint;
    crTextColor: TColorRef;
    bCharSet: Byte;
    bPitchAndFamily: Byte;
    szFaceName: array[0..LF_FACESIZE - 1] of AnsiChar;
    wWeight: Word;                   { Font weight (LOGFONT value)		 }
    sSpacing: Smallint;              { Amount to space between letters	 }
    crBackColor: TColorRef;          { Background color					 }
    lid: LCID;                       { Locale ID						 }
    dwReserved: DWORD;               { Reserved. Must be 0				 }
    sStyle: Smallint;                { Style handle						 }
    wKerning: Word;                  { Twip size above which to kern char pair }
    bUnderlineType: Byte;            { Underline type					 }
    bAnimation: Byte;                { Animated text like marching ants	 }
    bRevAuthor: Byte;                { Revision author index			 }
    bReserved1: Byte;
  end;
  CHARFORMAT2W = record
    cbSize: UINT;
    dwMask: DWORD;
    dwEffects: DWORD;
    yHeight: Longint;
    yOffset: Longint;
    crTextColor: TColorRef;
    bCharSet: Byte;
    bPitchAndFamily: Byte;
    szFaceName: array[0..LF_FACESIZE - 1] of WideChar;
    wWeight: Word;                   { Font weight (LOGFONT value)		 }
    sSpacing: Smallint;              { Amount to space between letters	 }
    crBackColor: TColorRef;          { Background color					 }
    lid: LCID;                       { Locale ID						 }
    dwReserved: DWORD;               { Reserved. Must be 0				 }
    sStyle: Smallint;                { Style handle						 }
    wKerning: Word;                  { Twip size above which to kern char pair }
    bUnderlineType: Byte;            { Underline type					 }
    bAnimation: Byte;                { Animated text like marching ants	 }
    bRevAuthor: Byte;                { Revision author index			 }
    bReserved1: Byte;
  end;
  CHARFORMAT2 = CHARFORMAT2A;
  TCharFormat2A = CHARFORMAT2A;
  TCharFormat2W = CHARFORMAT2W;
  TCharFormat2 = TCharFormat2A;

{ CHARFORMAT and PARAFORMAT "ALL" masks
  CFM_COLOR mirrors CFE_AUTOCOLOR, a little hack to easily deal with autocolor }
const
  CFM_EFFECTS = CFM_BOLD or CFM_ITALIC or CFM_UNDERLINE or CFM_COLOR or
    CFM_STRIKEOUT or CFE_PROTECTED or CFM_LINK;
  CFM_ALL = CFM_EFFECTS or CFM_SIZE or CFM_FACE or CFM_OFFSET or CFM_CHARSET;
  PFM_ALL = PFM_STARTINDENT or PFM_RIGHTINDENT or PFM_OFFSET or
    PFM_ALIGNMENT or PFM_TABSTOPS or PFM_NUMBERING or PFM_OFFSETINDENT;

{ New masks and effects -- a parenthesized asterisk indicates that
   the data is stored by RichEdit2.0, but not displayed }

  CFM_SMALLCAPS               = $0040;                  { (*)	 }
  CFM_ALLCAPS                 = $0080;                  { (*)	 }
  CFM_HIDDEN                  = $0100;                  { (*)	 }
  CFM_OUTLINE                 = $0200;                  { (*)	 }
  CFM_SHADOW                  = $0400;                  { (*)	 }
  CFM_EMBOSS                  = $0800;                  { (*)	 }
  CFM_IMPRINT                 = $1000;                  { (*)	 }
  CFM_DISABLED                = $2000;
  CFM_REVISED                 = $4000;

  CFM_BACKCOLOR               = $04000000;
  CFM_LCID                    = $02000000;
  CFM_UNDERLINETYPE           = $00800000;              { (*)	 }
  CFM_WEIGHT                  = $00400000;
  CFM_SPACING                 = $00200000;              { (*)	 }
  CFM_KERNING                 = $00100000;              { (*)	 }
  CFM_STYLE                   = $00080000;              { (*)	 }
  CFM_ANIMATION               = $00040000;              { (*)	 }
  CFM_REVAUTHOR               = $00008000;

  CFE_SUBSCRIPT               = $00010000;              { Superscript and subscript are }
  CFE_SUPERSCRIPT             = $00020000;              {  mutually exclusive			 }

  CFM_SUBSCRIPT               = CFE_SUBSCRIPT or CFE_SUPERSCRIPT;
  CFM_SUPERSCRIPT             = CFM_SUBSCRIPT;

  CFM_EFFECTS2 = CFM_EFFECTS or CFM_DISABLED or CFM_SMALLCAPS or CFM_ALLCAPS or
    CFM_HIDDEN  or CFM_OUTLINE or CFM_SHADOW or CFM_EMBOSS or
    CFM_IMPRINT or CFM_DISABLED or CFM_REVISED or
    CFM_SUBSCRIPT or CFM_SUPERSCRIPT or CFM_BACKCOLOR;

  CFM_ALL2 = CFM_ALL or CFM_EFFECTS2 or CFM_BACKCOLOR or CFM_LCID or
    CFM_UNDERLINETYPE or CFM_WEIGHT or CFM_REVAUTHOR or
    CFM_SPACING or CFM_KERNING or CFM_STYLE or CFM_ANIMATION;

  CFE_SMALLCAPS               = CFM_SMALLCAPS;
  CFE_ALLCAPS                 = CFM_ALLCAPS;
  CFE_HIDDEN                  = CFM_HIDDEN;
  CFE_OUTLINE                 = CFM_OUTLINE;
  CFE_SHADOW                  = CFM_SHADOW;
  CFE_EMBOSS                  = CFM_EMBOSS;
  CFE_IMPRINT                 = CFM_IMPRINT;
  CFE_DISABLED                = CFM_DISABLED;
  CFE_REVISED                 = CFM_REVISED;

{ NOTE: CFE_AUTOCOLOR and CFE_AUTOBACKCOLOR correspond to CFM_COLOR and
   CFM_BACKCOLOR, respectively, which control them }

  CFE_AUTOBACKCOLOR           = CFM_BACKCOLOR;

{ Underline types }

  CFU_CF1UNDERLINE            = $FF;    { map charformat's bit underline to CF2. }
  CFU_INVERT                  = $FE;    { For IME composition fake a selection. }
  CFU_UNDERLINEDOTTED         = $4;             { (*) displayed as ordinary underline	 }
  CFU_UNDERLINEDOUBLE         = $3;             { (*) displayed as ordinary underline	 }
  CFU_UNDERLINEWORD           = $2;             { (*) displayed as ordinary underline	 }
  CFU_UNDERLINE               = $1;
  CFU_UNDERLINENONE           = 0;

type
  PARAFORMAT2 = record
    cbSize: UINT;
    dwMask: DWORD;
    wNumbering: Word;
    wReserved: Word;
    dxStartIndent: Longint;
    dxRightIndent: Longint;
    dxOffset: Longint;
    wAlignment: Word;
    cTabCount: Smallint;
    rgxTabs: array [0..MAX_TAB_STOPS - 1] of Longint;
    dySpaceBefore: Longint;     { Vertical spacing before para			 }
    dySpaceAfter: Longint;      { Vertical spacing after para			 }
    dyLineSpacing: Longint;     { Line spacing depending on Rule		 }
    sStyle: Smallint;           { Style handle							 }
    bLineSpacingRule: Byte;     { Rule for line spacing (see tom.doc)	 }
    bCRC: Byte;                 { Reserved for CRC for rapid searching	 }
    wShadingWeight: Word;       { Shading in hundredths of a per cent	 }
    wShadingStyle: Word;        { Nibble 0: style, 1: cfpat, 2: cbpat	 }
    wNumberingStart: Word;      { Starting value for numbering			 }
    wNumberingStyle: Word;      { Alignment, roman/arabic, (), ), ., etc. }
    wNumberingTab: Word;        { Space bet 1st indent and 1st-line text }
    wBorderSpace: Word;         { Space between border and text (twips) }
    wBorderWidth: Word;         { Border pen width (twips)				 }
    wBorders: Word;             { Byte 0: bits specify which borders	 }
                                { Nibble 2: border style, 3: color index }
  end;
  TParaFormat2 = PARAFORMAT2;

{ PARAFORMAT 2.0 masks and effects }
const
  PFM_SPACEBEFORE                     = $00000040;
  PFM_SPACEAFTER                      = $00000080;
  PFM_LINESPACING                     = $00000100;
  PFM_STYLE                           = $00000400;
  PFM_BORDER                          = $00000800;      { (*)	 }
  PFM_SHADING                         = $00001000;      { (*)	 }
  PFM_NUMBERINGSTYLE                  = $00002000;      { (*)	 }
  PFM_NUMBERINGTAB                    = $00004000;      { (*)	 }
  PFM_NUMBERINGSTART                  = $00008000;      { (*)	 }

  PFM_RTLPARA                         = $00010000;
  PFM_KEEP                            = $00020000;      { (*)	 }
  PFM_KEEPNEXT                        = $00040000;      { (*)	 }
  PFM_PAGEBREAKBEFORE                 = $00080000;      { (*)	 }
  PFM_NOLINENUMBER                    = $00100000;      { (*)	 }
  PFM_NOWIDOWCONTROL                  = $00200000;      { (*)	 }
  PFM_DONOTHYPHEN                     = $00400000;      { (*)	 }
  PFM_SIDEBYSIDE                      = $00800000;      { (*)	 }

  PFM_TABLE                           = $c0000000;      { (*)	 }

{ Note: PARAFORMAT has no effects }

  PFM_EFFECTS = PFM_RTLPARA or PFM_KEEP or PFM_KEEPNEXT or PFM_TABLE or
    PFM_PAGEBREAKBEFORE or PFM_NOLINENUMBER or
    PFM_NOWIDOWCONTROL or PFM_DONOTHYPHEN or PFM_SIDEBYSIDE or PFM_TABLE;

  PFM_ALL2 = PFM_ALL or PFM_EFFECTS or PFM_SPACEBEFORE or PFM_SPACEAFTER or
    PFM_LINESPACING or PFM_STYLE or PFM_SHADING or PFM_BORDER or
    PFM_NUMBERINGTAB or PFM_NUMBERINGSTART or PFM_NUMBERINGSTYLE;

  PFE_RTLPARA                         = PFM_RTLPARA		 shr 16;
  PFE_KEEP                            = PFM_KEEP			 shr 16;                     { (*)	 }
  PFE_KEEPNEXT                        = PFM_KEEPNEXT		 shr 16;          { (*)	 }
  PFE_PAGEBREAKBEFORE                 = PFM_PAGEBREAKBEFORE shr 16;     { (*)	 }
  PFE_NOLINENUMBER                    = PFM_NOLINENUMBER	 shr 16;       { (*)	 }
  PFE_NOWIDOWCONTROL                  = PFM_NOWIDOWCONTROL	 shr 16;     { (*)	 }
  PFE_DONOTHYPHEN                     = PFM_DONOTHYPHEN 	 shr 16;       { (*)	 }
  PFE_SIDEBYSIDE                      = PFM_SIDEBYSIDE		 shr 16;        { (*)	 }

  PFE_TABLEROW                        = $c000;          { These 3 options are mutually	 }
  PFE_TABLECELLEND                    = $8000;          {  exclusive and each imply	 }
  PFE_TABLECELL                       = $4000;          {  that para is part of a table }

{
 *	PARAFORMAT numbering options (values for wNumbering):
 *
 *		Numbering Type		Value	Meaning
 *		tomNoNumbering		     0		Turn off paragraph numbering
 *		tomNumberAsLCLetter	  1		a, b, c, ...
 *		tomNumberAsUCLetter	  2		A, B, C, ...
 *		tomNumberAsLCRoman	  3		i, ii, iii, ...
 *		tomNumberAsUCRoman	  4		I, II, III, ...
 *		tomNumberAsSymbols	  5		default is bullet
 *		tomNumberAsNumber	     6		0, 1, 2, ...
 *		tomNumberAsSequence	  7		tomNumberingStart is first Unicode to use
 *
 *	Other valid Unicode chars are Unicodes for bullets.
}

  PFA_JUSTIFY                         = 4;      { New paragraph-alignment option 2.0 (*) }

{ notification structures }
type
  PMsgFilter = ^TMsgFilter;
  _msgfilter = record
    nmhdr: TNMHdr;
    msg: UINT;
    wParam: WPARAM;
    lParam: LPARAM;
  end;
  TMsgFilter = _msgfilter;
  MSGFILTER = _msgfilter;

  PReqSize = ^TReqSize;
  TReqSize = record
    nmhdr: TNMHdr;
    rc: TRect;
  end;

  PSelChange = ^TSelChange;
  _selchange = record
    nmhdr: TNMHdr;
    chrg: TCharRange;
    seltyp: Word;
  end;
  TSelChange = _selchange;
  SELCHANGE = _selchange;

const
  SEL_EMPTY           = $0000;
  SEL_TEXT            = $0001;
  SEL_OBJECT          = $0002;
  SEL_MULTICHAR       = $0004;
  SEL_MULTIOBJECT     = $0008;

{ used with IRichEditOleCallback::GetContextMenu, this flag will be
   passed as a "selection type".  It indicates that a context menu for
   a right-mouse drag drop should be generated.  The IOleObject parameter
   will really be the IDataObject for the drop
}
  GCM_RIGHTMOUSEDROP      = $8000;

type
  TEndDropFiles = record
    nmhdr: TNMHdr;
    hDrop: THandle;
    cp: Longint;
    fProtected: Bool;
  end;

  PENProtected = ^TENProtected;
  _enprotected = record
    nmhdr: TNMHdr;
    msg: UINT;
    wParam: WPARAM;
    lParam: LPARAM;
    chrg: TCharRange;
  end;
  TENProtected = _enprotected;
  ENPROTECTED = _enprotected;

  PENSaveClipboard = ^TENSaveClipboard;
  _ensaveclipboard = record
    nmhdr: TNMHdr;
    cObjectCount: Longint;
    cch: Longint;
  end;
  TENSaveClipboard = _ensaveclipboard;
  ENSAVECLIPBOARD = _ensaveclipboard;

  ENOLEOPFAILED = packed record
    nmhdr: TNMHdr;
    iob: Longint;
    lOper: Longint;
    hr: HRESULT;
  end;
  TENOleOpFailed = ENOLEOPFAILED;

const
  OLEOP_DOVERB        = 1;

type
  OBJECTPOSITIONS = packed record
    nmhdr: TNMHdr;
    cObjectCount: Longint;
    pcpPositions: PLongint;
  end;
  TObjectPositions = OBJECTPOSITIONS;

  ENLINK = record
    nmhdr: TNMHdr;
    msg: UINT;
    wParam: WPARAM;
    lParam: LPARAM;
    chrg: TCharRange;
  end;
  TENLink = ENLINK;

{ PenWin specific }
  _encorrecttext = record
    nmhdr: TNMHdr;
    chrg: TCharRange;
    seltyp: Word;
  end;
  TENCorrectText = _encorrecttext;
  ENCORRECTTEXT = _encorrecttext;

{ Far East specific }
  _punctuation = record
    iSize: UINT;
    szPunctuation: PChar;
  end;
  TPunctuation = _punctuation;
  PUNCTUATION = _punctuation;

{ Far East specific }
  _compcolor = record
    crText: TColorRef;
    crBackground: TColorRef;
    dwEffects: Longint;
  end;
  TCompColor = _compcolor;
  COMPCOLOR = _compcolor;

{ clipboard formats - use as parameter to RegisterClipboardFormat }

const
  CF_RTF                 = 'Rich Text Format';
  CF_RTFNOOBJS           = 'Rich Text Format Without Objects';
  CF_RETEXTOBJ           = 'RichEdit Text and Objects';

type
  _repastespecial = record
    dwAspect: DWORD;
    dwParam: DWORD;
  end;
  TRepasteSpecial = _repastespecial;
  REPASTESPECIAL = _repastespecial;

{ 	UndoName info }

  UNDONAMEID = (UID_UNKNOWN, UID_TYPING, UID_DELETE, UID_DRAGDROP, UID_CUT,
    UID_PASTE);

{ flags for the GETEXTEX data structure }

const
  GT_DEFAULT                  = 0;
  GT_USECRLF                  = 1;

{ EM_GETTEXTEX info; this struct is passed in the wparam of the message }

type
  PBOOL = ^BOOL;

  GETTEXTEX = record
    cb: DWORD;                 { count of bytes in the string  }
    flags: DWORD;              { flags (see the GT_XXX defines }
    codepage: UINT;            { code page for translation (CP_ACP for default,
                                 1200 for Unicode 					 }
    lpDefaultChar: LPCSTR;     { replacement for unmappable chars			 }
    lpUsedDefChar: PBOOL;      { pointer to flag set when def char used	 }
  end;
  TGetTextEx = GETTEXTEX;
{ flags for the GETTEXTLENGTHEX data structure }

const
  GTL_DEFAULT         = 0;      { do the default (return # of chars)		 }
  GTL_USECRLF         = 1;      { compute answer using CRLFs for paragraphs }
  GTL_PRECISE         = 2;      { compute a precise answer					 }
  GTL_CLOSE           = 4;      { fast computation of a "close" answer		 }
  GTL_NUMCHARS        = 8;      { return the number of characters			 }
  GTL_NUMBYTES        = 16;     { return the number of _bytes_				 }

{ EM_GETTEXTLENGTHEX info; this struct is passed in the wparam of the msg }

type
  GETTEXTLENGTHEX = record
    flags: DWORD;              { flags (see GTL_XXX defines)				 }
    codepage: UINT;            { code page for translation (CP_ACP for default,
                                 1200 for Unicode 					 }
  end;
  TGetTextLengthEx = GETTEXTLENGTHEX;

{ UNICODE embedding character }
const
  WCH_EMBEDDING     = $FFFC;

implementation

end.
