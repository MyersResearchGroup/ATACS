-- VHDL-AMS description of a switched capacitor integrator circuit.
-- This model tracks the real quantity vout that represents the
-- output voltage of the circuit.  The std_logic variable in determines
-- the rate of vout using the if-use statements statement.  When vin is 0,
-- vout increases at a rate between 18 and 22 mV/us and when vin is 1,
-- vout decreases at a rate between -22 and -18 mV/us.

library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity integrator is
end integrator;

architecture switchCap of integrator is
   quantity vout: real;          -- Output Voltage
   signal vin: std_logic := '0'; -- Is the voltage increasing?
		 
begin

   break vout=> -1000.0;     --Initial conditions

   if vin = '0' use
      vout'dot == span(18.0,22.0);
   elsif vin = '1' use
      vout'dot == span(-22.0,-18.0);
   end use;

   process
   begin
     assign(vin,'1',100,100);
     assign(vin,'0',100,100);
   end process;

   -- This property is used to ensure that the output voltage does
   -- not rail.  In this example, the property should always fail.
   assert (vout'above(-2000.0) and not vout'above(2000.0))
     report "Error: The output voltage railed."
     severity failure;

end switchCap;
