-- VHDL-AMS description of a switched capacitor integrator circuit
-- that has been modified to prevent the voltage from railing.  In this
-- circuit, a resistor in the form of a switched capacitor is inserted
-- in parallel with the feedback capacitor. This causes Vout to drift
-- back to 0 V. In other words, if Vout is increasing, it increases
-- faster when it is far below 0 V than when it is near or above 0 V.
-- Therefore, the model for this circuit uses a Vout range of 22 to 24
-- mV/μs when it is below −1000 mV, and it uses a range of 16 to 22
-- mV/μs when it is above −1000 mV. A similar modification is made
-- for the ranges of rates when Vout is decreasing. 

library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity integrator is
end integrator;

architecture switchCap of integrator is
   quantity vout:real; 
   signal vin: std_logic := '0';
     
begin

   break vout=> -5280.0;     --Initial conditions
   vout'dot == span(16.0,22.0);

   if vin='0' use
     if (not vout'above(-5280.0)) use
       vout'dot == span(22.0,24.0);
     else
       vout'dot == span(16.0,22.0);
     end use;
   elsif vin='1' use
     if (vout'above(5280.0)) use
       vout'dot == span(-24.0,-22.0);
     else
       vout'dot == span(-22.0,-16.0);
     end use;
   end use;

   process
   begin
     assign(vin,'1',528,528);
     assign(vin,'0',528,528);
   end process;

   -- This property is sued to ensure that the output voltage does
   -- not rail.  In this example, the property should always fail.
   assert (vout'above(-10560.0) and not vout'above(10560.0))
     report "Error: The output voltage railed."
     severity failure;

     
end switchCap;
