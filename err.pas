{$DEFINE ASM_VERSION}
//{$DEFINE VARIANT_USED}

{$IFDEF ASM_VERSION}
  {$IFDEF PAS_VERSION}
    {$UNDEF ASM_VERSION}
  {$ENDIF}
{$ENDIF}

{$ifdef FPC}
 {$mode delphi}
{$endif}

{=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

        KKKKK    KKKKK    OOOOOOOOO    LLLLL
        KKKKK    KKKKK  OOOOOOOOOOOOO  LLLLL
        KKKKK    KKKKK  OOOOO   OOOOO  LLLLL
        KKKKK  KKKKK    OOOOO   OOOOO  LLLLL
        KKKKKKKKKK      OOOOO   OOOOO  LLLLL
        KKKKK  KKKKK    OOOOO   OOOOO  LLLLL
        KKKKK    KKKKK  OOOOO   OOOOO  LLLLL
        KKKKK    KKKKK  OOOOOOOOOOOOO  LLLLLLLLLLLLL
        KKKKK    KKKKK    OOOOOOOOO    LLLLLLLLLLLLL

  Key Objects Library (C) 2000 by Kladov Vladimir.

  mailto: bonanzas@xcl.cjb.net
  Home: http://kol.nm.ru
        http://xcl.cjb.net
        http://xcl.nm.ru

 =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-}
{
  This code is grabbed mainly from standard SysUtils.pas unit,
  provided by Borland Delphi. This unit is for handling exceptions,
  and to use it just place a reference to exceptions unit in
  uses clause of any of your unit or dpr-file.
}

{       Copyright (C) 1995,99 Inprise Corporation       }
{       Copyright (C) 2001, Kladov Vladimir             }

unit err;
{* Unit to provide error handling for KOL programs using efficient
   exceptions mechanism. To use it, just place a reference to it into
   uses clause of any unit of the project (or dpr-file).
   |<br><br>
   It is possible to use standard SysUtils instead, but it increases
   size of executable at least by 10K. Using this unit to handle exceptions
   increases executable only by 6,5K.
}

interface

uses Windows,
KOL;

{$I KOLDEF.INC}
{$IFDEF _D6orHigher}
  {$WARN SYMBOL_DEPRECATED OFF}
{$ENDIF}
{$IFDEF _D7orHigher}
  {$WARN UNSAFE_TYPE OFF}
  {$WARN UNSAFE_CODE OFF}
{$ENDIF}

{+} // These resource strings are grabbed from SysConst and changed a bit to make it smaller.

//{$DEFINE USE_RESOURCESTRING}
{$IFDEF _D2orD3}
  {$IFDEF USE_RESOURCESTRING}
    {$UNDEF USE_RESOURCESTRING}
  {$ENDIF}
{$ENDIF}

{$IFDEF _D2orD3}
type
  LongWord = DWORD;
{$ENDIF}
{$IFNDEF USE_RESOURCESTRING}
const
{$ELSE}
resourcestring
{$ENDIF}
  SUnknown = '<unknown>';
  SOutOfMemory = 'Out of memory';
  SInOutError = 'I/O error %d';
  SFileNotFound = 'File not found';
  SInvalidFilename = 'Invalid filename';
  STooManyOpenFiles = 'Too many open files';
  SAccessDenied = 'File access denied';
  SEndOfFile = //'Read beyond end of file';
               'End of file';
  SDiskFull = 'Disk full';
  //SInvalidInput = 'Invalid numeric input'; // {-} Seems for console input only
  SDivByZero = 'Division by zero';
  SRangeError = 'Range check error';
  SIntOverflow = 'Integer overflow';
  SInvalidOp = 'Invalid floating point operation';
  SZeroDivide = 'Floating point division by zero';
  SOverflow = 'Floating point overflow';
  SUnderflow = 'Floating point underflow';
  SInvalidPointer = 'Invalid pointer operation';
  SInvalidCast = 'Invalid class typecast';
  SAccessViolation = 'Access violation at address %p. %s of address %p';
  SStackOverflow = 'Stack overflow';
  SControlC = //'Control-C hit';
              '^C'; // {-} for console applications only
  SPrivilege = 'Privileged instruction';
  SOperationAborted = 'Operation aborted';
  SException = 'Exception %s in module %s at %p.'#10'%s%s';
  SInvalidVarCast = 'Invalid variant type conversion';
  SInvalidVarOp = 'Invalid variant operation';
  SDispatchError = 'Variant method calls not supported';
  SVarArrayCreate = 'Error creating variant array';
  SVarNotArray = 'Variant is not an array';
  SVarArrayBounds = 'Variant array index out of bounds';
  SVar = 'EVariant';
  SReadAccess = 'Read';
  SWriteAccess = 'Write';
  SExternalException = 'External exception %x';
  SAssertionFailed = 'Assertion failed';
  SIntfCastError = 'Interface not supported';
  SSafecallException = 'Exception in safecall method';
  SAssertError = '%s (%s, line %d)';
  SAbstractError = 'Abstract Error';
  SModuleAccessViolation = 'Access violation at address %p in module ''%s''. %s of address %p';
  SWin32Error = 'Win32 Error.  Code: %d.'#10'%s';
  SUnkWin32Error = 'A Win32 API function failed';
  SNL = 'Application is not licensed to use this feature';
{-}

