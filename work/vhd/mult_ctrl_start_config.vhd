-- VHDL configuration created by Hierarchy Editor 1.0
-- Source configuration: multiplier mult4_a_kill_config config 
CONFIGURATION mult_ctrl_start_config OF mult_ctrl_start_top IS
  FOR behavior
    FOR others : mult_ctrl_start
      use entity work.mult_ctrl_start(behavior);
    end for;
    FOR others : mult_ctrl_start_env
      use entity work.mult_ctrl_start_env(behavior);
    end for;
  end for;
end mult_ctrl_start_config;
  
