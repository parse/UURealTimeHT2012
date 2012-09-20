-- Lab 1: Anders Hassis,  Ludvig Norinder & Oskar Wirén

with Ada.Text_IO;
	use Ada.Text_IO;

with Ada.Calendar;
	use Ada.Calendar;

with Ada.Numerics.Float_Random;
	use Ada.Numerics.Float_Random;

procedure part2 is
	start : constant Time := Clock;
	loopStartTime : Time := start;
	runF3 : Boolean := False;
	F3EndTime : Time;

	-- Task Watchdog
	task Watchdog is
		entry Init;
		entry F3Done;
	end Watchdog;

	task body Watchdog is
	begin 
		loop
			accept Init;

			-- Either F3 is done or F3 missed its deadline
			select
				accept F3Done;
			or
				delay until Clock + 0.5;
				Put_Line("F3 missed deadline, RESYNC");
				accept F3Done;
			end select;

		end loop;
	end Watchdog;

	-- Task F1
	procedure F1 is
		now : constant Time := Clock;
	begin 
		Put("F1 started at:");
		Put_Line(Duration'Image(now-start));
	end F1;

	-- Task F2
	procedure F2 is
		now : constant Time := Clock;
	begin 
		Put("F2 started at:");
		Put_Line(Duration'Image(now-start));
	end F2;

	-- Task F3
	procedure F3 is
		now : constant Time := Clock;
		g : generator ;
		num : Float;
	begin 
		Watchdog.Init;
		Reset(g);
		num := Random(g) * 2.0;

		Put("F3 started at:" & Duration'Image(now-start));
		Put_Line(" with delay:" & Float'Image(num));
		
		-- Delay a random amount of seconds and signal the Watchdog that F3 is done
		delay Duration(num);
		Watchdog.F3Done;
	end F3;
	
-- Main block
begin
	loop

		-- Fire off F1 and F2
		F1;
		F2;

		-- Check if F3 is going to run and start F3 after 0.5 seconds
		if (runF3) then
			delay until loopStartTime + 0.5;

			F3;
			F3EndTime := Clock;
			runF3 := False;

			-- If F3 took more than 1 second to run, find next even second for resyncing
			if f3EndTime - loopStartTime > 1.0 then
				loopStartTime := loopStartTime + Duration(Float'Floor(Float(f3EndTime - loopStartTime)));
			end if;
		else
			runF3 := True;
		end if;

		-- Since we used floor above, we add 1 second to our loop start time
		loopStartTime := loopStartTime + 1.0;
		delay until loopStartTime;

	end loop;
end part2;
