------------------------------------------------------------------------------
  -- file: mult_ctrl_done.vhd
  -- control for the last half of the multiplier.
------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.handshake.all;

entity mult_ctrl_done is
  port(
    reset : in std_logic;
    -- ctrl lines from the receiver
    reqdone : inout std_logic := '0';
    ackdone : in std_logic;
    -- ctrl lines from the first half of the multiplier
    req : in std_logic;
    ack : inout std_logic := '0';

    -- latch trigger in
    latch_done : in std_logic; -- pulse comes back when data is latched
    compute_done : in std_logic; -- pulse comes back when data is computed

    -- latch data out
    latch_data : inout std_logic := '0'; -- causes data to latch
    -- compute delay out
    compute_data : inout std_logic := '0' --causes delay pulse matching compute
                                          -- line
    );
end mult_ctrl_done;
    
architecture behavior of mult_ctrl_done is
--@synthesis_off
  constant inf : integer := 0;
--@synthesis_on
begin

  ctrl_proc : process
  begin
    guard (req, '1');
    
    assign(latch_data, '1', 0, inf); -- start to pulse latch_data
    guard (latch_done, '1');

    -- respond to sender that data is latched AND start to pulse compute_data
    assign (ack, '1', 0, inf, compute_data, '1', 0, inf);
    guard (compute_done, '1');

    guard (ackdone, '0'); -- lazy active
    assign (reqdone, '1', 0, inf);
    
    assign(latch_data, '0', 0, inf,       -- reset the pulses
           compute_data, '0', 0, inf);

    guard (latch_done, '0');
    guard (compute_done, '0');

    guard (req, '0');
    assign (ack, '0', 0, inf);

    guard (ackdone, '1');
    assign (reqdone, '0', 0, inf);

  end process;
  
end behavior;

