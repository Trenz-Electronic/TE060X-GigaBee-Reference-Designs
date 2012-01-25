----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:42:40 12/09/2007 
-- Design Name: 
-- Module Name:    blinkin - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
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