type

  TProcedure = procedure;
  TFileName = type KOLstring;

  Exception = class;
  TDestroyException = procedure( Sender: Exception ) of object;

  TError = ( e_Abort, e_Heap, e_OutOfMem, e_InOut, e_External, e_Int,
             e_DivBy0, e_Range, e_IntOverflow, e_Math, e_Math_InvalidArgument,
             e_InvalidOp, e_ZeroDivide, e_Overflow, e_Underflow, e_InvalidPointer,
             e_InvalidCast, e_Convert, e_AccessViolation, e_Privilege,
             e_StackOverflow, e_CtrlC, e_Variant, e_PropReadOnly,
             e_PropWriteOnly, e_Assertion, e_Abstract, e_IntfCast,
             e_InvalidContainer, e_InvalidInsert, e_Package, e_Win32,
             e_SafeCall, e_License, e_Custom, e_Com, e_Ole, e_Registry );
  {* Main error codes. These are to determine which exception occure. You
     can use e_Custom code for your own exceptions. }

  Exception = class(TObject)
  {* Exception class. In KOL, there is a single exception class is used.
     Instead of inheriting new exception classes from this ancestor, an
     instance of the same Exception class should be used. The difference
     is only in Code property, which contains a kind of exception.  }
  protected
    FCode: TError;
    FErrorCode: DWORD;
    FMessage: KOLString;
    FExceptionRecord: PExceptionRecord;
    FData: Pointer;
    FOriginalMessage:WideString;
    FOnDestroy: TDestroyException;
    procedure SetData(const Value: Pointer);
  public
    constructor Create(ACode: TError; const Msg: KOLString);
    {* Use this constructor to raise exception, which does not require of
       argument formatting. }
    constructor CreateFmt(ACode: TError; const Msg: KOLString; const Args: array of const);
    {* Use this constructor to raise an exception with formatted Message string.
       Take into attention, that Format procedure defined in KOL, uses API wvsprintf
       function, which can understand a restricted set of format specifications. }
    constructor CreateCustom(AError: DWORD; const Msg: KOLString; pData:pointer=nil);
    {* Use this constructor to create e_Custom exception and to assign AError to
       its ErrorCode property. }
    constructor CreateCustomFmt(AError: DWORD; const Msg: KOLString; const Args: array of const);
    {* Use this constructor to create e_Custom exception with formatted message
       string and to assign AError to its ErrorCode property. }
    constructor CreateResFmt(ACode: TError; Ident: Integer; const Args: array of const);
    {* }
    destructor Destroy; override;
    {* destructor }
    property Message: KOLString read FMessage; // write FMessage;
    {* Text string, containing descriptive message about the exception. }
    property Code: TError read FCode;
    {* Main exception code. This property can be used to determine, which exception
       occure. }
    property ErrorCode: DWORD read FErrorCode write FErrorCode;
    {* This code is to detailize error. For Code = e_InOut, ErrorCode contains
       more detail description of input/output error. For e_Custom, You can
       assign it to any value You want. }
    property ExceptionRecord: PExceptionRecord read FExceptionRecord;
    {* This property is only for e_External exception. }
    property Data: Pointer read FData write SetData;
    {* Custom defined pointer. Use it in your custom exceptions. }
    property OnDestroy: TDestroyException read FOnDestroy write FOnDestroy;
    {* This event is to allow to do something when custom Exception is
       released. }
	property OriginalMessage:WideString read FOriginalMessage write FOriginalMessage;
  end;
  {*
    With err unit, it is possible to use all capabilities of Delphi exception
    handling almost in the same way as usual. The difference only in that the
    single exception class should be used. To determine which exception occure,
    use property Code. So, code to handle exception can be written like follow:
    ! try
    ! ...
    ! except on E: Exception do
    !   case E.Code of
    !   e_DivBy0: HandleDivideByZero;
    !   e_Overflow: HandleOverflow;
    !   ...
    !   end;
    ! end;
    To raise an error, create an instance of Exception class object, but
    pass a Code to its constructor:
    ! var E: Exception;
    ! ...
    ! E := Exception.Create( e_Custom, 'My custom exception' );
    ! E.ErrorCode := MY_MAGIC_CODE_FOR_CUSTOM_EXCEPTION;
    ! raise E;
  }

  ExceptClass = class of Exception;

