library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity DRC is
end DRC;

architecture controller of DRC is
  quantity y1: real;                    -- Sensor 1 stopwatch
  quantity y2: real;                    -- Sensor 2 stopwatch
  quantity x1: real;                    -- Scheduler's stopwatch for sensor 1
  quantity x2: real;                    -- Scheduler's stopwatch for sensor 2
  quantity z: real;                     -- Controller's stopwatch
  signal read1A : std_logic := '0';     -- Tells sensor 1 to take a reading
  signal read1B : std_logic := '0';     -- Sync label counterpart
  signal read2A : std_logic := '0';     -- Tells sensor 2 to take a reading
  signal read2B : std_logic := '0';     -- Sync label counterpart
  signal request1A : std_logic := '0';  -- Requests signal to go to controller
  signal request1B : std_logic := '0';  -- Sync label counterpart
  signal request2A : std_logic := '0';  -- Requests signal to go to controller
  signal request2B : std_logic := '0';  -- Sync label counterpart
  signal send1A : std_logic := '0';     -- Signal from 1 sent to controller
  signal send1B : std_logic := '0';     -- Sync label counterpart
  signal send2A : std_logic := '0';     -- Signal from 2 sent to controller
  signal send2B : std_logic := '0';     -- Sync label counterpart
  signal ack1A : std_logic := '0';      -- Signal from 1 received
  signal ack1B : std_logic := '0';      -- Sync label counterpart
  signal ack2A : std_logic := '0';      -- Signal from 2 received
  signal ack2B : std_logic := '0';      -- Sync label counterpart
  signal busy : std_logic := '0';       -- Whether the controller can accept a
                                        -- signal
  signal flag1 : std_logic := '0';
  signal flag2 : std_logic := '0';

begin  

  break y1=> 60.0, y2=> 60.0;             -- Initial conditions

  break y1=> 0.0 when read1B = '1' or ack1B = '1';
  break y2=> 0.0 when read2B = '1' or ack2B = '1';
  break x1=> 0.0 when request1B = '1';
  break x2=> 0.0 when request2B = '1';
  break z=> 0.0 when send1B = '1' or send2B = '1' or ack1A = '1' or ack2A ='1';

  y1'dot == 10.0;                        -- Constant clock rates
  y2'dot == 10.0;
  x1'dot == 10.0;
  x2'dot == 10.0;
  z'dot == 10.0;

  sensor1: process
  begin
    if not y1'above(60.0) then
      wait until y1'above(60.0);
    end if;
    sync(request1A,request1B);
    sync(read1B,read1A);
    vassign(flag1,'0',0,0);
    while flag1 = '0' loop
      if busy = '1' then
        wait until y1'above(40.0) or busy = '0';
        if busy = '0' then
          sync(send1A,send1B);
          sync(ack1B,ack1A);
          assign(flag1,'1',0,0);
        elsif y1'above(40.0) then
          sync(request1A,request1B);
        end if;
      elsif busy = '0' then
        sync(send1A,send1B);
        sync(ack1B,ack1A);
        assign(flag1,'1',0,0);
      end if;
    end loop;
      --while not y1'above(4.0) loop
      --  if send1B = '1' then
      --    sync(send1A,send1B);
      --    sync(ack1B,ack1A);
      --    assign(flag1,'1',0,0);
      --  end if;
      --end loop;
      --if flag1 = '0' then
      --  sync(request1A,request1B);
      --end if;
      --vassign(flag1,'0',0,0);
  end process sensor1;

  sensor2: process
  begin
    if not y1'above(60.0) then
      wait until y2'above(60.0);
    end if;
    sync(request2A,request2B);
    sync(read2B,read2A);
    vassign(flag2,'0',0,0);
    while flag2 = '0' loop
      if busy = '1' then
       wait until y2'above(40.0) or busy = '0';
        if busy = '0' then
          sync(send2A,send2B);
          sync(ack2B,ack2A);
          assign(flag2,'1',0,0);
        elsif y2'above(40.0) then
          sync(request2A,request2B);
        end if;
      elsif busy = '0' then
        sync(send2A,send2B);
        sync(ack2B,ack2A);
        assign(flag2,'1',0,0);
      end if;
    end loop;
      --while not y2'above(4.0) loop
      --  if send2B = '1' then
      --    sync(send2A,send2B);
      --    sync(ack2B,ack2A);
      --    assign(flag2,'1',0,0);
      --  end if;
      --end loop;
      --if flag2 = '0' then
      --  sync(request2A,request2B);
      --end if;
      --vassign(flag2,'0',0,0);
  end process sensor2;

  scheduler: process
  begin
    if not (request1A = '1' or request1B = '1') then
      wait on request1A, request1B;
    end if;
    if request1A = '1' then
      sync(request1B,request1A);
      if not request2A = '1' then
        wait until x1'above(5.0) or request2A = '1';
      end if;
      while request2A = '1' loop
        sync(request2B,request2A);
        wait for delay(15,20);
        sync(read2A,read2B);
        wait for delay(5,11);
      end loop;
      sync(read1A,read1B);
    elsif request2A = '1' then
     sync(request2B,request2A);
      wait until x2'above(15.0) or request1A = '1';
      if request1A = '1' then
        sync(request1B,request1A);
        while request2A = '1' loop
          wait for delay(15,20);
         sync(read2A,read2B);
          wait for delay(5,11);
        end loop;
       sync(read1A,read1B);
      else
        sync(read2A,read2B);
      end if;
   end if;
  end process scheduler;

  control: process
  begin
    if not (send1A = '1' or send2A = '1') then
      wait on send1A, send2A;
    end if;
    if send1A = '1' then
      sync(send1B,send1A);
      assign(busy,'1',0,0);
      wait for delay(9,10);
      sync(ack1A,ack1B);
      assign(busy,'0',0,0);
      if not (send2A = '1' or z'above(100.0)) then
        wait until send2A = '1' or z'above(100.0);
      end if;
      if send2A = '1' then
        sync(send2B,send2A);
        assign(busy,'1',0,0);
        wait for delay(9,10);
        sync(ack2A,ack2B);
        wait for delay(36,56);
        assign(busy,'0',0,0);
      end if;
    elsif send2A = '1' then
      sync(send2B,send2A);
      assign(busy,'1',0,0);
      wait for delay(9,10);
      sync(ack2A,ack2B);
      assign(busy,'0',0,0);
      if not send1A = '1' then
        wait until send1A = '1' or z'above(100.0);
      end if;
      if send1A = '1' then
        sync(send1B,send1A);
        assign(busy,'1',0,0);
        wait for delay(9,10);
        sync(ack1A,ack1B);
        wait for delay(36,56);
        assign(busy,'0',0,0);
      end if;
    end if;
  end process control;
  
end controller;
