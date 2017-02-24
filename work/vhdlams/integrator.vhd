library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity integrator is
end integrator;

architecture switchCap of integrator is
   quantity vout, c: real;          -- Output Voltage
   --quantity c:    real;          -- Clock
   signal inc, inc2: std_logic := '1'; -- Is the voltage increasing?
		 
begin

   break vout=> -1000.0, c=> 0.0;     --Initial conditions
   break c=> 0.0 when c'above(100.0); -- Reset the clock

   c'dot == 1.0; -- Constant clock rate

   if inc = '1' use
      vout'dot == span(19.0,20.0);
   elsif inc = '0' use
      vout'dot == span(-20.0,-19.0);
   end use;

   process
   begin
     wait until c'above(100.0);
     assign(inc,'0',1,2);
     wait until c'above(100.0);
     assign(inc,'1',1,2);
   end process;
     
end switchCap;
