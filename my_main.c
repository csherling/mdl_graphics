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
  screen s;
  color g;

  struct stack *systems = new_stack();

  double step = 0.1;
  
  color c;
  c.red = 0;
  c.green = 0;
  c.blue = 0;
  
  tmp = new_matrix(4, 1000);
  clear_screen( s );

  for (i=0;i<lastop;i++) {  
    switch (op[i].opcode) {
    case PUSH:
      push(systems);
      break;
    case POP:
      pop(systems);	    
      break;
    case MOVE:
      tmp = make_translate(op[lastop].op.box.d0[0],
			   op[lastop].op.box.d0[1],
			   op[lastop].op.box.d0[2]
			   );
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      break;
    case ROTATE:
      int dir = op[lastop].op.rotate.axis;
      int dgs = op[lastop].op.rotate.degrees;
      int theta = dgs * (M_PI / 180);

      if(dir==0){
	tmp = make_rotX(theta);
      }
      else if(dir==1){
	tmp = make_rotY(theta);
      }
      else{
	tmp = make_rotZ(theta);
      }
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      break;
    case SCALE:
      tmp = make_scale(op[lastop].op.box.d0[0],
		       op[lastop].op.box.d0[1],
		       op[lastop].op.box.d0[2]
		       );      
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      break;
    case BOX:
      add_box(edges,
	      op[lastop].op.box.d0[0],
	      op[lastop].op.box.d0[1],
	      op[lastop].op.box.d0[2],
	      op[lastop].op.box.d1[0],
	      op[lastop].op.box.d1[1],
	      op[lastop].op.box.d1[2]
	      );
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      break;
    case SPHERE:
      add_sphere( edges,
		  op[lastop].op.box.d0[0],
		  op[lastop].op.box.d0[1],
		  op[lastop].op.box.d0[2],
		  op[lastop].op.sphere.r,
		  step
		  );
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      break;
    case TORUS:
      add_torus( edges,
		 op[lastop].op.box.d0[0],
		 op[lastop].op.box.d0[1],
		 op[lastop].op.box.d0[2],
		 op[lastop].op.sphere.r0,
		 op[lastop].op.sphere.r1,
		 step);
      matrix_mult(peek(systems), edges);
      draw_polygons(edges, s, c);
      edges->lastcol = 0;
      break;
    case LINE:
      add_edge(edges,
	       op[lastop].op.box.p0[0],
	       op[lastop].op.box.p0[1],
	       op[lastop].op.box.p0[2],
	       op[lastop].op.box.p1[0],
	       op[lastop].op.box.p1[1],
	       op[lastop].op.box.p1[2]
	       );      
      matrix_mult(stack->data[stack->top],edges);
      draw_edges(edges, s, c);
      edges->lastcol = 0;
      break;
    case SAVE:
      save_extension(s, op[loastop].op.save.p);
      break;
    case DISPLAY:
      display( s );
      break;
    }
  }
}
