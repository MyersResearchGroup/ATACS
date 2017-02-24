Vin = 0.3;
G = 5e-3;
L = 1e-6;
C = 1e-12;
Il = linspace(0,0.0011,4);
Vc = linspace(0,0.5,4);
fp = fopen("foo.txt","w","ieee-le");
fprintf(fp,"Il Vc dIl dVc\n");
for i=1:length(Il)
  for j=1:length(Vc)
    dIl=1/L*(-Vc(j)-Il(i)/G+Vin);
    dVc =  ite(Vc(j) < 0.055,1/C*(-(6.0105.*Vc(j).*Vc(j).*Vc(j)-0.9917.*Vc(j).*Vc(j)+0.0545.*Vc(j))+Il(i)),ite(Vc(j) >= 0.055 & Vc(j) < 0.35,1/C*(-(0.0692.*Vc(j).*Vc(j).*Vc(j)-0.0421.*Vc(j).*Vc(j)+0.004.*Vc(j)+8.9579e-4)+Il(i)),1/C*(-(0.2634.*Vc(j).*Vc(j).*Vc(j)-0.2765.*Vc(j).*Vc(j)+0.0968.*Vc(j)-0.0112)+Il(i))));
    fprintf(fp,"%g %g %g %g\n",round(Il(i)*1e+6),round(Vc(j)*1e+4),
	   round(dIl*1e-10*1e+6),round(dVc*1e-10*1e+4));
  endfor
endfor
fclose(fp);