var
ExceptClsProc: Pointer;

procedure AddExitProc(Proc: TProcedure);
function SysErrorMessage(ErrorCode: Integer): KOLString;
function ExceptObject: TObject;
function ExceptAddr: Pointer;
function ExceptionErrorMessage(ExceptObject: TObject; ExceptAddr: Pointer;
  Buffer: PKOLChar; Size: Integer): Integer;
procedure ShowException(ExceptObject: TObject; ExceptAddr: Pointer);
procedure Abort;
procedure RaiseLastWin32Error;
function Win32Check(RetVal: BOOL): BOOL;

type
  TTerminateProc = function: Boolean;


procedure AddTerminateProc(TermProc: TTerminateProc);
function CallTerminateProcs: Boolean;
function GDAL: LongWord;
procedure RCS;
procedure RPR;

{$IFNDEF _D2orD3}
function SafeLoadLibrary(const Filename: KOLString;
  ErrorMode: UINT = SEM_NOOPENFILEERRORBOX): HMODULE;
{$ENDIF}

implementation

function AllocMem(Size: Cardinal): Pointer;
begin
  GetMem(Result, Size);
  FillChar(Result^, Size, 0);
end;


type
  PExitProcInfo = ^TExitProcInfo;
  TExitProcInfo = record
    Next: PExitProcInfo;
    SaveExit: Pointer;
    Proc: TProcedure;
  end;

var
  ExitProcList: PExitProcInfo = nil;

procedure DoExitProc;
var
  P: PExitProcInfo;
  Proc: TProcedure;
begin
  P := ExitProcList;
  ExitProcList := P^.Next;
  ExitProc := P^.SaveExit;
  Proc := P^.Proc;
  Dispose(P);
  Proc;
end;

procedure AddExitProc(Proc: TProcedure);
var
  P: PExitProcInfo;
begin
  New(P);
  P^.Next := ExitProcList;
  P^.SaveExit := ExitProc;
  P^.Proc := Proc;
  ExitProcList := P;
  ExitProc := @DoExitProc;
end;

function SysErrorMessage(ErrorCode: Integer): KOLString;
var
  Len: Integer;
  Buffer: array[0..255] of KOLChar;
begin
  Len := FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM or
    FORMAT_MESSAGE_ARGUMENT_ARRAY, nil, ErrorCode, 0, Buffer,
    SizeOf(Buffer), nil);
  while (Len > 0) and ((Buffer[Len - 1] <= ' ') or
                       (Buffer[Len - 1] = '.')) do Dec(Len);
  SetLength(Result, Len);
  move( Buffer, Result[1], Len * Sizeof( KOLChar ) );
end;


type
  PRaiseFrame = ^TRaiseFrame;
  TRaiseFrame = record
    NextRaise: PRaiseFrame;
    ExceptAddr: Pointer;
    ExceptObject: TObject;
    ExceptionRecord: PExceptionRecord;
  end;

function ExceptObject: TObject;
begin
  if RaiseList <> nil then
    Result := PRaiseFrame(RaiseList)^.ExceptObject else
    Result := nil;
end;

function ExceptAddr: Pointer;
begin
  if RaiseList <> nil then
    Result := PRaiseFrame(RaiseList)^.ExceptAddr else
    Result := nil;
end;

function ConvertAddr(Address: Pointer): Pointer; assembler;
asm
        TEST    EAX,EAX         { Always convert nil to nil }
        JE      @@1
        SUB     EAX, $1000      { offset from code start; code start set by linker to $1000 }
@@1:
end;


type
	PStrData = ^TStrData;
	TStrData = record
		Ident: Integer;
{$IFDEF VER230}
		Str: string;
{$ELSE}
    Buffer: PKOLChar;
    BufSize: Integer;
    nChars: Integer;
{$ENDIF}
	end;

{$IFDEF VER230}
function EnumStringModules(Instance: HINST; Data: Pointer): Boolean;
{$IFDEF MSWINDOWS}
var
  Buffer: array [0..1023] of char;
begin
  with PStrData(Data)^ do
  begin
    SetString(Str, Buffer, LoadString(Instance, Ident, Buffer, Length(Buffer)));
    Result := Str = '';
  end;
