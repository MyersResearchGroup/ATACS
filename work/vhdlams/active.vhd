library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity active is
end active;

architecture structure of active is
  quantity x: real;                     -- stopwatch in sensor
  quantity y: real;                     -- stopwatch in controller
  quantity t: real;                     -- timer in controller, checks period
                                        -- of 135
  quantity z: real;                     -- stopwatch in actuator
  quantity w: real;                     -- integrator value
  constant cutlow : real := 36.0;
  constant cuthi : real := 146.0;
  signal reset_x : std_logic := '0';
  signal reset_y : std_logic := '0';
  signal reset_z : std_logic := '0';
  signal reset_t : std_logic := '0';
  signal sync_sensorA : std_logic := '0';
  signal sync_sensorB : std_logic := '0';
  signal sync_controllerA : std_logic := '0';
  signal sync_controllerB : std_logic := '0';
  signal pulse : std_logic := '0';
  signal w_rate : std_logic := '0';

begin  -- structure

  break x=> 0.0, y=> 0.0, t=> 135.0, z=> 0.0;

  break x=> 0.0 when reset_x = '0' or reset_x = '1';
  break y=> 0.0 when reset_y = '0' or reset_y = '1';
  break z=> 0.0 when reset_z = '0' or reset_z = '1';
  break t=> 0.0 when reset_t = '1';

  x'dot == 1.0;
  y'dot == 1.0;
  z'dot == 1.0;
  t'dot == 1.0;

  if w_rate = '1' use
    w'dot == 1.0;
  elsif w_rate = '0' use
    w'dot == 0.0;
  end use;  

  sensor: process is
  begin  -- process sensor
    wait for span(50.0,55.0);
    assign(reset_x,'1',0,0);
    wait for 10.0;
    vassign(sync_sensorA,'1',0,0);
    assign(reset_x,'0',0,0);
    wait for 5.0;
    assign(reset_x,'1',0,0);
    wait until sync_sensorB = '0';
    vassign(sync_sensorA,'0',0,0);
    assign(reset_x,'0',0,0);
  end process sensor;

  controller: process is
  begin  -- process controller
    wait for 10.0;
    assign(reset_y,'1',0,0);
    wait until sync_sensorA = '1';
    vassign(sync_sensorB,'1',0,0);
    assign(reset_y,'0',0,0);
    wait for 5.0;
    vassign(sync_sensorB,'0',0,0);
    assign(reset_y,'1',0,0);
    if t'above(134.0) then
      if not t'above(135.0) then
        wait for 1.0;
      end if;
      assign(reset_t,'1',0,0);
      wait for span(20.0,25.0);
      assign(reset_y,'0',0,0);
      assign(reset_t,'0',0,0);
      wait for 10.0;
      vassign(sync_controllerA,'1',0,0);
      assign(reset_y,'1',0,0);
      wait for 5.0;
      wait until sync_controllerB = '0';
      vassign(sync_controllerA,'0',0,0);
      assign(reset_y,'0',0,0);
    else
      wait for span(20.0,25.0);
      assign(reset_y,'0',0,0);
    end if;
  end process controller;

  actuator: process is
  begin  -- process actuator
    wait for 10.0;
    assign(reset_z,'1',0,0);
    wait until sync_controllerA = '1';
    assign(sync_controllerB,'1',0,0);
    assign(reset_z,'0',0,0);
    wait for 5.0;
    vassign(sync_controllerB,'0',0,0);
    assign(reset_z,'1',0,0);
    assign(pulse,'1',0,0);
    wait for span(25.0,30.0);
    assign(pulse,'0',0,0);
    assign(reset_z,'0',0,0);
  end process actuator;

  sync: process is
  begin  -- process sync
    wait until pulse = '1';
    assign(w_rate,'1',0,0);
    wait until w'above(146.0) or pulse = '1';
    if w'above(36.0) and not w'above(146.0) and pulse = '0' then
    else
      assign(w_rate,'0',0,0);
    end if;
    wait for 500.0;
  end process sync;

end structure;
