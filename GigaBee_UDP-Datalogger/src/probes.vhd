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

Package pkg_probes Is
   Component probes Is
   Generic(
      probes_number   :  natural := 16   -- Number of sensing probes
          );
   Port(
      clk             :  In  Std_logic;
      rst             :  In  Std_logic;
      InProbes        :  In  Std_logic_vector (probes_number - 1 downto 0);
      OutProbes       :  Out Std_logic_vector (probes_number - 1 downto 0);
      Change          :  Out Std_logic
       );
   End Component;
End Package;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


Entity probes Is
   Generic(
      probes_number   :  natural := 16   -- Number of sensing probes
          );
   Port(
      clk             :  In  Std_logic;
      rst             :  In  Std_logic;
      InProbes        :  In  Std_logic_vector (probes_number - 1 downto 0);
      OutProbes       :  Out Std_logic_vector (probes_number - 1 downto 0);
      Change          :  Out Std_logic
       );
End Entity;

Architecture arch Of probes Is

   signal probes_d  :  std_logic_vector(probes_number - 1 downto 0);
   signal probes_dd :  std_logic_vector(probes_number - 1 downto 0);

Begin

   Process(clk)
   Begin
      If clk'Event and clk = '1' Then
         If rst = '1' Then
            probes_d  <= (others => '0');
            probes_dd <= (others => '0');
         Else
            probes_d  <= InProbes;
            probes_dd <= probes_d;
         End If; -- Sync reset
      End If; -- Clock
   End Process;

   Change     <= '0' When probes_d = probes_dd else '1';
   OutProbes  <= probes_d;

End Architecture;
