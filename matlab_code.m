%G1 = 1 + D^3 + D^4 = [10011] = 23 (octal)
%G2 = 1 + D + D^2 + D^4 = [11101] = 35(octal)

k = 5; %3 , 5
G1 = 23; %5 , 23
G2 = 35; %7 , 35
%polynomials = {'x4+ x1 + 1','x4 + x3 + x2 + 1'};
trellis = poly2trellis(k,[G1,G2]);
trellis.nextStates
%trellis.nextStates(19)
commcnv_plotnextstates(trellis.nextStates);
trellis.outputs
commcnv_plotoutputs(trellis.outputs,(k-1)*(k-1));

%x = [0,1,1,1,0,0];
%coded = convenc(x,trellis)

0 ,0,0 ,0 ,0,0,0,0, 0,0,0,0 ,0,0,0,0 ,0,0,0,0 ,0,0,0,0 ,1,1,0,1 ,0,1,0,1,
1,0,0,1, 1,0,1,1, 0,0,0,0 0000 1101 0110 1100 0000

tb = 2;
decoded = vitdec(coded,trellis,tb,'trunc','hard')