end;
{$ENDIF MSWINDOWS}
{$IFDEF POSIX}
var
  rs: TResStringRec;
  Module: HModule;
begin
  Module := Instance;
  rs.Module := @Module;
  with PStrData(Data)^ do
  begin
    rs.Identifier := Ident;
    Str := LoadResString(@rs);
    Result := Str = '';
  end;
end;
{$ENDIF POSIX}
{$ELSE}
function EnumStringModules(Instance: Longint; Data: Pointer): Boolean;
begin
  with PStrData(Data)^ do
  begin
    nChars := LoadString(Instance, Ident, Buffer, BufSize);
    Result := nChars = 0;
  end;
end;
{$ENDIF}

{$IFNDEF FPC}
function FindStringResource(Ident: Integer; Buffer: PKOLChar; BufSize: Integer): Integer;
var
  StrData: TStrData;
begin
  StrData.Ident := Ident;
  StrData.Buffer := Buffer;
  StrData.BufSize := BufSize;
  StrData.nChars := 0;
  EnumResourceModules(EnumStringModules, @StrData);
  Result := StrData.nChars;
end;
{$ENDIF}

{$IFDEF FPC}
function LoadStr(Ident: Integer): KOLstring;
var
  Buffer: Array[0..1023] of KOLChar;
begin
  SetString(Result, Buffer, LoadString(HInstance, Ident, Buffer,SizeOf(Buffer)));
end;
{$ELSE}
function LoadStr(Ident: Integer): KOLstring;
var
  Buffer: Array[0..1023] of KOLChar; // KOLChar need KOLString Result
begin
  SetString(Result, Buffer, FindStringResource(Ident, Buffer, SizeOf(Buffer)));
end;
{$ENDIF}

function FmtLoadStr(Ident: Integer; const Args: array of const): KOLString;
begin
  //FmtStr(Result, LoadStr(Ident), Args);
  Result := Format(KOLString(LoadStr(Ident)), Args);
end;

function ExceptionErrorMessage(ExceptObject: TObject; ExceptAddr: Pointer;
  Buffer: PKOLChar; Size: Integer): Integer;
var
  MsgPtr: PKOLChar;
  //MsgEnd: PChar;
  //MsgLen: Integer;
  ModuleName: array[0..MAX_PATH] of KOLChar;
  //Temp: array[0..MAX_PATH] of Char;
  Fmt: Array[0..255] of AnsiChar;
  Info: TMemoryBasicInformation;
  ConvertedAddress: Pointer;
begin
  VirtualQuery(ExceptAddr, Info, sizeof(Info));
  if (Info.State <> MEM_COMMIT) or
    (GetModuleFilename( THandle(Info.AllocationBase), {Temp} ModuleName,
                        SizeOf({Temp} ModuleName)) = 0) then
  begin
    GetModuleFileName(HInstance, {Temp} ModuleName, SizeOf({Temp} ModuleName));
    ConvertedAddress := ConvertAddr(ExceptAddr);
  end
  else
    Integer(ConvertedAddress) := Integer(ExceptAddr) - Integer(Info.AllocationBase);
  //StrLCopy(ModuleName, AnsiStrRScan(Temp, '\') + 1, SizeOf(ModuleName) - 1);
  {-} // Why to extract unit name from a path? Isn't it well to show complete path
      // and to economy code for the extraction.
  MsgPtr := '';
  //MsgEnd := '';
  if ExceptObject is Exception then
  begin
    MsgPtr := PKOLChar(Exception(ExceptObject).Message);
    //MsgLen := StrLen(MsgPtr);
    //if (MsgLen <> 0) and (MsgPtr[MsgLen - 1] <> '.') then MsgEnd := '.';
    {-} // Isn't it too beautiful - devote ~40 bytes of code just to decide,
        // add or not a point at the end of the message.
  end;
  {$IFNDEF USE_RESOURCESTRING}
  StrCopy( Fmt, SException );
  {$ELSE}
  LoadString(FindResourceHInstance(HInstance),
    PResStringRec(@SException).Identifier, Fmt, SizeOf(Fmt));
  {$ENDIF}
  //MsgOK( ModuleName );
  {$IFDEF UNICODE_CTRLS} WStrCopy {$ELSE} StrCopy {$ENDIF}
    ( Buffer, PKOLChar( Format( AnsiString(Fmt), [ ExceptObject.ClassName,
      ModuleName, ConvertedAddress, MsgPtr, '' {MsgEnd}]) ) );
  Result := {$IFDEF UNICODE_CTRLS} WStrLen {$ELSE} StrLen {$ENDIF}(Buffer);
end;

