--------------------------------------------------------------------------------
-- Copyright (C) 2012 Trenz Electronic
--
-- Permission is hereby granted, free of charge, to any person obtaining a 
-- copy of this software and associated documentation files (the "Software"), 
-- to deal in the Software without restriction, including without limitation 
-- the rights to use, copy, modify, merge, publish, distribute, sublicense, 
-- and/or sell copies of the Software, and to permit persons to whom the 
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included 
-- in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
-- OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
-- IN THE SOFTWARE.
--------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity blinkin is
    Port ( clk : in  STD_LOGIC;
           rst : in  STD_LOGIC;
			  aux_led : out  STD_LOGIC_VECTOR(3 downto 0) := X"0";
           led : out  STD_LOGIC);
end blinkin;

architecture Behavioral of blinkin is

	constant divider : integer := 62500000;
	signal counter : integer := 0;
	signal led_i : std_logic := '0';
	signal lvds_clk : std_logic := '0';
	signal int_clk : std_logic := '0';
	
begin

	led <= led_i;
	int_clk <= clk;      --phy clock  125MHz
	aux_led <= led_i & led_i & led_i & led_i;
	
  process (rst, int_clk)
	begin
		if rst='1' then
			led_i <= '0';
			counter <= 0;
    elsif rising_edge(int_clk) then
	    if counter = divider-1 then
				counter <= 0;
			  led_i <= not led_i;
			else
				counter <= counter + 1;
	    end if;
		end if;
	end process;

end Behavioral;
