use ieee.math_real.all;

package nondeterminism is
  function random(number:integer) return integer;
  function delay(l,u: integer) return time; 
end nondeterminism; 


PACKAGE BODY nondeterminism is
  function random(number:integer) return integer is
begin  
   return((RAND mod number)+1); 
end random;

function delay(l,u: integer) return time is
   variable randel : time;
   variable num,iter :    integer;
begin
    i1:randel := 0 ns; 
    loop
       num := random(u-l+1)+l-1; 
       iter := num / 1000000000;
       num := num mod 1000000000;
       for i in 1 to iter loop
       	 randel := randel + 1000000000 ns;
       end loop;
       iter := num / 100000000;
       num := num mod 100000000;
       for i in 1 to iter loop
       	 randel := randel + 100000000 ns;
       end loop;
       iter := num / 10000000;
       num := num mod 10000000;
       for i in 1 to iter loop
       	 randel := randel + 10000000 ns;
       end loop;
       iter := num / 1000000;
       num := num mod 1000000;
       for i in 1 to iter loop
       	 randel := randel + 1000000 ns;
       end loop;
       iter := num / 100000;
       num := num mod 100000;
       for i in 1 to iter loop
       	 randel := randel + 100000 ns;
       end loop;
       iter := num / 10000;
       num := num mod 10000;
       for i in 1 to iter loop
       	 randel := randel + 10000 ns;
       end loop;
       iter := num / 1000;
       num := num mod 1000;
       for i in 1 to iter loop
       	 randel := randel + 1000 ns;
       end loop;
       iter := num / 100;
       num := num mod 100;
       for i in 1 to iter loop
       	 randel := randel + 100 ns;
       end loop;
       iter := num / 10;
       num := num mod 10;
       for i in 1 to iter loop
       	 randel := randel + 10 ns;
       end loop;
       iter := num;
       for i in 1 to iter loop
       	 randel := randel + 1 ns;
       end loop;
       return randel;
    end loop;
end delay;

end nondeterminism;






