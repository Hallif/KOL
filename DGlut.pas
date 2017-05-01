{ DGLUT.PAS }

{ Bob Crawford
  F.L.A.S.K.
  June, 1997 }

{ DGLUT is an Object Pascal translation of a small part of Mark Kilgard's
  GLUT library for OpenGL. Included here are just the shape-drawing routines
  from glut_shapes.c and the teapot routines from glut_teapot.c. Following
  is the original copyright notices from those files. }

{ Copyright (c) Mark J. Kilgard, 1994.

/**
(c) Copyright 1993, Silicon Graphics, Inc.

ALL RIGHTS RESERVED

Permission to use, copy, modify, and distribute this software
for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the name of Silicon
Graphics, Inc. not be used in advertising or publicity
pertaining to distribution of the software asaswithout specific,
written prior permission.

THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
"AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
PERFORMANCE OF THIS SOFTWARE.

US Government Users Restricted Rights

Use, duplication, or disclosure by the Government is subject to
restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
(c)(1)(ii) of the Rights in Technical Data and Computer
Software clause at DFARS 252.227-7013 and/or in similar or
successor clauses in the FAR or the DOD or NASA FAR
Supplement.  Unpublished-- rights reserved under the copyright
laws of the United States.  Contractor/manufacturer is Silicon
Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
94039-7311.

OpenGL(TM) is a trademark of Silicon Graphics, Inc.
}

unit DGlut;
{$ifdef fpc}
	{$mode fpc}
{$endif}
interface

{$ifdef fpc}
uses GL
	,GLU
{$else}
uses OpenGL
{$endif}
	;

type
  TGLfloat3v =
    array[0..2] of GLfloat;
  TInteger3v =
    array[0..2] of Integer;

const
  BoxPoints :
    Array[0..5, 0..2] of GLfloat =
      ( (-1,  0,  0),
        ( 0,  1,  0),
        ( 1,  0,  0),
        ( 0, -1,  0),
        ( 0,  0,  1),
        ( 0,  0, -1) );

  BoxFaces :
    Array[0..5, 0..3] of GLint =
      ( (0, 1, 2, 3),
        (3, 2, 6, 7),
        (7, 6, 5, 4),
        (4, 5, 1, 0),
        (5, 6, 2, 1),
        (7, 4, 0, 3) );

  { Octahedron data:
      The octahedron produced is centered at the origin and has radius 1.0 }
  OctData :
    Array[0..5] of TGLfloat3v =
      ( (1.0, 0.0, 0.0),
        (-1.0, 0.0, 0.0),
        (0.0, 1.0, 0.0),
        (0.0, -1.0, 0.0),
        (0.0, 0.0, 1.0),
        (0.0, 0.0, -1.0) );

  OctIndex :
    Array[0..7] of TInteger3v =
      (  (0, 4, 2),
         (1, 2, 4),
         (0, 3, 4),
         (1, 4, 3),
         (0, 2, 5),
         (1, 5, 2),
         (0, 5, 3),
         (1, 3, 5) );

  { Icosahedron data:
      These numbers are rigged to make an icosahedron of radius 1.0 }

  IcoX = 0.525731112119133606;
  IcoZ = 0.850650808352039932;

  IcoData :
    Array[0..11] of TGLfloat3v =
      ( (-IcoX,     0,  IcoZ),
        ( IcoX,     0,  IcoZ),
        (-IcoX,     0, -IcoZ),
        ( IcoX,     0, -IcoZ),
        (    0,  IcoZ,  IcoX),
        (    0,  IcoZ, -IcoX),
        (    0, -IcoZ,  IcoX),
        (    0, -IcoZ, -IcoX),
        ( IcoZ,  IcoX,     0),
        (-IcoZ,  IcoX,     0),
        ( IcoZ, -IcoX,     0),
        (-IcoZ, -IcoX,     0) );

  IcoIndex :
    Array[0..19] of TInteger3v =
      ( (0, 4, 1),
        (0, 9, 4),
        (9, 5, 4),
        (4, 5, 8),
        (4, 8, 1),
        (8, 10, 1),
        (8, 3, 10),
        (5, 3, 8),
        (5, 2, 3),
        (2, 7, 3),
        (7, 10, 3),
        (7, 6, 10),
        (7, 11, 6),
        (11, 0, 6),
        (0, 1, 6),
        (6, 1, 10),
        (9, 0, 11),
        (9, 11, 2),
        (9, 2, 5),
        (7, 2, 11) );

  { Tetrahedron data }

  TetT = 1.73205080756887729;

  TetData :
    Array[0..3] of TGLfloat3v =
      ( ( TetT,  TetT,  TetT),
        ( TetT, -TetT, -TetT),
        (-TetT,  TetT, -TetT),
        (-TetT, -TetT,  TetT) );

  TetIndex :
    Array[0..3] of TInteger3v =
      (  (0, 1, 3),
         (2, 1, 0),
         (3, 2, 0),
         (1, 2, 3)  );

{ Teapot stuff }

  { Rim, body, lid, and bottom data must be reflected in x
   and y; handle and spout data across the y axis only. }

  PatchData :
    Array[0..9, 0..15] of GLint =
    (
      { Rim }
      (102, 103, 104, 105,   4,   5,   6,   7,
         8,   9,  10,  11,  12,  13,  14,  15),
      { Body }
      ( 12,  13,  14,  15,  16,  17,  18,  19,
        20,  21,  22,  23,  24,  25,  26,  27),
      ( 24,  25,  26,  27,  29,  30,  31,  32,
        33,  34,  35,  36,  37,  38,  39,  40),
      { Lid *}
      ( 96,  96,  96,  96,  97,  98,  99, 100,
       101, 101, 101, 101,   0,   1,   2,   3),
      (  0,   1,   2,   3, 106, 107, 108, 109,
       110, 111, 112, 113, 114, 115, 116, 117),
      { Bottom }
      (118, 118, 118, 118, 124, 122, 119, 121,
       123, 126, 125, 120,  40,  39,  38,  37),
      { Handle }
      ( 41,  42,  43,  44,  45,  46,  47,  48,
        49,  50,  51,  52,  53,  54,  55,  56),
      ( 53,  54,  55,  56,  57,  58,  59,  60,
        61,  62,  63,  64,  28,  65,  66,  67),
      { Spout }
      ( 68,  69,  70,  71,  72,  73,  74,  75,
        76,  77,  78,  79,  80,  81,  82,  83),
      ( 80,  81,  82,  83,  84,  85,  86,  87,
        88,  89,  90,  91,  92,  93,  94,  95) );

  TeaData :
    Array[0..126, 0..2] of GLfloat =
    ( (   0.2,       0,     2.7),
      (   0.2,  -0.112,     2.7),
      ( 0.112,    -0.2,     2.7),
      (     0,    -0.2,     2.7),
      (1.3375,       0, 2.53125),
      (1.3375,  -0.749, 2.53125),
      ( 0.749, -1.3375, 2.53125),
      (     0, -1.3375, 2.53125),
      (1.4375,       0, 2.53125),
      (1.4375,  -0.805, 2.53125),
      ( 0.805, -1.4375, 2.53125),
      (     0, -1.4375, 2.53125),
      (   1.5,       0,     2.4),
      (   1.5,   -0.84,     2.4),
      (  0.84,    -1.5,     2.4),
      (     0,    -1.5,     2.4),
      (  1.75,       0,   1.875),
      (  1.75,   -0.98,   1.875),
      (  0.98,   -1.75,   1.875),
      (     0,   -1.75,   1.875),
      (     2,       0,    1.35),
      (     2,   -1.12,    1.35),
      (  1.12,      -2,    1.35),
      (     0,      -2,    1.35),
      (     2,       0,     0.9),
      (     2,   -1.12,     0.9),
      (  1.12,      -2,     0.9),
      (     0,      -2,     0.9),
      (    -2,       0,     0.9),
      (     2,       0,    0.45),
      (     2,   -1.12,    0.45),
      (  1.12,      -2,    0.45),
      (     0,      -2,    0.45),
      (   1.5,       0,   0.225),
      (   1.5,   -0.84,   0.225),
      (  0.84,    -1.5,   0.225),
      (     0,    -1.5,   0.225),
      (   1.5,       0,    0.15),
      (   1.5,   -0.84,    0.15),
      (  0.84,    -1.5,    0.15),
      (     0,    -1.5,    0.15),
      (  -1.6,       0,   2.025),
      (  -1.6,    -0.3,   2.025),
      (  -1.5,    -0.3,    2.25),
      (  -1.5,       0,    2.25),
      (  -2.3,       0,   2.025),
      (  -2.3,    -0.3,   2.025),
      (  -2.5,    -0.3,    2.25),
      (  -2.5,       0,    2.25),
      (  -2.7,       0,   2.025),
      (  -2.7,    -0.3,   2.025),
      (    -3,    -0.3,    2.25),
      (    -3,       0,    2.25),
      (  -2.7,       0,     1.8),
      (  -2.7,    -0.3,     1.8),
      (    -3,    -0.3,     1.8),
      (    -3,       0,     1.8),
      (  -2.7,       0,   1.575),
      (  -2.7,    -0.3,   1.575),
      (    -3,    -0.3,    1.35),
      (    -3,       0,    1.35),
      (  -2.5,       0,   1.125),
      (  -2.5,    -0.3,   1.125),
      ( -2.65,    -0.3,  0.9375),
      ( -2.65,       0,  0.9375),
      (    -2,    -0.3,     0.9),
      (  -1.9,    -0.3,     0.6),
      (  -1.9,       0,     0.6),
      (   1.7,       0,   1.425),
      (   1.7,   -0.66,   1.425),
      (   1.7,   -0.66,     0.6),
      (   1.7,       0,     0.6),
      (   2.6,       0,   1.425),
      (   2.6,   -0.66,   1.425),
      (   3.1,   -0.66,   0.825),
      (   3.1,       0,   0.825),
      (   2.3,       0,     2.1),
      (   2.3,   -0.25,     2.1),
      (   2.4,   -0.25,   2.025),
      (   2.4,       0,   2.025),
      (   2.7,       0,     2.4),
      (   2.7,   -0.25,     2.4),
      (   3.3,   -0.25,     2.4),
      (   3.3,       0,     2.4),
      (   2.8,       0,   2.475),
      (   2.8,   -0.25,   2.475),
      ( 3.525,   -0.25, 2.49375),
      ( 3.525,       0, 2.49375),
      (   2.9,       0,   2.475),
      (   2.9,   -0.15,   2.475),
      (  3.45,   -0.15,  2.5125),
      (  3.45,       0,  2.5125),
      (   2.8,       0,     2.4),
      (   2.8,   -0.15,     2.4),
      (   3.2,   -0.15,     2.4),
      (   3.2,       0,     2.4),
      (     0,       0,    3.15),
      (   0.8,       0,    3.15),
      (   0.8,   -0.45,    3.15),
      (  0.45,    -0.8,    3.15),
      (     0,    -0.8,    3.15),
      (     0,       0,    2.85),
      (   1.4,       0,     2.4),
      (   1.4,  -0.784,     2.4),
      ( 0.784,    -1.4,     2.4),
      (     0,    -1.4,     2.4),
      (   0.4,       0,    2.55),
      (   0.4,  -0.224,    2.55),
      ( 0.224,    -0.4,    2.55),
      (     0,    -0.4,    2.55),
      (   1.3,       0,    2.55),
      (   1.3,  -0.728,    2.55),
      ( 0.728,    -1.3,    2.55),
      (     0,    -1.3,    2.55),
      (   1.3,       0,     2.4),
      (   1.3,  -0.728,     2.4),
      ( 0.728,    -1.3,     2.4),
      (     0,    -1.3,     2.4),
      (     0,       0,       0),
      ( 1.425,  -0.798,       0),
      (   1.5,       0,   0.075),
      ( 1.425,       0,       0),
      ( 0.798,  -1.425,       0),
      (     0,    -1.5,   0.075),
      (     0,  -1.425,       0),
      (   1.5,   -0.84,   0.075),
      (  0.84,    -1.5,   0.075) );

  TeaTex :
    Array[0..1, 0..1, 0..1] of GLfloat =
      ( ((0, 0), (1, 0)),
        ((0, 1), (1, 1)) );


procedure DrawBox(Size : GLfloat; DrawType : GLenum);
procedure glutWireCube(Size : GLDouble);
procedure glutSolidCube(Size : GLDouble);
procedure glutWireSphere(Radius:GLdouble; Slices:GLint;Stacks:GLint);
procedure glutSolidSphere(Radius:GLdouble;Slices : GLint;Stacks : GLint);
procedure glutWireCone(
  Base : GLdouble;
  Height : GLdouble;
  Slices : GLint;
  Stacks : GLint);
procedure glutSolidCone(
  Base : GLdouble;  Height : GLdouble;
  Slices : GLint;  Stacks : GLint);
procedure glutWireTorus(
  innerRadius : GLdouble;  outerRadius :GLdouble;
  nsides : GLint;
  rings : GLint);
procedure glutSolidTorus(
  innerRadius : GLdouble;  outerRadius :GLdouble;
  nsides : GLint;
  rings : GLint);
procedure glutWireDodecahedron;
procedure glutSolidDodecahedron;
procedure Octaheadron(ShadeType : GLenum);
procedure glutWireOctaheadron;
procedure glutSolidOctaheadron;
procedure Icosahedron(ShadeType : GLenum);
procedure glutWireIcosahedron;
procedure glutSolidIcosahedron;
procedure Tetrahedron(ShadeType : GLenum);
procedure glutWireTetrahedron;
procedure glutSolidTetrahedron;

{ Teapot stuff }
procedure Teapot(Grid : GLint; Scale : GLdouble; ShadeType : GLenum);
procedure glutWireTeapot(Scale : GLdouble);
procedure glutSolidTeapot(Scale : GLdouble);

{ Generally useful stuff }
procedure Diff3(a0, a1, a2, b0, b1, b2 : GLfloat; var c : array of GLfloat);
procedure CrossProd(v1, v2 : array of GLfloat; var prod : array of GLfloat);
procedure Normalize(var v : array of GLfloat);

var
  quadObj :{$ifdef fpc}PGLUquadric{$else}PGLUquadricObj{$endif};  dodec : array[0..19, 0..2] of GLfloat;

implementation

procedure DrawBox(Size : GLfloat; DrawType : GLenum);
var
  V : array[0..7, 0..2] of GLfloat;
  I : GLint;
  HalfSize : GLfloat;
begin { DrawBox }
  HalfSize := Size / 2;
  V[0, 0] := -HalfSize;
  V[1, 0] := -HalfSize;
  V[2, 0] := -HalfSize;
  V[3, 0] := -HalfSize;
  V[4, 0] := HalfSize;
  V[5, 0] := HalfSize;
  V[6, 0] := HalfSize;
  V[7, 0] := HalfSize;

  V[0, 1] := -HalfSize;
  V[1, 1] := -HalfSize;
  V[4, 1] := -HalfSize;
  V[5, 1] := -HalfSize;
  V[2, 1] := HalfSize;
  V[3, 1] := HalfSize;
  V[6, 1] := HalfSize;
  V[7, 1] := HalfSize;

  V[0, 2] := -HalfSize;
  V[3, 2] := -HalfSize;
  V[4, 2] := -HalfSize;
  V[7, 2] := -HalfSize;
  V[1, 2] := HalfSize;
  V[2, 2] := HalfSize;
  V[5, 2] := HalfSize;
  V[6, 2] := HalfSize;

  for I := 0 to 5 do
  begin
    glBegin(DrawType);
      glNormal3fv(@BoxPoints[I, 0]);
      glVertex3fv(@V[BoxFaces[I, 0], 0]);
      glVertex3fv(@V[BoxFaces[I, 1], 0]);
      glVertex3fv(@V[BoxFaces[I, 2], 0]);
      glVertex3fv(@V[BoxFaces[I, 3], 0]);
    glEnd;
  end;
end; { DrawBox }

procedure glutWireSphere(
  Radius : GLdouble;
  Slices : GLint;
  Stacks : GLint);
begin { glutWireSphere }
  if quadObj = nil then
    quadObj := gluNewQuadric();
  gluQuadricDrawStyle(quadObj, GLU_LINE);
  gluQuadricNormals(quadObj, GLU_SMOOTH);
  gluSphere(quadObj, Radius, Slices, Stacks);
end; { glutWireSphere }

procedure glutSolidSphere(
  Radius : GLdouble;
  Slices : GLint;
  Stacks : GLint);
begin { glutSolidSphere }
  if quadObj = nil then
    quadObj := gluNewQuadric();
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  gluQuadricNormals(quadObj, GLU_SMOOTH);
  gluSphere(quadObj, Radius, Slices, Stacks);
end; { glutSolidSphere }

procedure glutWireCube(Size : GLDouble);
begin { glutWireCube }
  DrawBox(Size, GL_LINE_LOOP);
end; { glutWireCube }

procedure glutSolidCube(Size : GLDouble);
begin { glutSolidCube }
  DrawBox(Size, GL_QUADS);
end; { glutSolidCube }

procedure glutWireCone(
  Base : GLdouble;
  Height : GLdouble;
  Slices : GLint;
  Stacks : GLint);
begin { glutWireCone }
  if quadObj = nil then
    quadObj := gluNewQuadric();
  gluQuadricDrawStyle(quadObj, GLU_LINE);
  gluQuadricNormals(quadObj, GLU_SMOOTH);
  gluCylinder(quadObj, base, 0.0, height, slices, stacks);
end; { glutWireCone }

procedure glutSolidCone(
  Base : GLdouble;
  Height : GLdouble;
  Slices : GLint;
  Stacks : GLint);
begin { glutSolidCone }
  if quadObj = nil then
    quadObj := gluNewQuadric();
  gluQuadricDrawStyle(quadObj, GLU_FILL);
  gluQuadricNormals(quadObj, GLU_SMOOTH);
  gluCylinder(quadObj, base, 0.0, height, slices, stacks);
end; { glutSolidCone }

procedure Doughnut(
  inr : GLfloat;
  outR : GLfloat;
  nsides : GLint;
  rings : GLint;
  DrawType :GLenum);
var
  i, j : integer;
  theta, phi, theta1, phi1 : GLfloat;
  p0, p1, p2, p3,
  n0, n1, n2, n3 : array[0..2] of GLfloat;
begin { Doughnut }
  for i := 0 to rings - 1 do
  begin
    theta := i *2.0 * PI / rings;
    theta1 := (i + 1) * 2.0 * PI / rings;
    for j := 0 to nsides - 1 do
    begin
      phi := j *2.0 * PI / nsides;
      phi1 := (j + 1) * 2.0 * PI / nsides;

		p0[0] := cos(theta) * (outR + inr * cos(phi));
      p0[1] := -sin(theta) * (outR + inr * cos(phi));
      p0[2] := inr * sin(phi);

		p1[0] := cos(theta1) * (outR + inr * cos(phi));
      p1[1] := -sin(theta1) * (outR + inr * cos(phi));
      p1[2] := inr * sin(phi);

		p2[0] := cos(theta1) * (outR + inr * cos(phi1));
      p2[1] := -sin(theta1) * (outR + inr * cos(phi1));
      p2[2] := inr * sin(phi1);

      p3[0] := cos(theta) * (outR + inr * cos(phi1));
      p3[1] := -sin(theta) * (outR + inr * cos(phi1));
      p3[2] := inr * sin(phi1);

      n0[0] := cos(theta) * (cos(phi));
      n0[1] := -sin(theta) * (cos(phi));
      n0[2] := sin(phi);

      n1[0] := cos(theta1) * (cos(phi));
      n1[1] := -sin(theta1) * (cos(phi));
      n1[2] := sin(phi);

      n2[0] := cos(theta1) * (cos(phi1));
      n2[1] := -sin(theta1) * (cos(phi1));
      n2[2] := sin(phi1);

      n3[0] := cos(theta) * (cos(phi1));
      n3[1] := -sin(theta) * (cos(phi1));
      n3[2] := sin(phi1);

      glBegin(DrawType);
      glNormal3fv(@n3);
      glVertex3fv(@p3);
      glNormal3fv(@n2);
      glVertex3fv(@p2);
      glNormal3fv(@n1);
      glVertex3fv(@p1);
      glNormal3fv(@n0);
      glVertex3fv(@p0);
      glEnd();
    end;
  end;
end; { Doughnut }

procedure glutWireTorus(
  innerRadius : GLdouble;
  outerRadius :GLdouble;
  nsides : GLint;
  rings : GLint);
begin { glutWireTorus }
  Doughnut(innerRadius, outerRadius, nsides, rings, GL_LINE_LOOP);
end; { glutWireTorus }

procedure glutSolidTorus(
  innerRadius : GLdouble;
  outerRadius :GLdouble;
  nsides : GLint;
  rings : GLint);
begin { glutSolidTorus }
  Doughnut(innerRadius, outerRadius, nsides, rings, GL_QUADS);
end; { glutSolidTorus }

procedure initDodecahedron;
var
  alpha, beta : GLfloat;
begin { initDodecahedron }
  alpha := sqrt(2.0 / (3.0 + sqrt(5.0)));
  beta := 1.0 + sqrt(6.0 / (3.0 + sqrt(5.0)) -
    2.0 + 2.0 * sqrt(2.0 / (3.0 + sqrt(5.0))));

  dodec[0, 0] := -alpha;  dodec[0, 1] := 0;       dodec[0, 2] := beta;
  dodec[1, 0] := alpha;   dodec[1, 1] := 0;       dodec[1, 2] := beta;
  dodec[2, 0] := -1;      dodec[2, 1] := -1;      dodec[2, 2] := -1;
  dodec[3, 0] := -1;      dodec[3, 1] := -1;      dodec[3, 2] := 1;
  dodec[4, 0] := -1;      dodec[4, 1] := 1;       dodec[4, 2] := -1;
  dodec[5, 0] := -1;      dodec[5, 1] := 1;       dodec[5, 2] := 1;
  dodec[6, 0] := 1;       dodec[6, 1] := -1;      dodec[6, 2] := -1;
  dodec[7, 0] := 1;       dodec[7, 1] := -1;      dodec[7, 2] := 1;
  dodec[8, 0] := 1;       dodec[8, 1] := 1;       dodec[8, 2] := -1;
  dodec[9, 0] := 1;       dodec[9, 1] := 1;       dodec[9, 2] := 1;
  dodec[10, 0] := beta;   dodec[10, 1] := alpha;  dodec[10, 2] := 0;
  dodec[11, 0] := beta;   dodec[11, 1] := -alpha; dodec[11, 2] := 0;
  dodec[12, 0] := -beta;  dodec[12, 1] := alpha;  dodec[12, 2] := 0;
  dodec[13, 0] := -beta;  dodec[13, 1] := -alpha; dodec[13, 2] := 0;
  dodec[14, 0] := -alpha; dodec[14, 1] := 0;      dodec[14, 2] := -beta;
  dodec[15, 0] := alpha;  dodec[15, 1] := 0;      dodec[15, 2] := -beta;
  dodec[16, 0] := 0;      dodec[16, 1] := beta;   dodec[16, 2] := alpha;
  dodec[17, 0] := 0;      dodec[17, 1] := beta;   dodec[17, 2] := -alpha;
  dodec[18, 0] := 0;      dodec[18, 1] := -beta;  dodec[18, 2] := alpha;
  dodec[19, 0] := 0;      dodec[19, 1] := -beta;  dodec[19, 2] := -alpha;
end; { initDodecaheadron }

procedure Diff3(
  a0, a1, a2, b0, b1, b2 : GLfloat;
  var c : array of GLfloat);
begin { Diff3 }
  c[0] := a0 - b0;
  c[1] := a1 - b1;
  c[2] := a2 - b2;
end; { Diff3 }

procedure CrossProd(
  v1, v2 : array of GLfloat;
  var prod : array of GLfloat);
var
  p : array[0..2] of GLfloat;
begin { CrossProd }
  p[0] := v1[1] * v2[2] - v2[1] * v1[2];
  p[1] := v1[2] * v2[0] - v2[2] * v1[0];
  p[2] := v1[0] * v2[1] - v2[0] * v1[1];
  prod[0] := p[0];
  prod[1] := p[1];
  prod[2] := p[2];
end; { CrossProd }

procedure Normalize(var v : array of GLfloat);
var
  d : GLfloat;
begin { Normalize }
  d := sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  d := 1 / d;
  v[0] := v[0] * d;
  v[1] := v[1] * d;
  v[2] := v[2] * d;
end; { Normalize }

procedure Pentagon(a, b, c, d, e : integer; shadeType : GLenum);
var
  n0, d1, d2 : array[0..2] of GLfloat;
begin { Pentagon }
  Diff3(dodec[a, 0], dodec[a, 1], dodec[1, 2],
        dodec[b, 0], dodec[b, 1], dodec[b, 2], d1);
  Diff3(dodec[b, 0], dodec[b, 1], dodec[b, 2],
        dodec[c, 0], dodec[c, 1], dodec[c, 2], d2);
  CrossProd(d1, d2, n0);
  Normalize(n0);

  glBegin(shadeType);
    glNormal3fv(@n0);
    glVertex3fv(@dodec[a, 0]);
    glVertex3fv(@dodec[b, 0]);
    glVertex3fv(@dodec[c, 0]);
    glVertex3fv(@dodec[d, 0]);
    glVertex3fv(@dodec[e, 0]);
  glEnd();
end; { Pentagon }

procedure Dodecahedron(DrawType : GLenum);
begin { Dodecahedron }
  pentagon(0, 1, 9, 16, 5, DrawType);
  pentagon(1, 0, 3, 18, 7, DrawType);
  pentagon(1, 7, 11, 10, 9, DrawType);
  pentagon(11, 7, 18, 19, 6, DrawType);
  pentagon(8, 17, 16, 9, 10, DrawType);
  pentagon(2, 14, 15, 6, 19, DrawType);
  pentagon(2, 13, 12, 4, 14, DrawType);
  pentagon(2, 19, 18, 3, 13, DrawType);
  pentagon(3, 0, 5, 12, 13, DrawType);
  pentagon(6, 15, 8, 10, 11, DrawType);
  pentagon(4, 17, 8, 15, 14, DrawType);
  pentagon(4, 12, 5, 16, 17, DrawType);
end; { Dodecahedron }

procedure glutWireDodecahedron;
begin { glutWireDodecahedron }
  Dodecahedron(GL_LINE_LOOP);
end; { glutWireDodecahedron }

procedure glutSolidDodecahedron;
begin { glutSolidDodecahedron }
  Dodecahedron(GL_TRIANGLE_FAN);
end; { glutSolidDodecahedron }

procedure RecordItem(n1, n2, n3 :  array of GLfloat; ShadeType : GLenum);
var
  q0, q1 : array[0..2] of GLfloat;
begin { RecordItem }
  Diff3(n1[0], n1[1], n1[2], n2[0], n2[1], n2[2], q0);
  Diff3(n2[0], n2[1], n2[2], n3[0], n3[1], n3[2], q1);
  CrossProd(q0, q1, q1);
  Normalize(q1);

  glBegin(shadeType);
    glNormal3fv(@q1);
    glVertex3fv(@n1);
    glVertex3fv(@n2);
    glVertex3fv(@n3);
  glEnd;
end; { RecordItem }

procedure SubDivide(
  v0, v1, v2 : array of GLfloat;
  ShadeType : GLenum);
var
  Depth : Integer;
  w0, w1, w2 : array[0..2] of GLfloat;
  l : GLfloat;
  i, j, k, n : Integer;
begin { SubDivide }
  Depth := 1;
  for i := 0 to Depth - 1 do
  begin
    j := 0;
    while i + j < Depth do
    begin
      k := depth - i - j;
      for n := 0 to 2 do
      begin
        w0[n] := (i * v0[n] + j * v1[n] + k * v2[n]) / depth;
        w1[n] := ((i + 1) * v0[n] + j * v1[n] + (k - 1) * v2[n])
          / depth;
        w2[n] := (i * v0[n] + (j + 1) * v1[n] + (k - 1) * v2[n])
          / depth;
      end;
      l := sqrt(w0[0] * w0[0] + w0[1] * w0[1] + w0[2] * w0[2]);
      w0[0] := w0[0] / l;
      w0[1] := w0[1] / l;
      w0[2] := w0[2] / l;
      l := sqrt(w1[0] * w1[0] + w1[1] * w1[1] + w1[2] * w1[2]);
      w1[0] := w1[0] / l;
      w1[1] := w1[1] / l;
      w1[2] := w1[2] / l;
      l := sqrt(w2[0] * w2[0] + w2[1] * w2[1] + w2[2] * w2[2]);
      w2[0] := w2[0] / l;
      w2[1] := w2[1] / l;
      w2[2] := w2[2] / l;
      RecordItem(w1, w0, w2, ShadeType);
      Inc(j);
    end;
  end;
end; { SubDivide }

procedure DrawTriangle(
  I : Integer;
  Data : Array of TGLfloat3v;
  NDX : Array of TInteger3v;
  ShadeType : GLenum);
var
  X0, X1, X2 : TGLfloat3v;
begin { DrawTriangle }
  X0 := Data[NDX[i, 0]];
  X1 := Data[NDX[i, 1]];
  X2 := Data[NDX[i, 2]];
  SubDivide(X0, X1, X2, ShadeType);
end; { DrawTriangle }

procedure Octaheadron(ShadeType : glEnum);
var
  I : Integer;
begin { Octaheadron }
  for I := 0 to 7 do
    DrawTriangle(I, OctData, OctIndex, ShadeType);
end; { Octaheadron }

procedure glutWireOctaheadron;
begin { glutWireOctaheadron }
  Octaheadron(GL_LINE_LOOP);
end; { glutWireOctaheadron }

procedure glutSolidOctaheadron;
begin { glutSolidOctaheadron }
  Octaheadron(GL_TRIANGLES);
end; { glutSolidOctaheadron }

procedure Icosahedron(ShadeType : GLenum);
var
  I : Integer;
begin { Icosahedron }
  for I := 0 to 19 do
    DrawTriangle(I, IcoData, IcoIndex, ShadeType);
end; { Icosahedron }

procedure glutWireIcosahedron;
begin { glutWireIcosahedron }
  Icosahedron(GL_LINE_LOOP);
end; { glutWireIcosahedron }

procedure glutSolidIcosahedron;
begin { glutSolidIcosahedron }
  Icosahedron(GL_TRIANGLES);
end; { glutSolidIcosahedron }

procedure Tetrahedron(ShadeType : GLenum);
var
  I : Integer;
begin { Tetrahedron }
  for I := 0 to 3 do
    DrawTriangle(I, TetData, TetIndex, ShadeType);
end; { Tetrahedron }

procedure glutWireTetrahedron;
begin { glutWireTetrahedron }
  Tetrahedron(GL_LINE_LOOP);
end; { glutWireTetrahedron }

procedure glutSolidTetrahedron;
begin { glutSolidTetrahedron }
  Tetrahedron(GL_TRIANGLES);
end; { glutSolidTetrahedron }

{ Teapot stuff }

procedure Teapot(Grid : GLint; Scale : GLdouble; ShadeType : GLenum);
var
  P, Q, R, S : Array[0..3, 0..3, 0..2] of GLfloat;
  I, J, K, L : GLInt;
begin { Teapot }
  glPushAttrib(GL_ENABLE_BIT or GL_EVAL_BIT);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glEnable(GL_MAP2_VERTEX_3);
  glEnable(GL_MAP2_TEXTURE_COORD_2);
  glPushMatrix;
  glRotatef(270.0, 1.0, 0.0, 0.0);
  glScalef(0.5 * Scale, 0.5 * Scale, 0.5 * Scale);
  glTranslatef(0.0, 0.0, -1.5);
  for I := 0 to 9 do
  begin
    for J := 0 to 3 do
      for K := 0 to 3 do
        for L := 0 to 2 do
        begin
          P[J, K, L] := TeaData[PatchData[I, J * 4 + K], L];
          Q[J, K, L] := TeaData[PatchData[I, J * 4 + (3 - K)], L];
          if L = 1 then
            Q[J, K, L] := -Q[J, K, L];
          if i < 6 then
          begin
            R[J, K, L] := TeaData[PatchData[I, J * 4 + (3 - K)], L];
            if L = 0 then
              R[J, K, L] := -R[J, K, L];
            S[J, K, L] := TeaData[PatchData[I, J * 4 + K], L];
            if L = 0 then
              S[J, K, L] := -S[J, K, L];
            if L = 1 then
              S[J, K, L] := -S[J, K, L];
          end;
        end;
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, @TeaTex);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, @P[0, 0, 0]);
    glMapGrid2f(Grid, 0.0, 1.0, Grid, 0.0, 1.0);
    glEvalMesh2(ShadeType, 0, Grid, 0, Grid);
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, @Q[0, 0, 0]);
    glEvalMesh2(ShadeType, 0, Grid, 0, Grid);
    if I < 6 then
    begin
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, @R[0, 0, 0]);
      glEvalMesh2(ShadeType, 0, Grid, 0, Grid);
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, @S[0, 0, 0]);
      glEvalMesh2(ShadeType, 0, Grid, 0, Grid);
    end;
  end;
  glPopMatrix;
  glPopAttrib;
end; { Teapot }

procedure glutWireTeapot(Scale : GLdouble);
begin { glutWireTeapot }
  Teapot(10, Scale, GL_LINE);
end; { glutWireTeapot }

procedure glutSolidTeapot(Scale : GLdouble);
begin { glutSolidTeapot }
  Teapot(14, Scale, GL_FILL);
end; { glutSolidTeapot }

initialization
  initDodecahedron;

end.

