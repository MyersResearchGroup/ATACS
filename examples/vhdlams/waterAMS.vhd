-- This VHDL-AMS describes the water level monitor hybrid system example.
-- The water is initially at level 2 inches and increasing at a rate of 1
-- inch/minute.  Two to four  minutes after the water level reaches 20 inches,
-- the water begins decreasing at a rate of 2 inches/minute.  Two to four
-- time units after the water level reaches 10 inches, it returns to
-- increasing at rate of 1 inch/minute.
library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity water_level is
end water_level;

architecture monitor of water_level is
   quantity y: real; --water level
   signal inc: std_logic := '1'; --is the water lvl going up or down?
		 
begin

   break y=> 2.0; --Initial condition

   if inc = '1' use
      y'DOT == 1.0;
   elsif inc = '0' use
      y'DOT == -2.0;
   end use;

   process
   begin
     wait until y'above(20.0);
     assign(inc,'0',2,4);
     wait until not y'above(10.0);
     assign(inc,'1',2,4);
   end process;

   -- This property ensures that the water level never falls below
   -- 1 inch and never rises above 25 inches.  The property can be
   -- made to fail by increasing the lower bound above 1 or decreasing
   -- the upper bound below 25.
   assert y'above(1.0) and not y'above(25.0)
     report "Overfill/underfill of the tank!"
     severity FAILURE;
   
end monitor;
