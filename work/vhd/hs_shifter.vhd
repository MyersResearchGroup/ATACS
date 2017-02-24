--
-- handshaking model for 1-bit serial shifter
--
-- hs_shifter.vhd
-----------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.handshake.all;


entity hs_shifter is
	port(	
			-- Data to load
			req_load    : in std_logic;
			data_load   : in std_logic;
			ack_load    : inout std_logic:='0';
			-- Data shifted in
			req_shift   : in std_logic;
			shift_value : in std_logic;
			ack_shift   : inout std_logic:='0';
			-- Data value to shift out
			shift_out   : inout std_logic:='0';
			-- Done shifted in
			req_done    : in std_logic;
			dataout     : inout std_logic:='0';
			ack_done    : inout std_logic:='0';
			sig_data    : inout std_logic:='0'
	 	);
end hs_shifter;



architecture behavior of hs_shifter is
--	signal sig_data : std_logic:='0';
begin

	-- Process to shift data in and out of shifter block
	shift : process
	begin
		if (req_load = '1') then     -- Wait until data can be loaded
			if (data_load = '0') then
			    if (sig_data /= '0') then
				assign(sig_data,'0',1,2);
			    end if;
			else
			    if (sig_data /= '1') then
				assign(sig_data,'1',1,2);
			    end if;
			end if;
			assign(ack_load, '1',3,5);   -- Inform that data was received
			guard(req_load, '0');    -- Wait for request to be reset
			assign(ack_load, '0',1,5);   -- Reset acknowledge
		elsif (req_shift = '1') then -- Wait for shift to be requested
			if (sig_data = '0') then
			    if (shift_out /= '0') then
				assign(shift_out,'0',1,2);
			    end if;
			else
			    if (shift_out /= '1') then
				assign(shift_out,'1',1,2);
			    end if;
			end if;

			if (shift_value = '0') then
			    if (sig_data /= '0') then
				assign(sig_data,'0',3,5);
			    end if;
			else
			    if (sig_data /= '1') then
				assign(sig_data,'1',3,5);
			    end if;
			end if;
			assign(ack_shift, '1',1,5);  -- Inform that data was shifted in
			guard(req_shift, '0');   -- Wait for request to shift is reset
			assign(ack_shift, '0',1,5);  -- Reset acknowledge
		elsif (req_done = '1') then  -- Wait until the result is requested
			if (sig_data = '0') then
			    if (dataout /= '0') then
				assign(dataout,'0',1,2);
			    end if;
			else
			    if (dataout /= '1') then
				assign(dataout,'1',1,2);
			    end if;
			end if;
			assign(ack_done, '1',1,5);   -- Inform that data is now on output
			guard(req_done, '0');    -- Wait for req_done to be reset
			assign(ack_done, '0',1,5);   -- Reset acknowledge
		end if;
	end process shift;
end behavior;
	
