------------------------------------------------------------------------------
-- atacs -mc -mV -mz -mU -mP -mT -ms -mD -M20 -mG -sv mult_ctrl_done.vhd mult_ctrl_done_env.vhd mult_ctrl_done_top.vhd mult_ctrl_done
  -- file: mult_ctrl_done.vhd
  -- control for the last half of the multiplier.
------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use work.handshake.all;

entity mult_ctrl_done_top is
end mult_ctrl_done_top;

architecture behavior of mult_ctrl_done_top is

    component mult_ctrl_done_env
      port(
        reset : inout std_logic;
        -- ctrl lines from the receiver
        reqdone : in std_logic;
        ackdone : inout std_logic;
        -- ctrl lines from the first half of the multiplier
        req : inout std_logic;
        ack : in std_logic;
        
        -- latch trigger in
        latch_done : inout std_logic;
        compute_done : inout std_logic;
        
        -- latch data out
        latch_data : in std_logic;
        -- compute delay out
        compute_data : in std_logic
        );
    end component;

    component mult_ctrl_done
      port(
        reset : in std_logic;
        -- ctrl lines from the receiver
        reqdone : inout std_logic;
        ackdone : in std_logic;
        -- ctrl lines from the first half of the multiplier
        req : in std_logic;
        ack : inout std_logic;
        
        -- latch trigger in
        latch_done : in std_logic; -- pulse comes back when data is latched
        compute_done : in std_logic; -- pulse comes back when data is computed
        
        -- latch data out
        latch_data : inout std_logic;
        -- compute delay out
        compute_data : inout std_logic
        );
    end component;

    signal reset : std_logic := '0';
    signal reqdone : std_logic := '0';
    signal ackdone : std_logic := '0';
    signal req : std_logic := '0';
    signal ack : std_logic := '0';
    signal latch_done : std_logic := '0';
    signal compute_done : std_logic := '0';
    signal latch_data : std_logic := '0';
    signal compute_data : std_logic := '0';
      
begin
  mult_ctrl_done_0 : mult_ctrl_done
    port map ( reset => reset,
               reqdone => reqdone,
               ackdone => ackdone,
               req => req,
               ack => ack,
               latch_done => latch_done,
               compute_done => compute_done,
               latch_data => latch_data,
               compute_data => compute_data
               );

  mult_ctrl_done_env_0 : mult_ctrl_done_env
    port map ( reset => reset,
               reqdone => reqdone,
               ackdone => ackdone,
               req => req,
               ack => ack,
               latch_done => latch_done,
               compute_done => compute_done,
               latch_data => latch_data,
               compute_data => compute_data
               );
  
end behavior;

