-------------------------------------------------------------------------------
-- Project: Data Logger with Ethernet UDP output
-- Author(s): DFC Design, s.r.o. (www.dfcdesign.cz)
-- E-Mail:  info@dfcdesign.cz
-- Created: Sept 2011   
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- Copyright (C) 2011 Authors                                                --
--                                                                           --
-- This source file may be used and distributed without                      --
-- restriction provided that this copyright statement is not                 --
-- removed from the file and that any derivative work contains               --
-- the original copyright notice and the associated disclaimer.              --
--                                                                           --
-- This source file is free software; you can redistribute it                --
-- and-or modify it under the terms of the GNU Lesser General                --
-- Public License as published by the Free Software Foundation;              --
-- either version 3.0 of the License, or (at your option) any                --
-- later version.                                                            --
--                                                                           --
-- This source is distributed in the hope that it will be                    --
-- useful, but WITHOUT ANY WARRANTY; without even the implied                --
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                   --
-- PURPOSE.  See the GNU Lesser General Public License for more              --
-- details.                                                                  --
--                                                                           --
-- You should have received a copy of the GNU Lesser General                 --
-- Public License along with this source; If not, see                        --
-- <http://www.gnu.org/licenses/>                                            --
--                                                                           --
------------------------------------------------------------------------------- 

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

Package pkg_probes2fifo Is
 Component probes2fifo Is
   Generic(
      probes_number   : positive := 16;
      timestamp_width : positive := 32
          );
   Port(
      clk         : in  std_logic;
      rst         : in  std_logic;

      DataTrg     : in  std_logic;
      DataIn      : in  std_logic_vector(probes_number - 1 downto 0);
      timestamp   : in  std_logic_vector(timestamp_width - 1 downto 0); 
      
      fifo_data   : out std_logic_vector(probes_number + timestamp_width  -1 downto 0);
      fifo_wrreq  : out std_logic;
      fifo_wrfull : in  std_logic
       );
  End Component;
end package;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


Entity probes2fifo Is
   Generic(
      probes_number   : positive := 16;
      timestamp_width : positive := 32
          );
   Port(
      clk         : in  std_logic;
      rst         : in  std_logic;

      DataTrg     : in  std_logic;
      DataIn      : in  std_logic_vector(probes_number - 1 downto 0);
      timestamp   : in  std_logic_vector(timestamp_width - 1 downto 0); 
      
      fifo_data   : out std_logic_vector(probes_number + timestamp_width  -1 downto 0);
      fifo_wrreq  : out std_logic;
      fifo_wrfull : in  std_logic
       );
End Entity;

Architecture arch Of probes2fifo Is

   constant flg_rst_const         : std_logic_vector(3 downto 0) := x"1";
   constant flg_err_const         : std_logic_vector(3 downto 0) := x"2";

   Type probes2fifo_state_t Is (reset, normal, err);
   signal state_c          : probes2fifo_state_t := normal;

   signal rst_sent         : std_logic := '0';
   signal last_timestamp   : std_logic_vector(timestamp_width - 1 downto 0) := (others => '0');

Begin

   Process(clk)
   Begin
      If clk'Event And clk = '1' Then
         If rst = '1' Then
            state_c <= reset;
            rst_sent <= '0';
         Else
            last_timestamp <= last_timestamp;

            Case state_c Is
               When reset  =>
                  -- If there is a space in fifo
                  -- Send timestamp and reset flag twice
                  If fifo_wrfull = '0' Then
                     rst_sent <= '1';
                     If rst_sent = '1' Then
                        state_c <= normal;
                     End If;
                  End If;
               When normal =>
                  If DataTrg = '1' Then
                     If fifo_wrfull = '1' Then
                        state_c <= err;
                     Else
                        last_timestamp <= timestamp;
                     End If;
                  End If;
               When err    =>
                  If fifo_wrfull = '0' Then
                     state_c <= normal;
                  End If;
            End Case;
         End If;
      End If;
   End Process;

   Process(state_c, fifo_wrfull, rst, timestamp, DataTrg, DataIn, last_timestamp)
   Begin
      If rst = '1' Then
         fifo_wrreq <= '0';
         fifo_data <= (others => '0');
      Else
         fifo_wrreq <= '0';
         Case state_c Is
            When reset  =>
               fifo_data(probes_number + timestamp_width - 1 downto timestamp_width + flg_rst_const'left + 1) <= (others => '0');
               fifo_data(timestamp_width + flg_rst_const'left downto timestamp_width) <= flg_rst_const;
               fifo_data(timestamp_width - 1 downto 0) <= (others => '0');

               fifo_wrreq <= not fifo_wrfull;
            When normal =>
               fifo_data <= DataIn & timestamp;

               fifo_wrreq <= (not fifo_wrfull) and DataTrg;
            When err    =>
               fifo_data((probes_number + timestamp_width - 1) downto (timestamp_width + flg_err_const'left + 1)) <= (others => '0');
               fifo_data(timestamp_width + flg_err_const'left downto timestamp_width) <= flg_err_const;
               fifo_data(timestamp_width - 1 downto 0) <= last_timestamp;

               fifo_wrreq <= not fifo_wrfull;
         End Case;
      End If;
   End Process;


End arch;
