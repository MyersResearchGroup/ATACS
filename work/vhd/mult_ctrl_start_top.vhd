------------------------------------------------------------------------------
-- atacs -mc -mV -mz -mU -mP -mT -ms -mD -M20 -mG -sv mult_ctrl_start.vhd mult_ctrl_start_env.vhd mult_ctrl_start_top.vhd mult_ctrl_start
------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.handshake.all;

entity mult_ctrl_start_top is
end mult_ctrl_start_top;

architecture behavior of mult_ctrl_start_top is
  
  component mult_ctrl_start_env
    port(
      reset : inout std_logic;
      -- ctrl lines from the sender
      reqstart : inout std_logic;
      ackstart : in std_logic;
      -- ctrl lines to the ripple circuit
      req : in std_logic;
      ack : inout std_logic;
      
      -- latch trigger in
      ab_done : inout std_logic;
      csa_done : inout std_logic;
      
      -- latch triggers out
      latch_csa : in std_logic;
      latch_ab : in std_logic;
      
      -- inputs used to determine control for b's
      a1 : inout std_logic;
      a0 : inout std_logic;
      l : inout std_logic;
      -- control for the b's
      bneg : in std_logic;
      b2b : in std_logic;
      bzero : in std_logic;
      
      -- used when starting a new multiply to clear c and s,
      -- and to setup to latch a new A.
      clr : in std_logic;
      newa : in std_logic
      );
  end component;
  
  component mult_ctrl_start
    port(
      reset : in std_logic;
      -- ctrl lines from the sender
      reqstart : in std_logic;
      ackstart : inout std_logic;
      -- ctrl lines to the ripple circuit
      req : inout std_logic;
      ack : in std_logic;

      -- latch trigger in
      ab_done : in std_logic; -- pulse comes back when ab is latched
      csa_done : in std_logic; -- pulse comes back when csa is latched

      -- latch triggers out
      latch_csa : inout std_logic;
      latch_ab : inout std_logic;
      
      -- inputs used to determine control for b's
      a1 : in std_logic;
      a0 : in std_logic;
      l : in std_logic;
      -- control for the b's
      bneg : inout std_logic;
      b2b : inout std_logic;
      bzero : inout std_logic;
      
      -- used when starting a new multiply to clear c and s,
      -- and to setup to latch a new A.
      clr : inout std_logic;
      newa : inout std_logic
      );
  end component;

  signal reset : std_logic := '0';
  signal reqstart : std_logic := '0';
  signal ackstart : std_logic := '0';
  signal req : std_logic := '0';
  signal ack : std_logic := '0';
  signal ab_done : std_logic := '0';
  signal csa_done : std_logic := '0';
  signal latch_csa : std_logic := '0';
  signal latch_ab : std_logic := '0';
  signal a1 : std_logic := '0';
  signal a0 : std_logic := '0';
  signal l :  std_logic := '0';
  signal bneg : std_logic := '0';
  signal b2b : std_logic := '0';
  signal bzero : std_logic := '0';
  signal clr : std_logic := '0';
  signal newa : std_logic := '0';

begin
  mult_ctrl_start_0 : mult_ctrl_start
    port map (
      reset => reset,
      reqstart => reqstart,
      ackstart => ackstart,
      req => req,
      ack => ack,
      ab_done => ab_done,
      csa_done => csa_done,
      latch_csa => latch_csa,
      latch_ab => latch_ab,
      a1 => a1,
      a0 => a0,
      l => l,
      bneg => bneg,
      b2b => b2b,
      bzero => bzero,
      clr => clr,
      newa => newa
      );

  mult_ctrl_start_env_0 : mult_ctrl_start_env
    port map (
      reset => reset,
      reqstart => reqstart,
      ackstart => ackstart,
      req => req,
      ack => ack,
      ab_done => ab_done,
      csa_done => csa_done,
      latch_csa => latch_csa,
      latch_ab => latch_ab,
      a1 => a1,
      a0 => a0,
      l => l,
      bneg => bneg,
      b2b => b2b,
      bzero => bzero,
      clr => clr,
      newa => newa
      );
  
end behavior;