procedure ShowException(ExceptObject: TObject; ExceptAddr: Pointer);
var
  Buffer: array[0..1023] of KOLChar;
begin
  ExceptionErrorMessage(ExceptObject, ExceptAddr, Buffer, SizeOf(Buffer));
  {if IsConsole then
    WriteLn(Buffer)
  else}
  begin
    {LoadString(FindResourceHInstance(HInstance), PResStringRec(@SExceptTitle).Identifier,
      Title, SizeOf(Title));}
    MessageBox(0, Buffer, {Title} nil, MB_OK {or MB_ICONSTOP} or MB_SYSTEMMODAL);
  end;
end;

procedure Abort;

  function ReturnAddr: Pointer;
  asm
//          MOV     EAX,[ESP + 4] !!! codegen dependant
          MOV     EAX,[EBP - 4]
  end;

begin
  raise Exception.Create(e_Abort, SOperationAborted) at ReturnAddr;
end;

constructor Exception.CreateResFmt(ACode: TError; Ident: Integer;
  const Args: array of const);
begin
  FMessage := Format(LoadStr(Ident), Args);
end;

destructor Exception.Destroy;
begin
  if Assigned( FOnDestroy ) then
    FOnDestroy( Self );
  inherited;
end;

procedure Exception.SetData(const Value: Pointer);
begin
  FData := Value;
end;

constructor Exception.Create(ACode: TError; const Msg: KOLString);
begin
  FCode := ACode;
  FMessage := Msg;
  //FAllowFree := TRUE;
end;

constructor Exception.CreateCustom(AError: DWORD; const Msg: KOLString;pData:pointer=nil);
begin
  FCode := e_Custom;
  FMessage := Msg;
  FErrorCode := AError;
  FData:=pData;
end;

constructor Exception.CreateCustomFmt(AError: DWORD; const Msg: KOLString;
  const Args: array of const);
begin
  FCode := e_Custom;
  FErrorCode := AError;
  FMessage := Format(Msg, Args);
end;

constructor Exception.CreateFmt(ACode: TError; const Msg: KOLString;
  const Args: array of const);
begin
  FCode := ACode;
  FMessage := Format(Msg, Args);
end;

function CreateInOutError: Exception;
type
  TErrorRec = record
    Code: Integer;
    Ident: AnsiString; // TODO: resource string store as unicode in stringtable
  end;
const
  ErrorMap: array[0..5] of TErrorRec = (
    (Code: 2; Ident: SFileNotFound),
    (Code: 3; Ident: SInvalidFilename),
    (Code: 4; Ident: STooManyOpenFiles),
    (Code: 5; Ident: SAccessDenied),
    (Code: 100; Ident: SEndOfFile),
    (Code: 101; Ident: SDiskFull){,
    (Code: 106; Ident: SInvalidInput)} );
var
  I: Integer;
  InOutRes: Integer;
begin
  I := Low(ErrorMap);
  InOutRes := IOResult;  // resets IOResult to zero
  while (I <= High(ErrorMap)) and (ErrorMap[I].Code <> InOutRes) do Inc(I);
  if I <= High(ErrorMap) then
    Result := Exception.Create(e_InOut, ErrorMap[I].Ident)
  else
    Result := Exception.CreateFmt(e_InOut, SInOutError, [InOutRes]);
    //Result := Exception.Create(e_InOut, SInOutError + Int2Str( InOutRes ) );
  Result.ErrorCode := InOutRes;
end;


type
  TExceptMapRec = packed record
    ECode: TError;
    EIdent: AnsiString;
  end;

