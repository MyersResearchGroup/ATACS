------------------------------------------------------------------------------
  -- file: mult_ctrl_done.vhd
  -- control for the last half of the multiplier.
------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.handshake.all;

entity mult_ctrl_done_env is
  port(
    reset : inout std_logic := '0';
    -- ctrl lines from the receiver
    reqdone : in std_logic;
    ackdone : inout std_logic := '0';
    -- ctrl lines from the first half of the multiplier
    req : inout std_logic := '0';
    ack : in std_logic;

    -- latch trigger in
    latch_done : inout std_logic := '0';
    compute_done : inout std_logic := '0';

    -- latch data out
    latch_data : in std_logic;
    -- compute delay out
    compute_data : in std_logic
    );
end mult_ctrl_done_env;
    
architecture behavior of mult_ctrl_done_env is
--@synthesis_off
  constant inf : integer := 0;
--@synthesis_on
begin

  start_proc : process
  begin
    assign(req, '1', 0, inf);
    guard(ack, '1');
    assign(req, '0', 0, inf);
    guard(ack, '0');
  end process;

  receive_proc : process
  begin
    guard(reqdone, '1');
    assign(ackdone, '1', 0, inf);
    guard(reqdone, '0');
    assign(ackdone, '0', 0, inf);
  end process;

  data_proc : process
  begin
    guard(latch_data, '1');
    assign(latch_done, '1', 0, inf);
    guard(latch_data, '0');
    assign(latch_done, '0', 0, inf);
  end process;

  compute_proc : process
  begin
    guard(compute_data, '1');
    assign(compute_done, '1', 0, inf);
    guard(compute_data, '0');
    assign(compute_done, '0', 0, inf);
  end process;

end behavior;

