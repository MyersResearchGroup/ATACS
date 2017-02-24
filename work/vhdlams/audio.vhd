library IEEE;
use IEEE.std_logic_1164.all;
use work.handshake.all;
use work.nondeterminism.all;

entity audio is
end audio;

architecture control of audio is
  quantity x: real;                     -- stopwatch in sender
  quantity y: real;                     -- stopwatch in receiver
  signal c : integer := 0;
  signal c_2x: std_logic := '0';        -- signals c := 2*c
  signal c_2xplus: std_logic := '0';    -- signals c := 2*c+1
  signal c_one : std_logic := '0';      -- signals c := 1
  signal c_reset : std_logic := '0';    -- resets c
  signal c_2dec : std_logic := '0';     -- signals c := c-2
  signal c_4dec : std_logic := '0';     -- signals c := c-4
  signal leng : integer := 0;
  signal leng_inc : std_logic := '0';   -- increments leng
  signal leng_dec0 : std_logic := '0';  -- decrements leng from out0
  signal leng_dec1 : std_logic := '0';  -- decrements leng from out1
  signal k : std_logic := '0';          -- for parity of string input
  signal m : std_logic := '0';          -- for parity of receiver
  signal list_inA : std_logic := '0';
  signal list_inB : std_logic := '0';
  signal in0A : std_logic := '0';
  signal in0B : std_logic := '0';
  signal in1A : std_logic := '0';
  signal in1B : std_logic := '0';
  signal out0A : std_logic := '0';
  signal out0B : std_logic := '0';
  signal out1A : std_logic := '0';
  signal out1B : std_logic := '0';
  signal upA : std_logic := '0';
  signal upB : std_logic := '0';
  signal headeA : std_logic := '0';
  signal headeB : std_logic := '0';
  signal head0A : std_logic := '0';
  signal head0B : std_logic := '0';
  signal head1A : std_logic := '0';
  signal head1B : std_logic := '0';
  signal outneq0A : std_logic := '0';
  signal outneq0B : std_logic := '0';
  signal outneq1A : std_logic := '0';
  signal outneq1B : std_logic := '0';
  signal y_reset : std_logic := '0';
  signal loc_error : std_logic := '0';  -- if receiver goes to location error
  signal send_flag : std_logic := '0';  -- flag for sender process
  signal in_flag : std_logic := '0';    -- flag for input process