const
  ExceptMap: Array[1..24] of TExceptMapRec = (
    (ECode: e_OutOfMem;       EIdent: SOutOfMemory),
    (ECode: e_InvalidPointer; EIdent: SInvalidPointer),
    (ECode: e_DivBy0;         EIdent: SDivByZero),
    (ECode: e_Range;          EIdent: SRangeError),
    (ECode: e_IntOverflow;    EIdent: SIntOverflow),
    (ECode: e_InvalidOp;      EIdent: SInvalidOp),
    (ECode: e_ZeroDivide;     EIdent: SDivByZero),
    (ECode: e_Overflow;       EIdent: SOverflow),
    (ECode: e_Underflow;      EIdent: SUnderflow),
    (ECode: e_InvalidCast;    EIdent: SInvalidCast),
    (ECode: e_AccessViolation;EIdent: SAccessViolation),
    (ECode: e_Privilege;      EIdent: SPrivilege),
    (ECode: e_CtrlC;          EIdent: SControlC),
             // {-} Only for console applications
    (ECode: e_StackOverflow;  EIdent: SStackOverflow),
    {$IFDEF VARIANT_USED}
    (ECode: e_Variant;        EIdent: SInvalidVarCast),
    (ECode: e_Variant;        EIdent: SInvalidVarOp),
    (ECode: e_Variant;        EIdent: SDispatchError),
    (ECode: e_Variant;        EIdent: SVarArrayCreate),
    (ECode: e_Variant;        EIdent: SVarNotArray),
    (ECode: e_Variant;        EIdent: SVarArrayBounds),
    {$ELSE}
    (ECode: e_Variant;        EIdent: SVar),
    (ECode: e_Variant;        EIdent: SVar),
    (ECode: e_Variant;        EIdent: SVar),
    (ECode: e_Variant;        EIdent: SVar),
    (ECode: e_Variant;        EIdent: SVar),
    (ECode: e_Variant;        EIdent: SVar),
    {$ENDIF}
    (ECode: e_Assertion;      EIdent: SAssertionFailed),
    (ECode: e_External;       EIdent: SExternalException),
    (ECode: e_IntfCast;       EIdent: SIntfCastError),
    (ECode: e_SafeCall;       EIdent: SSafecallException));

procedure ErrorHandler(ErrorCode: Integer; ErrorAddr: Pointer);
var
  E: Exception;
begin
  {+}
  if ErrorCode <= 24 then
    with ExceptMap[ErrorCode] do E := Exception.Create(ECode, EIdent)
  else E := CreateInOutError;
  {-}

  raise E at ErrorAddr;
end;

function CreateAssertException(const Message, Filename: string;
  LineNumber: Integer): Exception;
var
  S: string;
begin
  if Message <> '' then S := Message else S := SAssertionFailed;
  Result := Exception.CreateFmt(e_Assertion, SAssertError,
         [S, Filename, LineNumber]);
end;

{$IFDEF FPC}
procedure RaiseExcept;near;
asm
        POP     EDX
        PUSH    ESP
        PUSH    EBP
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        PUSH    EAX
        PUSH    EDX
        PUSH    ESP
        PUSH    $7
        PUSH    $1
        PUSH    $0EEDFADE
        PUSH    EDX
        JMP     RaiseException
end;

procedure RaiseAssertException(const E: Exception; const ErrorAddr, ErrorStack: Pointer);
asm
        MOV     ESP,ECX
        MOV     [ESP],EDX
        MOV     EBP,[EBP]
        JMP     RaiseExcept
end;
{$ELSE}
procedure RaiseAssertException(const E: Exception; const ErrorAddr, ErrorStack: Pointer);
asm
        MOV     ESP,ECX
        MOV     [ESP],EDX
        MOV     EBP,[EBP]
        JMP     System.@RaiseExcept
end;
{$ENDIF}

procedure AssertErrorHandler(const Message, Filename: string;
  LineNumber: Integer; ErrorAddr: Pointer);
var
  E: Exception;
begin
   E := CreateAssertException(Message, Filename, LineNumber);
   RaiseAssertException(E, ErrorAddr, PChar(@ErrorAddr)+4);
end;

procedure AbstractErrorHandler;
begin
  raise Exception.Create(e_Abstract, SAbstractError);
end;

{$IFDEF ASM_VERSION}
function MapException(P: PExceptionRecord): Byte;
asm     //cmd    //opd
        MOV      EAX, [EAX].TExceptionRecord.ExceptionCode
        SUB      EAX, $C0000000
        CMP      EAX, $FD
        JA       @@code22

        XOR      ECX, ECX
        MOV      EDX, offset @@cvTable - 1
@@loo:
        INC      EDX
        MOV      CL, [EDX]
        JECXZ    @@code22
        INC      EDX
        CMP      AL, [EDX]
        JNE      @@loo

        MOV      AL, CL
        RET

@@cvTable:
        DB       3, $94
        DB       4, $8C
        DB       5, $95
        DB       6, $8F, 6, $90, 6, $92
        DB       7, $8E
        DB       8, $91
        DB       9, $8D, 9, $93
        DB       11, $05
        DB       12, $96
        DB       14, $FD
        DB       0

@@code22:
        MOV      AL, 22
