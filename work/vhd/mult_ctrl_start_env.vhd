------------------------------------------------------------------------------
  -- file: mult_ctrl_start.vhd
  -- control for the first half of the multiplier.
------------------------------------------------------------------------------
  
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity mult_ctrl_start_env is
  port(
    reset : inout std_logic := '0';
    -- ctrl lines from the sender
    reqstart : inout std_logic := '0';
    ackstart : in std_logic;
    -- ctrl lines to the ripple circuit
    req : in std_logic;
    ack : inout std_logic := '0';

    -- latch trigger in
    ab_done : inout std_logic := '0';
    csa_done : inout std_logic := '0';

    -- latch triggers out
    latch_csa : in std_logic;
    latch_ab : in std_logic;
    
    -- inputs used to determine control for b's
    a1 : inout std_logic := '0';
    a0 : inout std_logic := '0';
    l : inout std_logic := '0';
    -- control for the b's
    bneg : in std_logic;
    b2b : in std_logic;
    bzero : in std_logic;

    -- used when starting a new multiply to clear c and s,
    -- and to setup to latch a new A.
    clr : in std_logic;
    newa : in std_logic
    );
end mult_ctrl_start_env;

architecture behavior of mult_ctrl_start_env is
--  constant inf : integer := 2000000000;
begin

  a1_proc : process
    variable var : integer;
  begin
    guard_or( latch_ab, '1', latch_csa, '1');
    var := selection(1);
    if (var = 1) then
      vassign(a1, '0', 0, inf);
    else
      vassign(a1, '1', 0, inf);
    end if;
    guard_and( latch_ab, '0', latch_csa, '0');
  end process;

  a0_proc : process
    variable var : integer;
  begin
    guard_or( latch_ab, '1', latch_csa, '1');
    var := selection(1);
    if (var = 1) then
      vassign(a0, '0', 0, inf);
    else
      vassign(a0, '1', 0, inf);
    end if;
    guard_and( latch_ab, '0', latch_csa, '0');
  end process;

  l_proc : process
    variable var : integer;
  begin
    guard_or( latch_ab, '1', latch_csa, '1');
    var := selection(1);
    if (var = 1) then
      vassign(l, '0', 0, inf);
    else
      vassign(l, '1', 0, inf);
    end if;
    guard_and( latch_ab, '0', latch_csa, '0');
  end process;
  
  start_proc : process
  begin
    assign(reqstart, '1', 0, inf);
    guard(ackstart, '1');
    assign(reqstart, '0', 0, inf);
    guard(ackstart, '0');
  end process;

  receive_proc : process
  begin
    guard(req, '1');
    assign(ack, '1', 0, inf);
    guard(req, '0');
    assign(ack, '0', 0, inf);
  end process;

  csa_proc : process
  begin
    guard(latch_csa, '1');
    assign(csa_done, '1', 0, inf);
    guard(latch_csa, '0');
    assign(csa_done, '0', 0, inf);
  end process;

  ab_proc : process
  begin
    guard(latch_ab, '1');
    assign(ab_done, '1', 0, inf);
    guard(latch_ab, '0');
    assign(ab_done, '0', 0, inf);
  end process;


end behavior;