begin  -- control

  break x=> 0.0 when list_inB = '0' or in1A = '0' or in0A = '0';
  break y=> 0.0 when upB = '0' or y_reset = '1';

  x'dot == span(0.95,1.05);
  y'dot == span(0.95,1.05);

  sender: process
  begin                                 -- begin in loc idle
    sync(list_inB,list_inA);            -- go to loc s_start
    sync(in1A,in1B);                    -- go to loc rise_1
    sync(upA,upB);                      -- go to loc transhigh
    while send_flag = '0' loop          -- from loc transhigh
      if head1A = '1' then              -- go to loc transhigh1
        wait until x'above(2.0);
        if head1A = '1' then
          sync(head1B,head1A);          -- go to loc translow1
          wait until x'above(4.0);
          sync(in1A,in1B);              -- go to loc rise_1
          sync(upA,upB);                -- back to loc transhigh or idle
        elsif headeA = '1' then         -- back to loc idle
          sync(headeB,headeA);
          exit;
        end if;
      end if;
      while head0A = '1' loop           -- go to loc transhigh0
        sync(head0B,head0A);
        wait until x'above(4.0);        -- go to loc translow
        sync(in0A,in0B);
        assign(head1B,'1',0,0);
        if head1A = '1' then            -- go to loc translow1
          sync(head1B,head1A);          
          wait until x'above(4.0);      -- go to loc rise_1
          sync(in1A,in1B);
          sync(upA,upB);                -- back to loc transhigh
        elsif headeA = '1' then
          sync(headeB,headeA);
          assign(send_flag,'1',0,0);
          exit;
        end if;
        while head0A = '1' loop         -- sub-loop go to loc translow0
          sync(head0B,head0A);          
          wait until x'above(2.0);      -- go to loc rise_0
          sync(head0B,head0A);
          wait until x'above(2.0);      -- back to loc transhigh0
          sync(upA,upB);
        end loop;
        wait for 1.0 sec;
      end loop;
    end loop;
    vassign(send_flag,'0',0,0);
  end process sender;

  receiver: process
  begin                                 -- begin in loc r_idle
    vassign(y_reset,'0',0,0);        
    while not upA = '1' loop            -- list_in sync from loc r_idle
      if list_inB = '1' then
        sync(list_inA,list_inB);
      end if;
      wait for 1.0 sec;
    end loop;
    sync(upB,upA);                      -- go to loc up_1
    if outneq1A = '1' then              -- go to loc error
      sync(outneq1B,outneq1A);
      assign(loc_error,'1',0,0);
      wait for 500.0 sec;
    end if;
    while out1B = '1' loop              -- go to loc last_is_1
      if m = '1' then                   -- m := ~m
        assign(m,'0',0,0);
      else
        assign(m,'1',0,0);
      end if;
      sync(out1A,out1B);
      if not y'above(3.0) then          -- go to loc error
        sync(upB,upA);
        assign(loc_error,'1',0,0);
        wait for 500.0 sec;
      elsif y'above(3.0) and not y'above(5.0) then  -- go back to loc up_1
        sync(upB,upA);
        exit;
      elsif y'above(5.0) and not y'above(7.0) then  -- go to loc up_0
        sync(upB,upA);
        if outneq0B = '1' then                      -- go to loc error
          sync(outneq0A,outneq0B);
          assign(loc_error,'1',0,0);
          wait for 500.0 sec;
        else
          sync(out0A,out0B);            -- go to loc last_is_0
          if m = '1' then               -- m := ~m
            assign(m,'0',0,0);
          else
            assign(m,'1',0,0);
          end if;
          if not y'above(3.0) then      -- go to loc error
            if not y'above(0.1) then
              sync(outneq0A,outneq0B);
            else
              sync(upB,upA);
            end if;
            assign(loc_error,'0',0,0);
            wait for 500.0 sec;
          elsif y'above(7.0) and not y'above(7.1) then
            if outneq0B = '1' then      -- go to loc error
              sync(outneq0A,outneq0B);
              assign(loc_error,'1',0,0);
              wait for 500.0 sec;
            elsif out0B = '1' then      -- go to loc r_final
              sync(out0A,out0B);
              assign(y_reset,'1',0,0);
              vassign(m,'0',0,0);
              if leng >= 1 then      -- go to loc error
                assign(loc_error,'1',0,0);
                wait for 500.0 sec;
              else
                exit;                   -- go back to loc idle
              end if;
            elsif y'above(5.0) then     -- go to loc next_is_01
              assign(y_reset,'1',0,0);
              if outneq0B = '1' then    -- go to loc error
                sync(outneq0A,outneq0B);
                assign(loc_error,'1',0,0);
                wait for 500.0 sec;
              end if;                   -- go back to loc up_1
            end if;
          else
            if y'above(3.0) and not y'above(5.0) then  -- go to loc up_0
              if outneq0B = '1' then
                sync(outneq0A,outneq0B);
                assign(loc_error,'1',0,0);
                wait for 500.0 sec;
              else
                sync(out0A,out0B);        -- back to loc last_is_0
              end if;
            end if;
          end if;
        end if;
      elsif y'above(9.0) then           -- go to loc r_final
        if not (leng = 0) then
          if m = '1' then
            assign(m,'0',0,0);
            assign(y_reset,'1',0,0);
          end if;
          if m = '0' then
            sync(out0A,out0B);
          end if;
        end if;
        if leng = 0 then             -- go to loc error
          sync(outneq0A,outneq0B);
          assign(loc_error,'1',0,0);
          wait for 500.0 sec;
        end if;
      elsif y'above(7.0) then           -- go to loc next_is_01
        vassign(m,'0',0,0);
        sync(out0A,out0B);
        if outneq0B = '1' then          -- go to loc error
          sync(outneq0A,outneq0B);
          assign(loc_error,'1',0,0);
          wait for 500.0 sec;
        end if;                         -- go to loc up_1
      end if;
    end loop;
  end process receiver;

  input_proc: process
  begin                                 -- begin in loc i_stop
    vassign(in_flag,'0',0,0);
    while not list_inB = '1' loop
      vassign(headeA,'1',0,0);
      wait for 0.5 sec;
      if headeB = '1' then              -- heade sync from loc i_stop
        sync(headeA,headeB);
        wait for 1.0 sec;
      end if;
    end loop;
    vassign(headeA,'0',0,0);
    sync(list_inA,list_inB);            -- go to loc head_is_1
    assign(c_one,'1',0,0);
    assign(c_one,'0',1,1);
    assign(k,'1',0,0);
    assign(leng_inc,'1',0,0);
    assign(leng_inc,'0',1,1);
    while in_flag = '0' loop
      while not in1A = '1' loop
        vassign(head1A,'1',0,0);
        wait for 0.5 sec;
        if head1B  = '1' then
          sync(head1A,head1B);          -- head1 sync from loc head_is_1
        end if;
      end loop;
      vassign(head1A,'0',0,0);
      if in1A = '1' then
        sync(in1B,in1A);
        assign(leng_inc,'1',0,0);
        assign(leng_inc,'0',1,1);
        if k = '1' then                 -- k := ~k
          assign(k,'0',0,0);
        else
          assign(k,'1',0,0);
        end if;
        if span(0.0,1.0) > span(0.0,1.0) then
          assign(c_2x,'1',0,0);        -- go to loc head_is_0
          assign(c_2x,'0',1,1);
          while span(0.0,1.0) > span(0.0,1.0) loop
            assign(head0B,'1',0,0);
            wait for 0.5 sec;
            if head0A = '1' then
              sync(head0A,head0B);      -- head0 sync from loc head_is_0
            end if;
            if in0A = '1' then
              if k = '0' then
                assign(in_flag,'1',0,0);
                exit;
              else
                sync(in0B,in0A);
                if k = '1' then           -- k := ~k
                  assign(k,'0',0,0);
                else
                  assign(k,'1',0,0);
                end if;
                assign(leng_inc,'1',0,0);
                assign(leng_inc,'0',1,1);
                if span(0.0,1.0) > span(0.0,1.0) then
                  assign(c_2x,'1',0,0);
                  assign(c_2x,'0',1,1);
                else
                  assign(c_2xplus,'1',0,0);
                  assign(c_2xplus,'0',1,1);
                end if;
              end if;
            end if;
          end loop;
        else                            -- in1 sync from loc head_is_1
          assign(c_2xplus,'1',0,0);
          assign(c_2xplus,'0',1,1);
        end if;
      end if;
      wait for 1.0 sec;
    end loop;
    sync(in0B,in0A);
    if span(0.0,1.0) > span(0.0,1.0) then  -- go to loc endeven00
      assign(c_2x,'1',0,0);
      assign(c_2x,'0',1,1);
      assign(leng_inc,'1',0,0);
      assign(leng_inc,'0',1,1);
      assign(head0A,'1',0,0);
      wait for 0.5 sec;
      while not in0A = '1' loop
        if head0B = '1' then            -- head0 sync from loc endeven00
          sync(head0A,head0B);
        end if;
        vassign(head0A,'0',0,0);
      end loop;
      sync(in0B,in0A);                  -- go to loc i_stop
    end if;
  end process input_proc;

  out1: process
  begin  -- process out1
    if leng = 1 and c = 1 then
      assign(out1B,'1',0,0);
      wait for 0.5 sec;
      if out1A = '1' then
        sync(out1B,out1A);
        assign(c_reset,'1',0,0);
        assign(c_reset,'0',1,1);
        assign(leng_dec1,'1',0,0);
        assign(leng_dec1,'0',1,1);
      end if;
    elsif leng = 2 and c >= 2 then
      assign(out1B,'1',0,0);
      wait for 0.5 sec;
      if out1A = '1' then
        sync(out1B,out1A);
        assign(c_2dec,'1',0,0);
        assign(c_2dec,'0',1,1);
        assign(leng_dec1,'1',0,0);
        assign(leng_dec1,'0',1,1);
      end if;
    elsif leng = 3 and c >=4 then
      assign(out1B,'1',0,0);
      wait for 0.5 sec;
      if out1A = '1' then
        sync(out1B,out1A);
        assign(c_4dec,'1',0,0);
        assign(c_4dec,'0',1,1);
        assign(leng_dec1,'1',0,0);
        assign(leng_dec1,'0',1,1);
      end if;
    end if;
    vassign(out1B,'0',0,0);
    wait for 1.0 sec;
  end process out1;

  outneq0: process
  begin  -- process outneq0
    if leng = 0 then
      assign(outneq0B,'1',0,0);
      wait for 0.5 sec;
      if outneq0A = '1' then
        sync(outneq0B,outneq0A);
      end if;
    elsif leng = 1 and c = 1 then
      assign(outneq0B,'1',0,0);
      wait for 0.5 sec;
      if outneq0A = '1' then
        sync(outneq0B,outneq0A);
      end if;
    elsif leng = 2 and c >= 2 then
      assign(outneq0B,'1',0,0);
      wait for 0.5 sec;
      if outneq0A = '1' then
        sync(outneq0B,outneq0A);
      end if;
    elsif leng = 2 and c >= 4 then
      assign(outneq0B,'1',0,0);
      wait for 0.5 sec;
      if outneq0A = '1' then
        sync(outneq0B,outneq0A);
      end if;
    elsif leng >= 4 then
      assign(outneq0B,'1',0,0);
      wait for 0.5 sec;
      if outneq0A = '1' then
        sync(outneq0B,outneq0A);
      end if;
    end if;
    vassign(outneq0B,'0',0,0);
    wait for 1.0 sec;
  end process outneq0;

  out0: process
  begin  -- process out0
    if leng = 1 and c = 0 then
      assign(out0B,'1',0,0);
      wait for 0.5 sec;
      if out0A = '1' then
        sync(out0B,out0A);
        assign(leng_dec0,'1',0,0);
        assign(leng_dec0,'0',1,1);
      end if;
    elsif leng = 2 and c <=1 then
      assign(out0B,'1',0,0);
      wait for 0.5 sec;
      if out0A = '1' then
        sync(out0B,out0A);
        assign(leng_dec0,'1',0,0);
        assign(leng_dec0,'0',1,1);
      end if;
    elsif leng = 3 and c <= 3 then
      assign(out0B,'1',0,0);
      wait for 0.5 sec;
      if out0A = '1' then
        sync(out0B,out0A);
        assign(leng_dec0,'1',0,0);
        assign(leng_dec0,'0',1,1);
      end if;
    end if;
    vassign(out0B,'0',0,0);
    wait for 1.0 sec;
  end process out0;

  outneq1: process
  begin  -- process outneq1
    if leng = 0 then
      assign(outneq1A,'1',0,0);
      wait for 0.5 sec;
      if outneq1B = '1' then
        sync(outneq1A,outneq1B);
      end if;
    elsif leng = 1 and c = 0 then
      assign(outneq1A,'1',0,0);
      wait for 0.5 sec;
      if outneq1B = '1' then
        sync(outneq1A,outneq1B);
      end if;
    elsif leng = 2 and c <= 1 then
      assign(outneq1A,'1',0,0);
      wait for 0.5 sec;
      if outneq1B = '1' then
        sync(outneq1A,outneq1B);
      end if;
    elsif leng = 3 and c <= 3 then
      assign(outneq1A,'1',0,0);
      wait for 0.5 sec;
      if outneq1B = '1' then
        sync(outneq1A,outneq1B);
      end if;
    elsif leng >= 4 then
      assign(outneq1A,'1',0,0);
      wait for 0.5 sec;
      if outneq1B = '1' then
        sync(outneq1A,outneq1B);
      end if;
    end if;
    vassign(outneq1A,'0',0,0);
    wait for 1.0 sec;
  end process outneq1;

  c_proc: process (c_2x, c_2xplus, c_one, c_reset, c_2dec, c_4dec)
  begin  -- process c
    if c_2x = '1' then
      int_assign(c,c*2,0,0);
    elsif c_2xplus = '1' then
      int_assign(c,c*2+1,0,0);
    elsif c_one = '1' then
      int_assign(c,1,0,0);
    elsif c_reset = '1' then
      int_assign(c,0,0,0);
    elsif c_2dec = '1' then
      int_assign(c,c-2,0,0);
    elsif c_4dec = '1' then
      int_assign(c,c-4,0,0);
    end if;
  end process c_proc;

  leng_proc: process (leng_inc, leng_dec0, leng_dec1)
  begin  -- process leng
    if leng_inc = '1' then
      int_assign(leng,leng+1,0,0);
    elsif leng_dec0 = '1' or leng_dec1 = '1' then
      int_assign(leng,leng-1,0,0);
    end if;
  end process leng_proc;

end control;
