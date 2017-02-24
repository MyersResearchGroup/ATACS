------------------------------------------------------------------------------
  -- file: mult_ctrl_start.vhd
  -- control for the first half of the multiplier.
------------------------------------------------------------------------------
  
library ieee;
use ieee.std_logic_1164.all;
use work.handshake.all;

entity mult_ctrl_start is
  port(
    reset : in std_logic;
    -- ctrl lines from the sender
    reqstart : in std_logic;
    ackstart : inout std_logic := '0';
    -- ctrl lines to the ripple circuit
    req : inout std_logic := '0';
    ack : in std_logic;

    -- latch trigger in
    ab_done : in std_logic; -- pulse comes back when ab is latched
    csa_done : in std_logic; -- pulse comes back when csa is latched

    -- latch triggers out
    latch_csa : inout std_logic := '0';  -- causes c,s,a to latch
    latch_ab : inout std_logic := '0'; -- causes a and b to latch
    
    -- inputs used to determine control for b's
    a1 : in std_logic;
    a0 : in std_logic;
    l : in std_logic;
    -- control for the b's
    bneg : inout std_logic := '0';
    b2b : inout std_logic := '0';
    bzero : inout std_logic := '0';

    -- used when starting a new multiply to clear c and s,
    -- and to setup to latch a new A.
    clr : inout std_logic := '0';
    newa : inout std_logic := '0'
    );
end mult_ctrl_start;

architecture behavior of mult_ctrl_start is
  signal state0 : std_logic := '0';
  signal state1 : std_logic := '0';
begin

  ctrl_proc : process
  begin
    guard (reqstart, '1');
    assign(clr, '1', 50, 100);
    assign(newa, '1', 50, 100);
    
    -- pulse latch_ab
    assign(latch_ab, '1', 50, 100);
    guard (ab_done, '1');
    
    -- respond to sender that data is latched.
    assign (ackstart, '1', 50, 100); -- moved up to get rid of csc
    
    assign(latch_ab, '0', 50, 100);
    guard (ab_done, '0');
    
    ----------------- state 01 -----------------
    assign(state0, '1', 50, 100);
    assign(clr, '0', 50, 100);
    assign(newa, '0', 50, 100);
    
    -- set bneg, b2b, bzero
    if (a1 = '0' and a0 = '0' and l = '0') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '1' and a0 = '1' and l = '1') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '1') then -- +2b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '0') then -- -2b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '0' and l = '1') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '0') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '1') then -- -b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
--    elsif (a1 = '1' and a0 = '1' and l = '0') then -- -b
    else
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
      -- else
      --  assert (false) report "illegal a1, a0, l" severity failure;
    end if;
    -- pulse latch_csa
    assign(latch_csa, '1', 50, 100);
    guard (csa_done, '1');

    ----------------- state 11 -----------------
    assign(state1, '1', 50, 100); -- moved up to get rid of csc.
    
    assign(latch_csa, '0', 50, 100);
    guard (csa_done, '0');
    
    -- set bneg, b2b, bzero
    if (a1 = '0' and a0 = '0' and l = '0') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '1' and a0 = '1' and l = '1') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '1') then -- +2b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '0') then -- -2b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '0' and l = '1') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '0') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '1') then -- -b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
--    elsif (a1 = '1' and a0 = '1' and l = '0') then -- -b
    else
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
      -- else
      --  assert (false) report "illegal a1, a0, l" severity failure;
    end if;
    
    -- pulse latch_csa
    assign(latch_csa, '1', 50, 100);
    guard (csa_done, '1');

    ----------------- state 10 -----------------
    assign(state0, '0', 50, 100); -- moved up to get rid of csc

    assign(latch_csa, '0', 50, 100);
    guard (csa_done, '0');
    
    -- set bneg, b2b, bzero
    if (a1 = '0' and a0 = '0' and l = '0') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '1' and a0 = '1' and l = '1') then -- +0
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100); -- don't care
      vassign(bzero, '1', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '1') then -- +2b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '0') then -- -2b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '1', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '0' and l = '1') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '0' and a0 = '1' and l = '0') then -- +b
      vassign(bneg, '0', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
    elsif (a1 = '1' and a0 = '0' and l = '1') then -- -b
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
--    elsif (a1 = '1' and a0 = '1' and l = '0') then -- -b
    else
      vassign(bneg, '1', 50, 100);
      vassign(b2b, '0', 50, 100);
      vassign(bzero, '0', 50, 100);
      -- else
      --  assert (false) report "illegal a1, a0, l" severity failure;
    end if;
    -- pulse latch_csa
    assign(latch_csa, '1', 50, 100);
    guard (csa_done, '1');

    ----------------- state 00 -----------------
    assign(state1, '0', 50, 100); -- moved up to get rid of csc

    assign(latch_csa, '0', 50, 100);
    guard (csa_done, '0');
    
    guard (ack, '0');
    assign (req, '1', 50, 100);
    guard_and (ack, '1', reqstart, '0');
    assign (req, '0', 50, 100, ackstart, '0', 50, 100);
  end process;
  
end behavior;