end;
{$ELSE} //Pascal
function MapException(P: PExceptionRecord): Byte;
begin
  case P.ExceptionCode of
    STATUS_INTEGER_DIVIDE_BY_ZERO:
      Result := 3;
    STATUS_ARRAY_BOUNDS_EXCEEDED:
      Result := 4;
    STATUS_INTEGER_OVERFLOW:
      Result := 5;
    STATUS_FLOAT_INEXACT_RESULT,
    STATUS_FLOAT_INVALID_OPERATION,
    STATUS_FLOAT_STACK_CHECK:
      Result := 6;
    STATUS_FLOAT_DIVIDE_BY_ZERO:
      Result := 7;
    STATUS_FLOAT_OVERFLOW:
      Result := 8;
    STATUS_FLOAT_UNDERFLOW,
    STATUS_FLOAT_DENORMAL_OPERAND:
      Result := 9;
    STATUS_ACCESS_VIOLATION:
      Result := 11;
    STATUS_PRIVILEGED_INSTRUCTION:
      Result := 12;
    STATUS_CONTROL_C_EXIT:
      Result := 13;
    STATUS_STACK_OVERFLOW:
      Result := 14;
  else
    Result := 22; { must match System.reExternalException }
  end;
end;
{$ENDIF}

function GetExceptionClass(P: PExceptionRecord): ExceptClass;
//var ErrorCode: Byte;
begin
  //ErrorCode := MapException(P);
  Result := Exception; {ExceptMap[ErrorCode].EClass;}
end;

function GetExceptionObject(P: PExceptionRecord): Exception;
var
  ErrorCode: Integer;

  function CreateAVObject: Exception;
  var
    AccessOp: string; // string ID indicating the access type READ or WRITE
    AccessAddress: Pointer;
    MemInfo: TMemoryBasicInformation;
    ModName: array[0..MAX_PATH] of KOLChar;
  begin
    with P^ do
    begin
      if ExceptionInformation[0] = 0 then
        AccessOp := SReadAccess else
        AccessOp := SWriteAccess;
      AccessAddress := Pointer(ExceptionInformation[1]);
      VirtualQuery(ExceptionAddress, MemInfo, SizeOf(MemInfo));
      if (MemInfo.State = MEM_COMMIT) and (GetModuleFileName(THandle(MemInfo.AllocationBase),
        ModName, SizeOf(ModName)) <> 0) then
        Result := Exception.CreateFmt(e_AccessViolation, sModuleAccessViolation,
          [ExceptionAddress, ExtractFileName(ModName), AccessOp,
          AccessAddress])
      else Result := Exception.CreateFmt(e_AccessViolation, sAccessViolation,
          [ExceptionAddress, AccessOp, AccessAddress]);
    end;
  end;

begin
  ErrorCode := MapException(P);
  case ErrorCode of
    3..10, 12..21:
      with ExceptMap[ErrorCode] do Result := Exception.Create(ECode, EIdent);
    11: Result := CreateAVObject;
  else
    begin
      Result := Exception.CreateFmt(e_External, SExternalException, [P.ExceptionCode]);
      //Result.FExceptionRecord := P;
    end;
  end;
  Result.FExceptionRecord := P;
end;

procedure ExceptHandler(ExceptObject: TObject; ExceptAddr: Pointer); far;
begin
  ShowException(ExceptObject, ExceptAddr);
  Halt(1);
end;

{+}
function InitAssertErrorProc: Boolean;
begin
  AssertErrorProc := @AssertErrorHandler;
  Result := TRUE;
end;
{-}

procedure InitExceptions;
begin
  ErrorProc := @ErrorHandler;
  ExceptProc := @ExceptHandler;
  ExceptionClass := Exception;
  ExceptClsProc := @GetExceptionClass;
  ExceptObjProc := @GetExceptionObject;

  {AssertErrorProc := @AssertErrorHandler;}
  {+} // Initialize Assert only when "Assertions" option is turned on in Compiler:
  Assert( InitAssertErrorProc, '' );
  {-}
  //AbstractErrorProc := @AbstractErrorHandler;
  // {-} KOL does not use classes, so EAbstractError should never be raised.

end;

procedure DoneExceptions;
begin
  ErrorProc := nil;
  ExceptProc := nil;
  ExceptionClass := nil;
  ExceptObjProc := nil;
  AssertErrorProc := nil;
end;

procedure RaiseLastWin32Error;
var
  LastError: DWORD;
  Error: Exception;
begin
  LastError := GetLastError;
  if LastError <> ERROR_SUCCESS then
    Error := Exception.CreateFmt(e_Win32, SWin32Error, [LastError,
      SysErrorMessage(LastError)])
  else
    Error := Exception.Create(e_Win32, SUnkWin32Error );
  Error.ErrorCode := LastError;
  raise Error;
end;

function Win32Check(RetVal: BOOL): BOOL;
begin
  if not RetVal then RaiseLastWin32Error;
  Result := RetVal;
end;

