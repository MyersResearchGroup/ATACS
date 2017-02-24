library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity integrator is
end integrator;

architecture switchCap of integrator is
   quantity vout:real; -- Why cannot be on same line?
   quantity c:real;
   signal inc,inc16,inc24,clkrst: std_logic := '0';
     
begin

   break vout=> -1320.0, c=> 0.0;     --Initial conditions
   break c=> 0.0 when clkrst and c'above(132.0);

   c'dot == 1.0; -- Constant clock rate

   if inc='0' use
     if inc16='0' use
       if inc24='0' use
         vout'dot == -22.0;
       else
         vout'dot == -24.0;
       end use;
     else
       vout'dot == -16.0;
     end use;
   else 
     if inc16='0' use
       if inc24='0' use
         vout'dot == 22.0;
       else
         vout'dot == 24.0;
       end use;
     else
       vout'dot == 16.0;
     end use;
   end use;

   process
   begin
     assign(inc,'1',0,0);
     if (not vout'above(-1320)) then
       assign(inc24,'1',0,0);
       assign(inc24,'0',0,132);
     else
       assign(inc16,'1',0,0);
       assign(inc16,'0',0,132);
     end if;      
     wait until c'above(132.0);
     assign(clkrst,'1',0,0);
     wait until not c'above(132.0);
     assign(clkrst,'0',0,0);
     assign(inc,'0',0,0);
     if (vout'above(1320)) then
       assign(inc24,'1',0,0);
       assign(inc24,'0',0,132);
     else
       assign(inc16,'1',0,0);
       assign(inc16,'0',0,132);
     end if;      
     wait until c'above(132.0);
     assign(clkrst,'1',0,0);
     wait until not c'above(132.0);
     assign(clkrst,'0',0,0);
   end process;

   assert vout'above(-2640) and
     not vout'above(2640)
     report "Integrator saturates"
     severity failure;
     
end switchCap;

