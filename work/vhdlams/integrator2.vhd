library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity integrator is
end integrator;

architecture switchCap of integrator is
   quantity vout,c: real;          -- Output Voltage
   --quantity c:    real;          -- Clock
   signal inc19,inc20,clkrst: std_logic := '0'; -- Is the voltage increasing?
   --signal inc20: std_logic := '0'; -- Is the voltage increasing?
   --signal clkrst: std_logic := '0';
     
begin

   break vout=> -1000.0, c=> 0.0;     --Initial conditions
   break c=> 0.0 when clkrst and c'above(100.0);

   c'dot == 1.0; -- Constant clock rate

   if inc19='0' use
     if inc20='0' use
       vout'dot == -20.0;
     else
       vout'dot == -19.0;
     end use;
   else
     if inc20='0' use
       vout'dot == 19.0;
     else
       vout'dot == 20.0;
     end use;
   end use;

   process
   begin
     wait until vout'above(-2000.0);
     assign(inc19,'1',0,0);
     assign(inc20,'1',0,100);
     wait until c'above(100.0);
     assign(clkrst,'1',0,0);
     wait until not c'above(100.0);
     assign(clkrst,'0',0,0);
     wait until not vout'above(2000.0);
     assign(inc19,'0',0,0);
     assign(inc20,'0',0,100);
     wait until c'above(100.0);
     assign(clkrst,'1',0,0);
     wait until not c'above(100.0);
     assign(clkrst,'0',0,0);
   end process;
     
end switchCap;