type
  PTerminateProcInfo = ^TTerminateProcInfo;
  TTerminateProcInfo = record
    Next: PTerminateProcInfo;
    Proc: TTerminateProc;
  end;

var
  TerminateProcList: PTerminateProcInfo = nil;

procedure AddTerminateProc(TermProc: TTerminateProc);
var
  P: PTerminateProcInfo;
begin
  New(P);
  P^.Next := TerminateProcList;
  P^.Proc := TermProc;
  TerminateProcList := P;
end;

function CallTerminateProcs: Boolean;
var
  PI: PTerminateProcInfo;
begin
  Result := True;
  PI := TerminateProcList;
  while Result and (PI <> nil) do
  begin
    Result := PI^.Proc;
    PI := PI^.Next;
  end;
end;

procedure FreeTerminateProcs;
var
  PI: PTerminateProcInfo;
begin
  while TerminateProcList <> nil do
  begin
    PI := TerminateProcList;
    TerminateProcList := PI^.Next;
    Dispose(PI);
  end;
end;

function AL1(const P): LongWord;
asm
        MOV     EDX,DWORD PTR [P]
        XOR     EDX,DWORD PTR [P+4]
        XOR     EDX,DWORD PTR [P+8]
        XOR     EDX,DWORD PTR [P+12]
        MOV     EAX,EDX
end;

function AL2(const P): LongWord;
asm
        MOV     EDX,DWORD PTR [P]
        ROR     EDX,5
        XOR     EDX,DWORD PTR [P+4]
        ROR     EDX,5
        XOR     EDX,DWORD PTR [P+8]
        ROR     EDX,5
        XOR     EDX,DWORD PTR [P+12]
        MOV     EAX,EDX
end;

const
  AL1s: array[0..2] of LongWord = ($FFFFFFF0, $FFFFEBF0, 0);
  AL2s: array[0..2] of LongWord = ($42C3ECEF, $20F7AEB6, $D1C2F74E);

procedure ALV;
begin
  raise Exception.Create(e_License, SNL);
end;

function ALR: Pointer;
var
  LibModule: PLibModule;
begin
  if MainInstance <> 0 then
    Result := Pointer(LoadResource(MainInstance, FindResource(MainInstance, 'DVCLAL',
      PKOLChar( RT_RCDATA ))))
  else
  begin
    Result := nil;
    LibModule := LibModuleList;
    while LibModule <> nil do
    begin
      with LibModule^ do
      begin
        Result := Pointer(LoadResource(Instance, FindResource(Instance, 'DVCLAL',
          PKOLChar( RT_RCDATA ))));
        if Result <> nil then Break;
      end;
      LibModule := LibModule.Next;
    end;
  end;
  if Result = nil then ALV;
end;

function GDAL: LongWord;
type
  TDVCLAL = array[0..3] of LongWord;
  PDVCLAL = ^TDVCLAL;
var
  P: Pointer;
  A1, A2: LongWord;
  PAL1s, PAL2s: PDVCLAL;
  ALOK: Boolean;
begin
  P := ALR;
  A1 := AL1(P^);
  A2 := AL2(P^);
  Result := A1;
  PAL1s := @AL1s;
  PAL2s := @AL2s;
  ALOK := ((A1 = PAL1s[0]) and (A2 = PAL2s[0])) or
          ((A1 = PAL1s[1]) and (A2 = PAL2s[1])) or
          ((A1 = PAL1s[2]) and (A2 = PAL2s[2]));
  FreeResource(Integer(P));
  if not ALOK then ALV;
end;

procedure RCS;
var
  P: Pointer;
  ALOK: Boolean;
begin
  P := ALR;
  ALOK := (AL1(P^) = AL1s[2]) and (AL2(P^) = AL2s[2]);
  FreeResource(Integer(P));
  if not ALOK then ALV;
end;

procedure RPR;
var
  AL: LongWord;
begin
  AL := GDAL;
  if (AL <> AL1s[1]) and (AL <> AL1s[2]) then ALV;
end;

{$IFNDEF _D2orD3}
function SafeLoadLibrary(const Filename: KOLString; ErrorMode: UINT): HMODULE;
var
  OldMode: UINT;
  FPUControlWord: Word;
begin
  OldMode := SetErrorMode(ErrorMode);
  try
    asm
      FNSTCW  FPUControlWord
    end;
    try
      Result := LoadLibrary(PKOLChar(Filename));
    finally
      asm
        FNCLEX
        FLDCW FPUControlWord
      end;
    end;
  finally
    SetErrorMode(OldMode);
  end;
end;
{$ENDIF}

initialization
  InitExceptions;

finalization
  FreeTerminateProcs;
  DoneExceptions;

end.

