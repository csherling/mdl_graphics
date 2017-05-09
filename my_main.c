/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
                     based on the provided values, then 
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a 
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store 
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct matrix *edges;
  screen s;

  struct stack *systems = new_stack();

  double step = 0.1;
  
  color c;
  c.red = 128;
  c.green = 128;
  c.blue = 128;
  
  tmp = new_matrix(4, 1000);
  edges = new_matrix(4, 1000);
  clear_screen( s );

  ident(edges);
  
  printf("Hello 1\n");
  
  double ax, dgs, theta;
  ax = 0;
  dgs = 0;
  theta = 0;

  char * nem;
  nem = (char *)malloc(100);

  SYMTAB * tem;
  
  for (i=0;i<lastop;i++) {  
    switch (op[i].opcode) {
    case PUSH:
      push(systems);
      printf("Hello Push\n");
      break;
    case POP:
      pop(systems);
      printf("Hello Pop\n");
      break;
    case MOVE:
      tmp = make_translate(op[i].op.move.d[0],
			   op[i].op.move.d[1],
			   op[i].op.move.d[2]
			   );
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      printf("Hello Move\n");
      break;
    case ROTATE:
      ax = op[i].op.rotate.axis;
      dgs = op[i].op.rotate.degrees;
      theta = dgs * (M_PI / 180);
      
      printf("ax %lf \n", ax);
      printf("dgs %lf \n", dgs);
      printf("theta %lf \n", theta);
      
      if(ax==0){
	tmp = make_rotX(theta);
      }
      else if(ax==1){
	tmp = make_rotY(theta);
      }
      else{
	tmp = make_rotZ(theta);
      }
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      printf("Hello Rotate\n");
      break;
    case SCALE:
      tmp = make_scale(op[i].op.scale.d[0],
		       op[i].op.scale.d[1],
		       op[i].op.scale.d[2]
		       );      
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      printf("Hello Scale\n");
      break;
    case BOX:
      add_box(edges,
	      op[i].op.box.d0[0],
	      op[i].op.box.d0[1],
	      op[i].op.box.d0[2],
	      op[i].op.box.d1[0],
	      op[i].op.box.d1[1],
	      op[i].op.box.d1[2]
	      );
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      printf("Hello Box\n");
      break;
    case SPHERE:
      add_sphere( edges,
		  op[i].op.sphere.d[0],
		  op[i].op.sphere.d[1],
		  op[i].op.sphere.d[2],
		  op[i].op.sphere.r,
		  step
		  );
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      printf("Hello Sphere\n");
      break;
    case TORUS:
      add_torus( edges,
		 op[i].op.torus.d[0],
		 op[i].op.torus.d[1],
		 op[i].op.torus.d[2],
		 op[i].op.torus.r0,
		 op[i].op.torus.r1,
		 step);
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      printf("Hello Torus\n");
      break;
    case LINE:
      add_edge(edges,
	       op[i].op.line.p0[0],
	       op[i].op.line.p0[1],
	       op[i].op.line.p0[2],
	       op[i].op.line.p1[0],
	       op[i].op.line.p1[1],
	       op[i].op.line.p1[2]
	       );      
      matrix_mult(peek(systems),edges);
      draw_lines(edges, s, c);
      edges->lastcol = 0;
      printf("Hello Line\n");
      break;
    case SAVE:
      printf("Hello Save1\n");
      tem = op[i].op.save.p;
      printf("Hello Save2\n");
      nem = tem->name;
      printf("Hello Save3\n");
      save_extension(s, nem);
      printf("Hello Save4\n");
      break;
    case DISPLAY:
      display( s );
      break;
    }
  }
}
